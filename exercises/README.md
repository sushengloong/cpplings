# Exercises

Files in this directory are the tracked exercise templates. Do not use them for
learner solutions.

Run the runner once to create editable copies under `.cpplings/work/exercises/`,
then open the path printed by `cpplings`:

```sh
./build/cpplings
./build/cpplings run hello_01
```

Most exercises include assertions in `main`. Treat those assertions as the
specification for the behavior you need to implement.

New `.cpp` files are auto-detected by the runner. Add a line to
`manifest.txt` when an exercise needs a custom hint, C++ standard, or exact
curriculum position.
