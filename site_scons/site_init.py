import os
import fnmatch


#helper functions
def get_sources_recursive(file_list, source, regex, rel=''):
	for f in os.listdir(source + rel):
		if fnmatch.fnmatch(f, regex):
			file_list += [rel + f]
		if os.path.isdir(source + rel + f):
			get_sources_recursive(file_list, source, regex, rel + f + '/')

def set_target(name, build_dir):
	return os.path.join('#%s' % build_dir, name);