## Build:

```
$ cmake -E make_directory build
$ cmake -E chdir build cmake .. -Dtest=ON
$ cmake --build build
$ cmake -E chdir build ctest --verbose

```
