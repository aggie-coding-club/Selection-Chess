import os
import configparser

configPath = "scons.config"
compiled_path = "./compiled/" # This folder is where the final product goes

print("Reading config file '", configPath, "'", sep='')
config = configparser.RawConfigParser()
config.read(configPath)

env = Environment()
libPath = []

# TODO: Consider going the way of Godot, and using 'platform=windows' instead of compiler
compiler = ARGUMENTS.get('compiler', "default")
print("Got compiler as default")
if (compiler == "default"):
    compiler = config.get("default", "compiler").strip('"')

if (compiler == "msvc"):
    print("Using MSVC compiler")
    env.Append(CPPFLAGS = '/EHsc ')
    # env.Append(CPPDEFINES=['USING_WINDOWS'])
elif (compiler == "gcc"):
    print("Using g++ compliler")
    # env.Append(CPPDEFINES=['USING_NIX'])
else:
    raise Exception ("Unrecognized compiler '" + compiler + "'")

noBoost = ARGUMENTS.get('noboost', 0)
if (noBoost) :
    env.Append(CPPDEFINES=['NO_BOOST'])
else:
    boost_prefix = config.get(compiler, "boostIncludePath")
    env.Append(CPPPATH = [boost_prefix])
    boost_lib = config.get(compiler, "boostLibPath")
    # note to windows users: if you are missing static library after compiling, compile again using '.\b2 runtime-link=static'
    libPath.append(os.path.join(boost_lib))
    if (compiler == "gcc"):
        env.Append(LIBS = "pthread") # it seems boost depends on pthread

debug = ARGUMENTS.get('debug', 0)
if int(debug):
    # We actually set the value of DEBUG to the numeric value, in case we want to use bit flags later
    env.Append(CPPDEFINES=['DEBUG=' + str(debug)])
    # if (compiler == "msvc"): # TODO: I don't know how to use MSVC lol, need to figure it out better
    #     env.Append(CPPFLAGS = '/RTC ')
    #     env.Append(CPPFLAGS = '/EHr ')
    if (compiler == "gcc"):
        env.Append(CPPFLAGS = '-g')

env.Append(LIBPATH = libPath)

# make sure these strings have trailing space so multiline string doesn't merge last item of this line with first item of next line
guiCpps = '' \
'src/interface/gui_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp ' \
'src/chess/array_board.cpp src/chess/board.cpp ' \
'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp ' \
'src/interface/human_runner.cpp src/chess/cmd_tokenizer.cpp '
if (not noBoost):
    guiCpps += 'src/interface/engine_runner.cpp ' # compile with engine_runner only if we can use Boost

gui = env.Program(compiled_path + 'SelChessGui', Split(guiCpps))

engine = env.Program(compiled_path + 'engines/Hippocrene', Split(
    'src/engine/hippo_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp '
    'src/chess/array_board.cpp src/chess/board.cpp '
    'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp '
    'src/engine/min_max.cpp '
    'src/chess/cmd_tokenizer.cpp'
))

unittest = env.Program(compiled_path + 'UnitTest', Split(
    'src/testing/test_macros.cpp '
    'src/testing/unit_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp '
    'src/chess/array_board.cpp src/chess/board.cpp '
    'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp '
    'src/engine/min_max.cpp '
    'src/interface/human_runner.cpp '
    'src/chess/cmd_tokenizer.cpp'
))
