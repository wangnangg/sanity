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


