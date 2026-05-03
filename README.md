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

Run tests:

```sh
make test
```

Format and lint:

```sh
make format
make format-check
make lint
```

Or, with CMake:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Use

```sh
./build/cpplings              # show the next exercise
./build/cpplings init         # create missing editable exercise copies
./build/cpplings list         # show the curriculum and working files
./build/cpplings progress     # show marked-done progress
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
of commits while preserving the starter curriculum in `exercises/`. Progress is
based on working copies where the `// I AM NOT DONE` marker has been removed;
`verify` still compiles and runs the code as the real test.

## Curriculum

The curriculum templates live under `exercises/`. The runner auto-detects
`exercises/**/*.cpp` files. For stable ordering, custom standards, and hints,
add entries to [exercises/manifest.txt](exercises/manifest.txt). Each manifest
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

Add more exercises by creating a C++ template file under `exercises/`. The
runner will auto-detect it, append it after manifest-listed exercises, and create
a working copy the next time users run `cpplings`. Add a manifest entry when you
want a custom hint, C++ standard, or curriculum position.
