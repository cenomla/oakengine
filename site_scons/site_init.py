import os
import fnmatch
import re

#config
bin_dir = 'bin'
obj_dir = 'bin/obj'

env_flavor_sets = {
	'_common': dict(
		CC = "gcc",
		CXX = "g++",
	),
	'debug': dict(
		OBJDIR = os.path.join(obj_dir, 'debug'),
		BINDIR = os.path.join(bin_dir, 'debug'),
	),
	'release': dict(
		OBJDIR = os.path.join(obj_dir, 'release'),
		BINDIR = os.path.join(bin_dir, 'release')
	),
}

env_flavor_adds = {
	'_common': dict(
		CFLAGS = ['-std=c99', '-Wall'],
		CXXFLAGS = ['-std=c++1z', '-Wall', '-fno-exceptions', '-fno-rtti', '-Wno-pragmas'],
	),
	'debug': dict(
		CFLAGS = ['-g', '-DDEBUG'],
		CXXFLAGS = ['-g', '-DDEBUG'],
	),
	'release': dict(
		CFLAGS = ['-O3', '-DNDEBUG'],
		CXXFLAGS = ['-O3', '-DNDEBUG'],
	),
}

def modules():
	yield 'lib'
	yield 'core'
	yield 'sandbox'
	yield 'tests'

def flavors():
    for flavor in set(env_flavor_sets.keys() + env_flavor_adds.keys()):
        if not flavor.startswith('_'):
            yield flavor

#environment functions

def get_env_base():
	env = Environment()
	env.SConscriptChdir(0)
	
	env.flavors = (set(flavors()).intersection(COMMAND_LINE_TARGETS) or flavors())

	env.Replace(**env_flavor_sets['_common'])
	env.Append(**env_flavor_adds['_common'])
	return env

def get_env_flavor(env, flavor):
	flv = env.Clone()

	flv['targets'] = {}
	flv.get_targets = lambda *args, **kwargs: get_targets(flv, *args, **kwargs)
	flv.set_target = lambda name: set_target(flv, name)
	flv.Replace(**env_flavor_sets[flavor])
	flv.Append(**env_flavor_adds[flavor])

	return flv

def process_flavor(env_base, flavor): 

	print 'processing flavor: ',flavor,'...'

	env = get_env_flavor(env_base, flavor)

	for module in modules():
		scons_path = os.path.join(module, "SCsub")
		assert os.path.isfile(scons_path)
		print 'reading module: ',module,'...'

		targets = env.SConscript(scons_path, variant_dir = os.path.join('$OBJDIR', module + '.d'), duplicate = 0, exports = {'env': env})

		if targets:
			for target_name in targets:
				target_key = '%s::%s' % (module, target_name)
				assert target_key not in env['targets']
				env['targets'][target_key] = targets[target_name]
				for target in targets[target_name]:
					env.InstallAs(source = target, target = os.path.join('$BINDIR', '%s/%s' % (module, os.path.basename(str(target)))))

	Alias(flavor, env['BINDIR'])

#helper functions
def get_sources_recursive(file_list, source, regex, rel=''):
	for f in os.listdir(source + rel):
		if fnmatch.fnmatch(f, regex):
			file_list += [rel + f]
		if os.path.isdir(source + rel + f):
			get_sources_recursive(file_list, source, regex, rel + f + '/')

def get_targets(env, *args, **kwargs):
    """Return list of target nodes for given target name queries.
    Every positional argument is a singe query.
    Supported query formats:
    1. Fully-Qualified "Module::Target" name queries.
       Matches exact target entries.
    2. Target-name-only queries (no "::" in query).
       Matches all targets with that name, potentially from multiple modules.
       In case of multi-module matches, a warning will be printed.
    3. Wildcard queries (containing "*" in the query).
       Matches all targets whose fully-qualified Module::Target name
       matches the wildcard expression.
       No warning is printed for multiple matches.
    Optionally, pass a keyword argument "no_multi_warn=True" to suppress
    warning messages for unexpected multiple matches for a query.
    Warning messages are always printed when a query results zero matches.
    """
    no_multi_warn = kwargs.pop('no_multi_warn', False)
    def query_to_regex(query):
        """Return RegEx for specified query `query`."""
        # Escape query string
        query = re.escape(query)
        if r'\*' in query:  # '\' because of RE escaping
            # It's a wildcard query
            return re.compile('^%s$' % (query.replace('\\*', '.*'))), False
        if r'\:\:' in query:  # '\' because of RE escaping
            # It's a fully-qualified "Module::Target" query
            return re.compile('^%s$' % (query)), True
        # else - it's a target-name-only query
        return re.compile(r'^[^\:]*\:{2}%s$' % (query)), True
    target_names = set(env['targets'].keys())
    matching_target_names = list()
    for query in args:
        # Remove matched target names to avoid scanning them again
        target_names = target_names.difference(matching_target_names)
        qre, warn = query_to_regex(query)
        match_count = 0
        for target_name in target_names:
            if qre.match(target_name):
                matching_target_names.append(target_name)
                match_count += 1
        # Warn about unexpected scenarios
        if 0 == match_count:
            # No matches for query probably means typo in query
            print ('scons: warning: get_targets query "%s" had no matches' %
                   (query))
        elif warn and (not no_multi_warn) and (1 < match_count):
            # Multiple matches for a "warnable" query might indicate
            #  a too-broad query.
            print ('scons: warning: get_targets query "%s" had %d matches' %
                   (query, match_count))
    # Aggregate all matching target lists and return a single list of targets
    return reduce(lambda acculist, tname: acculist + env['targets'][tname],
                  matching_target_names, [])
