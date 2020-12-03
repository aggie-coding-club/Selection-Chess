env = Environment()

compiled_path = "./compiled/" # This folder is where the final product goes

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    # We actually set the value of DEBUG to the numeric value, in case we want to use bit flags later
    env.Append(CPPDEFINES=['DEBUG=' + str(debug)])

gui = env.Program(compiled_path + 'SelChessGui', Split('gui_main.cpp board.cpp utils.cpp'))
engine = env.Program(compiled_path + 'engines/Hippocrene', Split('hippo_main.cpp board.cpp utils.cpp'), LIBS=['user32']) #TODO: do we actually need this library?

# test1 = env.Program('Parent', Split('parent.cpp'))
# test2 = env.Program('Child', Split('child.cpp'), LIBS=['user32'])

engrunnertest = env.Program(compiled_path + 'EngRunner', Split('engine_runner.cpp board.cpp utils.cpp'))
