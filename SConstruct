#!python
import os, subprocess, configparser

configPath = "scons.config"
compiled_path = "./compiled/" # This folder is where final products goes (excluding GDScript library files)

print("Reading config file '", configPath, "'", sep='')
config = configparser.RawConfigParser()
config.read(configPath)

defaultPlatform = config.get("default", "platform").strip('"')
defaultUseLLVM = 'no'
if defaultPlatform == 'linux':
    defaultUseLLVM = config.get("linux", "use_llvm").strip('"')

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options
opts.Add(EnumVariable('target', "Compilation target", 'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", defaultPlatform, ['', 'windows', 'linux', 'osx']))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'", '', ['', 'windows', 'linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", defaultUseLLVM))
opts.Add(BoolVariable('use_boost', "Enable/disable functionality that depends on Boost library", 'yes'))
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
Help(opts.GenerateHelpText(env))

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['p'] != '':
    env['platform'] = env['p']

if env['platform'] == '':
    print("\nNo valid target platform selected.\n")
    # Put everything into big if-else because doing quit() doesn't allow help menu to show

else:
    print("Platform =",  env['platform'], ("(llvm)" if  env['use_llvm'] else ''))
    # Check our platform specifics
    if env['platform'] == "osx":
        env['gdnl_path'] += 'osx/'
        godot_cpp_lib += '.osx'
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-g','-O2', '-arch', 'x86_64', '-std=c++17'])
            env.Append(LINKFLAGS = ['-arch', 'x86_64'])
        else:
            env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64', '-std=c++17'])
            env.Append(LINKFLAGS = ['-arch', 'x86_64'])

    elif env['platform'] == "linux":
        env['gdnl_path'] += 'x11/'
        godot_cpp_lib += '.linux'
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
        else:
            env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])

    elif env['platform'] == "windows":
        env['gdnl_path'] += 'win64/'
        godot_cpp_lib += '.windows'
        # This makes sure to keep the session environment variables on windows,
        # that way you can run scons in a vs 2017 prompt and it will find all the required tools
        env.Append(ENV = os.environ)

        env.Append(CCFLAGS = ['-DWIN32', '-D_WIN32', '-D_WINDOWS', '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS'])
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '-MDd'])
        else:
            env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD'])

    if env['target'] in ('debug', 'd'):
        godot_cpp_lib += '.debug'
    else:
        godot_cpp_lib += '.release'

    godot_cpp_lib += '.' + str(bits)

    # make sure our binding library is properly includes
    env.Append(CPPPATH=['.', godot_headers_path, godot_cpp_bindings_path + 'include/', godot_cpp_bindings_path + 'include/core/', godot_cpp_bindings_path + 'include/gen/'])
    env.Append(LIBPATH=[godot_cpp_bindings_path + 'bin/'])

    if env['use_boost']:
        boost_prefix = config.get( env['platform'], "boostIncludePath")
        env.Append(CPPPATH = [boost_prefix])
        boost_lib = config.get(env['platform'], "boostLibPath")
        # note to windows users: if you are missing static library after compiling, compile again using '.\b2 runtime-link=static'
        env.Append(LIBPATH=[os.path.join(boost_lib)])
        if ( env['platform'] == "linux"):
            env.Append(LIBS = "pthread") # it seems boost depends on pthread
    env.Append(LIBS=[godot_cpp_lib])

    if env['target'] in ('debug', 'd'):
        env.Append(CPPDEFINES=['DEBUG'])

    # tweak this if you want to use different folders, or more folders, to store your source code in.
    env.Append(CPPPATH=['src/godot/'])
    sources = Glob('src/godot/*.cpp')

    library = env.SharedLibrary(target=env['gdnl_path'] + env['gdnl_name'] , source=sources)

    #Default(library)

