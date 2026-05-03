# Cpplings

Cpplings is a Rustlings-style exercise runner for learning modern C++.

Each exercise is a small C++ program that starts out broken. The checked-in
files under `exercises/` are templates; the runner creates ignored working copies
under `.cpplings/work/exercises/` for learners to edit. Open the working copy,
read the comments, fix the code, then ask the runner to compile and execute it.
Most exercises use `assert` for their checks, so a passing exercise is just a
program that compiles and exits successfully.

## Build

```sh
make
```

Or, with CMake:

```sh
cmake -S . -B build
cmake --build build
```

## Use

```sh
./build/cpplings              # show the next exercise
./build/cpplings init         # create missing editable exercise copies
./build/cpplings list         # show the curriculum and working files
./build/cpplings run hello_01 # compile and run one exercise
./build/cpplings hint         # hint for the next exercise
./build/cpplings verify       # check exercises in order
./build/cpplings reset hello_01 # reset one working copy from its template
./build/cpplings watch        # rerun verify when exercise files change
```

An exercise is considered intentionally unfinished while it contains:

```cpp
// I AM NOT DONE
```

Remove that marker from the working copy when you think the exercise is solved.
The runner will still compile and run the file, so removing the marker is not
enough by itself.

The `.cpplings/` directory is ignored by Git, which keeps learner solutions out
of commits while preserving the starter curriculum in `exercises/`.

## Curriculum

The curriculum lives in [exercises/manifest.txt](exercises/manifest.txt). Each
line has:

```text
name|path|standard|hint
```

The runner currently ships with a compact first pass over C++ basics:

- hello world
- variables and references
- functions
- control flow
- strings
- vectors
- structs
- classes
- templates
- RAII

Add more exercises by creating a C++ template file and adding it to the manifest.
Existing users will receive the new exercise as a working copy the next time they
run `cpplings`.
