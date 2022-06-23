#!python
import os, subprocess, configparser

configPath = "scons.config"
compiled_path = "./compiled/" # This folder is where final products goes (excluding GDScript library files)

# Default values to use if there is not a scons.config file 
defaultPlatform = ''
defaultUseLLVM = 'no'
defaultUseBoost = 'no'
config = None

if os.path.isfile(configPath):
    print("Reading config file '", configPath, "'", sep='')
    config = configparser.RawConfigParser()
    config.read(configPath)

    defaultPlatform = config.get("default", "platform").strip('"')
    if defaultPlatform == 'linux':
        defaultUseLLVM = config.get("linux", "use_llvm").strip('"')

    defaultUseBoost = 'yes'
else:
    print(">> No scons.config file found! Using command-line parameters only...")


opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options
opts.Add(BoolVariable('all', "Set yes to compile all targets (unit, engine, & interface)", 'no'))
opts.Add(BoolVariable('unit', "Set yes to compile UnitTest executable", 'no'))
opts.Add(BoolVariable('engine', "Set yes to compile Hippocrene Engine executable", 'no'))
opts.Add(BoolVariable('interface', "Set yes to compile GDNative library for Interface", 'no'))

opts.Add(EnumVariable('target', "Compile targets in debug or release mode", 'debug', ['debug', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", defaultPlatform, ['', 'windows', 'linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", defaultUseLLVM))
opts.Add(BoolVariable('use_boost', "Enable/disable functionality that depends on Boost library", defaultUseBoost))
opts.Add(PathVariable('gdnl_path', "The path where the interface's GDNative lib is output.", 'godot_project/bin/'))
opts.Add(PathVariable('gdnl_name', "Output name of interface's GDNative library.", 'libselchess', PathVariable.PathAccept))

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot-cpp/godot-headers/"
godot_cpp_bindings_path = "godot-cpp/"
godot_cpp_lib = "libgodot-cpp"

# only support 64 at this time..
bits = 64

# Updates the environment with the option variables.
opts.Update(env)

# Generates help for the -h scons option.
Help("\nSel Chess Compiler Options:\n---------------------------")
Help(opts.GenerateHelpText(env))

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['all']:
    env['unit'] = True
    env['engine'] = True
    env['interface'] = True

# Main function of this script
def compile():
# ------- First, do the things that are common to all compiled targets ------- #
    print("Platform =",  env['platform'], ("(llvm)" if  env['use_llvm'] else ''))
    # Check our platform specifics
    if env['platform'] == "osx":
        if env['target'] == 'debug':
            env.Append(CCFLAGS = ['-g','-O2', '-arch', 'x86_64', '-std=c++17'])
            env.Append(LINKFLAGS = ['-arch', 'x86_64'])
        else:
            env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64', '-std=c++17'])
            env.Append(LINKFLAGS = ['-arch', 'x86_64'])

    elif env['platform'] == "linux":
        if env['target'] == 'debug':
            env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
        else:
            env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])

    elif env['platform'] == "windows":
        # This makes sure to keep the session environment variables on windows,
        # that way you can run scons in a vs 2017 prompt and it will find all the required tools
        env.Append(ENV = os.environ)

        env.Append(CCFLAGS = ['-DWIN32', '-D_WIN32', '-D_WINDOWS', '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS'])
        if env['target'] == 'debug':
            env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '-MDd'])
        else:
            env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD'])

    if env['target'] == 'debug':
        env.Append(CPPDEFINES=['DEBUG'])

# ----------------------- Now do target-specific stuff ----------------------- #
    if env['unit']:
        compile_unittest_exe()
    if env['engine']:
        compile_hippo_exe()
    if env['interface']:
        compile_godot_lib()
    if not (env['unit'] or env['engine'] or env['interface']):
        print("ERR: You must select at least one target to compile!\nFor example, `scons engine=yes`\n")
        quit()

def compile_unittest_exe():
    global env
    # make sure these strings have trailing space so multiline string doesn't merge last item of this line with first item of next line
    unittest = env.Program(compiled_path + 'UnitTest', Split(
        'src/testing/test_macros.cpp '
        'src/testing/unit_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp '
        'src/chess/array_board.cpp src/chess/board.cpp '
        'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp '
        'src/engine/min_max.cpp '
        'src/interface/human_runner.cpp '
        'src/chess/cmd_tokenizer.cpp'
    ))

def compile_hippo_exe():
    global env
    engine = env.Program(compiled_path + 'engines/Hippocrene', Split(
        'src/engine/hippo_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp '
        'src/chess/array_board.cpp src/chess/board.cpp '
        'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp '
        'src/engine/min_max.cpp '
        'src/chess/cmd_tokenizer.cpp'
    ))


def compile_godot_lib():
    global env, godot_cpp_lib
    # Check our platform specifics
    if env['platform'] == "osx":
        env['gdnl_path'] += 'osx/'
        godot_cpp_lib += '.osx'
    elif env['platform'] == "linux":
        env['gdnl_path'] += 'x11/'
        godot_cpp_lib += '.linux'
    elif env['platform'] == "windows":
        env['gdnl_path'] += 'win64/'
        godot_cpp_lib += '.windows'

    if env['target'] == 'debug':
        godot_cpp_lib += '.debug'
    else:
        godot_cpp_lib += '.release'

    godot_cpp_lib += '.' + str(bits)
    env.Append(LIBS=[godot_cpp_lib])

    # make sure our binding library is properly includes
    env.Append(CPPPATH=['.', godot_headers_path, godot_cpp_bindings_path + 'include/', godot_cpp_bindings_path + 'include/core/', godot_cpp_bindings_path + 'include/gen/'])
    env.Append(LIBPATH=[godot_cpp_bindings_path + 'bin/'])

    if env['use_boost']:
        if config is None:
            print("ERR: Cannot compile with Boost without a scons.config file!")
            quit()
        boost_prefix = config.get( env['platform'], "boostIncludePath")
        env.Append(CPPPATH = [boost_prefix])
        boost_lib = config.get(env['platform'], "boostLibPath")
        # note to windows users: if you are missing static library after compiling, compile again using '.\b2 runtime-link=static'
        env.Append(LIBPATH=[os.path.join(boost_lib)])
        if ( env['platform'] == "linux"):
            env.Append(LIBS = "pthread") # it seems boost depends on pthread
    else:
        # This macro is used by source to remove boost-dependent code
        env.Append(CPPDEFINES=['NO_BOOST'])

    # tweak this if you want to use different folders, or more folders, to store your source code in.
    env.Append(CPPPATH=['src/godot/'])
    sources = Glob('src/godot/*.cpp')

    guiCpps = '' \
    'src/interface/gui_main.cpp src/utils/utils.cpp src/chess/chess_utils.cpp ' \
    'src/chess/array_board.cpp src/chess/board.cpp ' \
    'src/chess/game.cpp src/chess/move.cpp src/chess/pieces.cpp src/chess/ruleset.cpp ' \
    'src/interface/human_runner.cpp src/chess/cmd_tokenizer.cpp '
    if env['use_boost']:
        guiCpps += 'src/interface/engine_runner.cpp ' # compile with engine_runner only if we can use Boost

    gui = env.Program(compiled_path + 'SelChessGui', Split(guiCpps))

    library = env.SharedLibrary(target=env['gdnl_path'] + env['gdnl_name'] , source=sources)

if env['platform'] == '':
    print("\nNo valid target platform selected. Try `scons platform=[platform]` or add a scons.config file.\nType `scons --help` for more parameters.\n")
else:
    compile()
