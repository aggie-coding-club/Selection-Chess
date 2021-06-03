import os
import configparser

configPath = "scons.config"
compiled_path = "./compiled/" # This folder is where the final product goes

print("Reading config file '", configPath, "'", sep='')
config = configparser.RawConfigParser()
config.read(configPath)

env = Environment()

# TODO: Consider going the way of Godot, and using 'platform=windows' style vernacular
compiler = config.get("my-config", "compiler").strip('"')
if (compiler == "msvc"):
    print("Using MSVC compiler")
    env.Append(CPPFLAGS = '/EHsc ')
elif (compiler == "gcc"):
    print("Using g++ compliler")
else:
    raise Exception ("Unrecognized compiler '" + compiler + "'")

noBoost = ARGUMENTS.get('noboost', 0)
if (noBoost) :
    env.Append(CPPDEFINES=['NO_BOOST'])
else:
    boost_prefix = config.get("my-config", "boostPath")
    env.Append(CPPPATH = [boost_prefix])
    env.Append(LIBPATH = [os.path.join(boost_prefix, "stage/lib")])
    # note: if you are missing static library after compiling, compile again using '.\b2 runtime-link=static'

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    # We actually set the value of DEBUG to the numeric value, in case we want to use bit flags later
    env.Append(CPPDEFINES=['DEBUG=' + str(debug)])
    # if (compiler == "msvc"): # TODO: I don't know how to use MSVC lol, need to figure it out better
    #     env.Append(CPPFLAGS = '/RTC ')
    #     env.Append(CPPFLAGS = '/EHr ')


# make sure these strings have trailing space so multiline string doesn't merge last item of this line with first item of next line
guiCpps = '' \
'gui_main.cpp utils.cpp ' \
'array_board.cpp board.cpp ' \
'game.cpp move.cpp pieces.cpp ruleset.cpp ' \
'human_runner.cpp cmd_tokenizer.cpp '
if (not noBoost):
    guiCpps += 'engine_runner.cpp ' # compile with engine_runner only if we can use Boost

gui = env.Program(compiled_path + 'SelChessGui', Split(guiCpps))

engine = env.Program(compiled_path + 'engines/Hippocrene', Split(
    'hippo_main.cpp utils.cpp '
    'array_board.cpp board.cpp '
    'game.cpp move.cpp pieces.cpp ruleset.cpp '
    'min_max.cpp'
))
