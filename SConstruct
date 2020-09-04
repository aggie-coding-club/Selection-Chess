env = Environment()

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    # We actually set the value of DEBUG to the numeric value, in case we want to use bit flags later
    env.Append(CPPDEFINES=['DEBUG=' + str(debug)])

gui = env.Program('SelChessGui', Split('gui_main.cpp board.cpp utils.cpp'))
engine = env.Program('Hippocrene', Split('hippo_main.cpp board.cpp utils.cpp'))
