
config?=debug
build_dir:=build/${config}
flags:= -Isrc -std=c++1z -Wall -Wfloat-conversion -Wsign-conversion -Wimplicit-fallthrough -Wno-return-type-c-linkage  -Werror -MMD
gtest_dir:= googletest/googletest
gtest_flags:=-isystem ${gtest_dir}/include -I${gtest_dir}
link_flags:=-lstdc++ -lm -pthread -lblas -llapack
ifeq ($(config), release)
  flags += -O3 -DNDEBUG
  link_flags += -O3 -DNDEBUG
else
  ifeq ($(config), profile)
    flags += -g -O3 -pg -no-pie
    link_flags += -g -O3 -no-pie
  else
    ifeq ($(config), coverage)
      flags += -g -ftest-coverage -fprofile-arcs
      link_flags += -g -fprofile-arcs
    else
      ifeq ($(config), debug)
        flags += -g
        link_flags += -g
      else
$(error Unknown config: $(config))
      endif
    endif
  endif
endif

.PHONY: all lib utest
utest: ${build_dir}/utest
all: lib utest
lib: ${build_dir}/libsanity.a
${build_dir}/src/linear/oper.o: src/linear/oper.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/oper.cpp -o ${build_dir}/src/linear/oper.o
${build_dir}/src/linear:
	mkdir -p $@
${build_dir}/src/linear/lapack.o: src/linear/lapack.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/lapack.cpp -o ${build_dir}/src/linear/lapack.o
${build_dir}/src/linear/utils.o: src/linear/utils.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/utils.cpp -o ${build_dir}/src/linear/utils.o
${build_dir}/src/linear/blas.o: src/linear/blas.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/blas.cpp -o ${build_dir}/src/linear/blas.o
${build_dir}/src/linear/decomp.o: src/linear/decomp.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/decomp.cpp -o ${build_dir}/src/linear/decomp.o
${build_dir}/test/main.o: test/main.cpp | ${build_dir}/test 
	clang ${gtest_flags} ${flags} -c test/main.cpp -o ${build_dir}/test/main.o
${build_dir}/test:
	mkdir -p $@
${build_dir}/test/linear/blas.o: test/linear/blas.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/blas.cpp -o ${build_dir}/test/linear/blas.o
${build_dir}/test/linear:
	mkdir -p $@
${build_dir}/test/linear/decomp.o: test/linear/decomp.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/decomp.cpp -o ${build_dir}/test/linear/decomp.o
${build_dir}/${gtest_dir}/src/gtest-all.o: ${gtest_dir}/src/gtest-all.cc | ${build_dir}/${gtest_dir}/src 
	clang ${gtest_flags} -c ${gtest_dir}/src/gtest-all.cc -o ${build_dir}/${gtest_dir}/src/gtest-all.o
${build_dir}/${gtest_dir}/src:
	mkdir -p $@
${build_dir}/utest: ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/decomp.o ${build_dir}/test/main.o ${build_dir}/test/linear/blas.o ${build_dir}/test/linear/decomp.o ${build_dir}/${gtest_dir}/src/gtest-all.o  | ${build_dir}
	clang ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/decomp.o ${build_dir}/test/main.o ${build_dir}/test/linear/blas.o ${build_dir}/test/linear/decomp.o ${build_dir}/${gtest_dir}/src/gtest-all.o  ${link_flags} -o ${build_dir}/utest
${build_dir}:
	mkdir -p $@
${build_dir}/libsanity.a: ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/decomp.o  | ${build_dir}
	ar crf ${build_dir}/libsanity.a ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/decomp.o 
deps:=${build_dir}/src/linear/oper.d ${build_dir}/src/linear/lapack.d ${build_dir}/src/linear/utils.d ${build_dir}/src/linear/blas.d ${build_dir}/src/linear/decomp.d ${build_dir}/test/main.d ${build_dir}/test/linear/blas.d ${build_dir}/test/linear/decomp.d ${build_dir}/${gtest_dir}/src/gtest-all.d 

.PHONY: clean
clean:
	  rm build -rf
-include ${deps}
