import os
import configparser

configPath = "scons.config"
compiled_path = "./compiled/" # This folder is where the final product goes

print("Reading config file '", configPath, "'", sep='')
config = configparser.RawConfigParser()
config.read(configPath)

env = Environment()

compiler = config.get("my-config", "compiler").strip('"')
if (compiler == "msvc"):
    print("Using MSVC compiler")
    env.Append(CPPFLAGS = '/EHsc')
else:
    raise Exception ("Unrecognized compiler '" + compiler + "'")

boost_prefix = config.get("my-config", "boostPath")

env.Append(CPPPATH = [boost_prefix])
env.Append(LIBPATH = [os.path.join(boost_prefix, "stage/lib")])
# note: if you are missing static library after compiling, compile again using '.\b2 runtime-link=static'

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    # We actually set the value of DEBUG to the numeric value, in case we want to use bit flags later
    env.Append(CPPDEFINES=['DEBUG=' + str(debug)])

gui = env.Program(compiled_path + 'SelChessGui', Split('gui_main.cpp board.cpp utils.cpp engine_runner.cpp human_runner.cpp tokenizer.cpp'))
engine = env.Program(compiled_path + 'engines/Hippocrene', Split('hippo_main.cpp board.cpp utils.cpp'), LIBS=['user32']) #TODO: do we actually need this library?

# test1 = env.Program('Parent', Split('parent.cpp'))
# test2 = env.Program('Child', Split('child.cpp'), LIBS=['user32'])
