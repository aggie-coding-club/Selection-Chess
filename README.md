# Selection Chess GUI and the Hippocrene Engine

Selection chess is a variant of chess where players can dynamically rearrange selections of tiles during their turn. The Hippocrene Engine is an engine that shares source code with the GUI, and is an engine which can generate moves. The GUI allows engines to play eachother or humans, and uses established conventions so other engines can be plugged in if compatible.

## Contribution

This is currently a learners project for the Aggie Coding Club, so we are not looking for outside help at this time. This is more intended to be something we can work on to build cool algorithms in our spare time. This is subject to change in the future, whether it be anything from abandonment to fully open-sourcing.

### How to Compile

We use [SCons](https://www.scons.org/). Once you have SCons properly installed on your machine, you can compile both the GUI and engine by running `scons` inside of the project folder.
Running `scons -c` will cleanup.

To compile with debug outputs enabled, run `scons debug=1`

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
while (i = 0; foo(i); i++); // get lowest value for which foo returns true
return i;
```

### Naming Conventions

We use `.cpp`, `.hpp`, and `.h` in this project. Specifically, `.hpp` implies that it is a header file with implementation, whereas every `.h` corresponds to a matching `.cpp`. Source code files are named in lower_snake_case.

Parameters of a function start with an underscore `_`, and members of a class start with `m_`. Constants use UPPER_SNAKE_CASE. Classes, structs, enums, functions, and methods use UpperCamelCase. Variables use lowerCamelCase.
