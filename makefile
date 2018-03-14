
config?=debug
build_dir:=build/${config}
flags:= -Isrc -std=c++1z -Wall -Wfloat-conversion -Wsign-conversion -Wimplicit-fallthrough -Werror -MMD
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
${build_dir}/src/linear/lapack.o: src/linear/lapack.cpp | ${build_dir}/src/linear 
	g++ ${flags} -c src/linear/lapack.cpp -o ${build_dir}/src/linear/lapack.o
${build_dir}/src/linear:
	mkdir -p $@
${build_dir}/src/linear/decomp.o: src/linear/decomp.cpp | ${build_dir}/src/linear 
	g++ ${flags} -c src/linear/decomp.cpp -o ${build_dir}/src/linear/decomp.o
${build_dir}/test/main.o: test/main.cpp | ${build_dir}/test 
	g++ ${gtest_flags} ${flags} -c test/main.cpp -o ${build_dir}/test/main.o
${build_dir}/test:
	mkdir -p $@
${build_dir}/test/linear/decomp.o: test/linear/decomp.cpp | ${build_dir}/test/linear 
	g++ ${gtest_flags} ${flags} -c test/linear/decomp.cpp -o ${build_dir}/test/linear/decomp.o
${build_dir}/test/linear:
	mkdir -p $@
${build_dir}/${gtest_dir}/src/gtest-all.o: ${gtest_dir}/src/gtest-all.cc | ${build_dir}/${gtest_dir}/src 
	g++ ${gtest_flags} -c ${gtest_dir}/src/gtest-all.cc -o ${build_dir}/${gtest_dir}/src/gtest-all.o
${build_dir}/${gtest_dir}/src:
	mkdir -p $@
${build_dir}/utest: ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/decomp.o ${build_dir}/test/main.o ${build_dir}/test/linear/decomp.o ${build_dir}/${gtest_dir}/src/gtest-all.o  | ${build_dir}
	g++ ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/decomp.o ${build_dir}/test/main.o ${build_dir}/test/linear/decomp.o ${build_dir}/${gtest_dir}/src/gtest-all.o  ${link_flags} -o ${build_dir}/utest
${build_dir}:
	mkdir -p $@
${build_dir}/libsanity.a: ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/decomp.o  | ${build_dir}
	ar crf ${build_dir}/libsanity.a ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/decomp.o 
deps:=${build_dir}/src/linear/lapack.d ${build_dir}/src/linear/decomp.d ${build_dir}/test/main.d ${build_dir}/test/linear/decomp.d ${build_dir}/${gtest_dir}/src/gtest-all.d 

.PHONY: clean
clean:
	  rm build -rf
-include ${deps}
