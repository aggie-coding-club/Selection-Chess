# Selection Chess GUI and the Hippocrene Engine

Selection Chess is a variant of chess where players can dynamically rearrange selections of tiles during their turn. The Hippocrene Engine is an engine that shares source code with the Interface, and is an engine which can generate moves. The Interface allows engines to play each other or humans, and uses established conventions so other engines can be plugged in if compatible.

## Contribution

This is currently a learners project for the Aggie Coding Club, so we are not looking for outside help at this time. This is more intended to be something we can work on to build cool algorithms in our spare time. This is subject to change in the future, whether it be anything from abandonment to fully open-sourcing.

### Getting Started

We have a [document on how to get started](
https://docs.google.com/document/d/1t32vdaahksmd2ImCNaXNv0qEbKwnHjy0MOYP9bsr5Sg/edit?usp=sharing). It will be updated as the project progresses.

## How to Compile

We use [SCons](https://www.scons.org/) as our build system.

### TL;DR

If you just want to compile Hippocrene engine out of the box, run the following:

```scons platform=[your platform] engine=yes```

### Platform

You must specify which platform you are compiling with. This can be done with the `platform` parameter. Can be `windows`, `linux`, or `osx`.

* WSL users should use `platform=linux`. 
* Linux users who want to use Clang instead of GCC can include `use_llvm=yes`.

If none is specified, it will use the default in your scons.config file.

### What to compile

There are 3 items that SCons can compile. SCons will only compile each of them if told via these parameters:

* `engine=yes` will compile the Hippocrene engine.
* `unit=yes` will compile the Unit Test executable.
* `interface=yes` will compile the GDNative library used by Interface.
* `all=yes` will compile all of the above.

### Config file

Using the scons.config file is recommended. Copy and rename `example.scons.config` to `scons.config` and change the relevant settings.
This will allow you to omit the `platform` parameter. Additionally, it is required for linking the Boost Library.

### Boost

We use `Boost.Process` in our Interface library, therefore require Boost to compile it. **Hippocrene and Unit Test do not require Boost to compile**. Additionally, you can compile a limited version of the interface without Boost using the `use_boost=no` parameter. See the Getting Started doc for details on how to install and set up Boost.

### Other useful commands

Running `scons -c all=yes` will remove ('clean') all compiled objects.

The `target=release` parameter can be used for release builds, as opposed to the default debug builds.

Run `scons --help` for complete list of flags and what they do.

## Standards

Two space indentation. [The One True Brace Style](https://en.wikipedia.org/wiki/Indentation_style#Variant:_1TBS_(OTBS)) is used for bracing, as shown below:

```cpp
void foo(std::string bar, int baz) {
  if (m_qux) {
    std::cout << "hello world " << baz << std::endl;
  } else {
    std::cout << bar << std::endl;
  }
}
```

Notice that braces are required even for one line `if` or other control statements. One exception to this is control blocks with no implementation, such as:

```cpp
size_t i;
while (i = 0; foo(i); i++); // get lowest value for which foo returns false
return i;
```

### Naming Conventions

We use `.cpp`, `.hpp`, and `.h` in this project. Specifically, `.hpp` implies that it is a header file with implementation, whereas every `.h` corresponds to a matching `.cpp`. Source code files are named in lower_snake_case.

Parameters of a function start with an underscore `_`, and members of a class start with `m_`. Constants use UPPER_SNAKE_CASE. Classes, structs, enums, functions, and methods use UpperCamelCase. Variables use lowerCamelCase.
