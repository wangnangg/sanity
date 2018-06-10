# sanity
A c++ library for easy access of various math functions.



# Info

## Profiling
gprof or valgrind can be used to profile.

### gprof
Compile and link the program with `-pg` and `-no-pie` flags. This can be done by:

```
make config=profile utest
```

Execute the binary:

```
build/profile/utest --gtest_filter="your-filter"
```

After execution, a file named gmon.out will be in current directory. Interpret this file with gprof:

```
gprof build/profile/utest gmon.out
```

The results will be printed in stdout.

### valgrind
Compile and link the program with release flags. This can be done by:

```
make config=release utest
```

Execute the binary through valgrind with a tool named callgrind:

```
valgrind --tool=callgrind build/release/utest --gtest_filter="your-filter"
```

A filenamed callgrind.out.pid will be generated. This file can be inspected visually with kcachegrind:

```
kcachegrind callgrind.out.pid
```

## Coverage

To instrument the binary for collecting coverage data, c/c++ source files must be compiled with `-ftest-coverage -fprofile-arcs` or `--coverage`, and the objects must be linked with `-lgcov` or `--coverage`. When compilation is done, files named `*.gcno` will be generated alone with the object files. This can be done by:

```
make config=coverage utest
```

Then, the generated binary must be executed:

```
build/coverage/utest --gtest_filter="your-filter"
```

After its execution, files named `*.gcda` will be generated in the same folder as the corresponding objects. Finally, the data can be visualized by `lcov`:

```
lcov -c -d ${dir_to_search_gcda_files} -b ${base_dir_of_src_files} --no-external -o ${output_folder}
```

All of these steps can be done by:

```
./coverage.sh
```

