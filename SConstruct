import os

def modules():
	yield 'lib'
	yield 'core'
	yield 'sandbox'
	yield 'tests'

build_dir = 'bin'

env = Environment()
env.Append(LIBPATH = ['#%s' % build_dir, '#%s/lib' % build_dir])

env['targets'] = dict()

env.set_target = lambda name: set_target(name, build_dir)
env.SConscriptChdir(0)

for module in modules():
	scons_path = os.path.join(module, "SCsub")
	assert os.path.isfile(scons_path)
	print 'reading module: ',module,'...'

	targets = env.SConscript(scons_path, variant_dir = os.path.join(build_dir, module + '.d'), duplicate = 0, exports = {'env': env})

	if targets:
		for target_name in targets:
			target_key = '%s.%s' % (module, target_name)
			assert target_key not in env['targets']
			env['targets'][target_key] = targets[target_name]