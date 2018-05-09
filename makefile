
config?=debug
build_dir:=build/${config}
flags:= -Isrc -std=c++1z -Wall -Wno-error=unused-variable -Wfloat-conversion -Wsign-conversion -Wimplicit-fallthrough -Wno-return-type-c-linkage  -Werror -MMD
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
${build_dir}/src/graph/decompose.o: src/graph/decompose.cpp | ${build_dir}/src/graph 
	clang ${flags} -c src/graph/decompose.cpp -o ${build_dir}/src/graph/decompose.o
${build_dir}/src/graph:
	mkdir -p $@
${build_dir}/src/graph/search.o: src/graph/search.cpp | ${build_dir}/src/graph 
	clang ${flags} -c src/graph/search.cpp -o ${build_dir}/src/graph/search.o
${build_dir}/src/root/newton.o: src/root/newton.cpp | ${build_dir}/src/root 
	clang ${flags} -c src/root/newton.cpp -o ${build_dir}/src/root/newton.o
${build_dir}/src/root:
	mkdir -p $@
${build_dir}/src/linear/oper.o: src/linear/oper.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/oper.cpp -o ${build_dir}/src/linear/oper.o
${build_dir}/src/linear:
	mkdir -p $@
${build_dir}/src/linear/matrix_view.o: src/linear/matrix_view.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/matrix_view.cpp -o ${build_dir}/src/linear/matrix_view.o
${build_dir}/src/linear/lapack.o: src/linear/lapack.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/lapack.cpp -o ${build_dir}/src/linear/lapack.o
${build_dir}/src/linear/utils.o: src/linear/utils.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/utils.cpp -o ${build_dir}/src/linear/utils.o
${build_dir}/src/linear/permute.o: src/linear/permute.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/permute.cpp -o ${build_dir}/src/linear/permute.o
${build_dir}/src/linear/vector_view.o: src/linear/vector_view.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/vector_view.cpp -o ${build_dir}/src/linear/vector_view.o
${build_dir}/src/linear/blas.o: src/linear/blas.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/blas.cpp -o ${build_dir}/src/linear/blas.o
${build_dir}/src/linear/solve.o: src/linear/solve.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/solve.cpp -o ${build_dir}/src/linear/solve.o
${build_dir}/src/linear/decomp.o: src/linear/decomp.cpp | ${build_dir}/src/linear 
	clang ${flags} -c src/linear/decomp.cpp -o ${build_dir}/src/linear/decomp.o
${build_dir}/src/powerflow/matpower_format.o: src/powerflow/matpower_format.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/matpower_format.cpp -o ${build_dir}/src/powerflow/matpower_format.o
${build_dir}/src/powerflow:
	mkdir -p $@
${build_dir}/src/powerflow/dcsolve.o: src/powerflow/dcsolve.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/dcsolve.cpp -o ${build_dir}/src/powerflow/dcsolve.o
${build_dir}/src/powerflow/model.o: src/powerflow/model.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/model.cpp -o ${build_dir}/src/powerflow/model.o
${build_dir}/src/powerflow/dcmodel.o: src/powerflow/dcmodel.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/dcmodel.cpp -o ${build_dir}/src/powerflow/dcmodel.o
${build_dir}/src/powerflow/ieee_cdf.o: src/powerflow/ieee_cdf.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/ieee_cdf.cpp -o ${build_dir}/src/powerflow/ieee_cdf.o
${build_dir}/src/powerflow/solve.o: src/powerflow/solve.cpp | ${build_dir}/src/powerflow 
	clang ${flags} -c src/powerflow/solve.cpp -o ${build_dir}/src/powerflow/solve.o
${build_dir}/src/petrinet/reach.o: src/petrinet/reach.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/reach.cpp -o ${build_dir}/src/petrinet/reach.o
${build_dir}/src/petrinet:
	mkdir -p $@
${build_dir}/src/petrinet/srn.o: src/petrinet/srn.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/srn.cpp -o ${build_dir}/src/petrinet/srn.o
${build_dir}/src/petrinet/srnreach.o: src/petrinet/srnreach.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/srnreach.cpp -o ${build_dir}/src/petrinet/srnreach.o
${build_dir}/src/petrinet/mkutils.o: src/petrinet/mkutils.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/mkutils.cpp -o ${build_dir}/src/petrinet/mkutils.o
${build_dir}/src/petrinet/petrinet.o: src/petrinet/petrinet.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/petrinet.cpp -o ${build_dir}/src/petrinet/petrinet.o
${build_dir}/src/petrinet/srnssolve.o: src/petrinet/srnssolve.cpp | ${build_dir}/src/petrinet 
	clang ${flags} -c src/petrinet/srnssolve.cpp -o ${build_dir}/src/petrinet/srnssolve.o
${build_dir}/src/splinear/oper.o: src/splinear/oper.cpp | ${build_dir}/src/splinear 
	clang ${flags} -c src/splinear/oper.cpp -o ${build_dir}/src/splinear/oper.o
${build_dir}/src/splinear:
	mkdir -p $@
${build_dir}/src/splinear/eigen.o: src/splinear/eigen.cpp | ${build_dir}/src/splinear 
	clang ${flags} -c src/splinear/eigen.cpp -o ${build_dir}/src/splinear/eigen.o
${build_dir}/src/splinear/utils.o: src/splinear/utils.cpp | ${build_dir}/src/splinear 
	clang ${flags} -c src/splinear/utils.cpp -o ${build_dir}/src/splinear/utils.o
${build_dir}/src/splinear/solve.o: src/splinear/solve.cpp | ${build_dir}/src/splinear 
	clang ${flags} -c src/splinear/solve.cpp -o ${build_dir}/src/splinear/solve.o
${build_dir}/src/splinear/matrix.o: src/splinear/matrix.cpp | ${build_dir}/src/splinear 
	clang ${flags} -c src/splinear/matrix.cpp -o ${build_dir}/src/splinear/matrix.o
${build_dir}/test/main.o: test/main.cpp | ${build_dir}/test 
	clang ${gtest_flags} ${flags} -c test/main.cpp -o ${build_dir}/test/main.o
${build_dir}/test:
	mkdir -p $@
${build_dir}/test/root/newton.o: test/root/newton.cpp | ${build_dir}/test/root 
	clang ${gtest_flags} ${flags} -c test/root/newton.cpp -o ${build_dir}/test/root/newton.o
${build_dir}/test/root:
	mkdir -p $@
${build_dir}/test/linear/matrix_view.o: test/linear/matrix_view.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/matrix_view.cpp -o ${build_dir}/test/linear/matrix_view.o
${build_dir}/test/linear:
	mkdir -p $@
${build_dir}/test/linear/blas.o: test/linear/blas.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/blas.cpp -o ${build_dir}/test/linear/blas.o
${build_dir}/test/linear/solve.o: test/linear/solve.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/solve.cpp -o ${build_dir}/test/linear/solve.o
${build_dir}/test/linear/decomp.o: test/linear/decomp.cpp | ${build_dir}/test/linear 
	clang ${gtest_flags} ${flags} -c test/linear/decomp.cpp -o ${build_dir}/test/linear/decomp.o
${build_dir}/test/powerflow/dcsolve.o: test/powerflow/dcsolve.cpp | ${build_dir}/test/powerflow 
	clang ${gtest_flags} ${flags} -c test/powerflow/dcsolve.cpp -o ${build_dir}/test/powerflow/dcsolve.o
${build_dir}/test/powerflow:
	mkdir -p $@
${build_dir}/test/powerflow/model.o: test/powerflow/model.cpp | ${build_dir}/test/powerflow 
	clang ${gtest_flags} ${flags} -c test/powerflow/model.cpp -o ${build_dir}/test/powerflow/model.o
${build_dir}/test/powerflow/solve.o: test/powerflow/solve.cpp | ${build_dir}/test/powerflow 
	clang ${gtest_flags} ${flags} -c test/powerflow/solve.cpp -o ${build_dir}/test/powerflow/solve.o
${build_dir}/test/petrinet/srn.o: test/petrinet/srn.cpp | ${build_dir}/test/petrinet 
	clang ${gtest_flags} ${flags} -c test/petrinet/srn.cpp -o ${build_dir}/test/petrinet/srn.o
${build_dir}/test/petrinet:
	mkdir -p $@
${build_dir}/test/petrinet/srnsolve.o: test/petrinet/srnsolve.cpp | ${build_dir}/test/petrinet 
	clang ${gtest_flags} ${flags} -c test/petrinet/srnsolve.cpp -o ${build_dir}/test/petrinet/srnsolve.o
${build_dir}/${gtest_dir}/src/gtest-all.o: ${gtest_dir}/src/gtest-all.cc | ${build_dir}/${gtest_dir}/src 
	clang ${gtest_flags} -c ${gtest_dir}/src/gtest-all.cc -o ${build_dir}/${gtest_dir}/src/gtest-all.o
${build_dir}/${gtest_dir}/src:
	mkdir -p $@
${build_dir}/utest: ${build_dir}/src/graph/decompose.o ${build_dir}/src/graph/search.o ${build_dir}/src/root/newton.o ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/matrix_view.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/permute.o ${build_dir}/src/linear/vector_view.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/solve.o ${build_dir}/src/linear/decomp.o ${build_dir}/src/powerflow/matpower_format.o ${build_dir}/src/powerflow/dcsolve.o ${build_dir}/src/powerflow/model.o ${build_dir}/src/powerflow/dcmodel.o ${build_dir}/src/powerflow/ieee_cdf.o ${build_dir}/src/powerflow/solve.o ${build_dir}/src/petrinet/reach.o ${build_dir}/src/petrinet/srn.o ${build_dir}/src/petrinet/srnreach.o ${build_dir}/src/petrinet/mkutils.o ${build_dir}/src/petrinet/petrinet.o ${build_dir}/src/petrinet/srnssolve.o ${build_dir}/src/splinear/oper.o ${build_dir}/src/splinear/eigen.o ${build_dir}/src/splinear/utils.o ${build_dir}/src/splinear/solve.o ${build_dir}/src/splinear/matrix.o ${build_dir}/test/main.o ${build_dir}/test/root/newton.o ${build_dir}/test/linear/matrix_view.o ${build_dir}/test/linear/blas.o ${build_dir}/test/linear/solve.o ${build_dir}/test/linear/decomp.o ${build_dir}/test/powerflow/dcsolve.o ${build_dir}/test/powerflow/model.o ${build_dir}/test/powerflow/solve.o ${build_dir}/test/petrinet/srn.o ${build_dir}/test/petrinet/srnsolve.o ${build_dir}/${gtest_dir}/src/gtest-all.o  | ${build_dir}
	clang ${build_dir}/src/graph/decompose.o ${build_dir}/src/graph/search.o ${build_dir}/src/root/newton.o ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/matrix_view.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/permute.o ${build_dir}/src/linear/vector_view.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/solve.o ${build_dir}/src/linear/decomp.o ${build_dir}/src/powerflow/matpower_format.o ${build_dir}/src/powerflow/dcsolve.o ${build_dir}/src/powerflow/model.o ${build_dir}/src/powerflow/dcmodel.o ${build_dir}/src/powerflow/ieee_cdf.o ${build_dir}/src/powerflow/solve.o ${build_dir}/src/petrinet/reach.o ${build_dir}/src/petrinet/srn.o ${build_dir}/src/petrinet/srnreach.o ${build_dir}/src/petrinet/mkutils.o ${build_dir}/src/petrinet/petrinet.o ${build_dir}/src/petrinet/srnssolve.o ${build_dir}/src/splinear/oper.o ${build_dir}/src/splinear/eigen.o ${build_dir}/src/splinear/utils.o ${build_dir}/src/splinear/solve.o ${build_dir}/src/splinear/matrix.o ${build_dir}/test/main.o ${build_dir}/test/root/newton.o ${build_dir}/test/linear/matrix_view.o ${build_dir}/test/linear/blas.o ${build_dir}/test/linear/solve.o ${build_dir}/test/linear/decomp.o ${build_dir}/test/powerflow/dcsolve.o ${build_dir}/test/powerflow/model.o ${build_dir}/test/powerflow/solve.o ${build_dir}/test/petrinet/srn.o ${build_dir}/test/petrinet/srnsolve.o ${build_dir}/${gtest_dir}/src/gtest-all.o  ${link_flags} -o ${build_dir}/utest
${build_dir}:
	mkdir -p $@
${build_dir}/libsanity.a: ${build_dir}/src/graph/decompose.o ${build_dir}/src/graph/search.o ${build_dir}/src/root/newton.o ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/matrix_view.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/permute.o ${build_dir}/src/linear/vector_view.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/solve.o ${build_dir}/src/linear/decomp.o ${build_dir}/src/powerflow/matpower_format.o ${build_dir}/src/powerflow/dcsolve.o ${build_dir}/src/powerflow/model.o ${build_dir}/src/powerflow/dcmodel.o ${build_dir}/src/powerflow/ieee_cdf.o ${build_dir}/src/powerflow/solve.o ${build_dir}/src/petrinet/reach.o ${build_dir}/src/petrinet/srn.o ${build_dir}/src/petrinet/srnreach.o ${build_dir}/src/petrinet/mkutils.o ${build_dir}/src/petrinet/petrinet.o ${build_dir}/src/petrinet/srnssolve.o ${build_dir}/src/splinear/oper.o ${build_dir}/src/splinear/eigen.o ${build_dir}/src/splinear/utils.o ${build_dir}/src/splinear/solve.o ${build_dir}/src/splinear/matrix.o  | ${build_dir}
	ar crf ${build_dir}/libsanity.a ${build_dir}/src/graph/decompose.o ${build_dir}/src/graph/search.o ${build_dir}/src/root/newton.o ${build_dir}/src/linear/oper.o ${build_dir}/src/linear/matrix_view.o ${build_dir}/src/linear/lapack.o ${build_dir}/src/linear/utils.o ${build_dir}/src/linear/permute.o ${build_dir}/src/linear/vector_view.o ${build_dir}/src/linear/blas.o ${build_dir}/src/linear/solve.o ${build_dir}/src/linear/decomp.o ${build_dir}/src/powerflow/matpower_format.o ${build_dir}/src/powerflow/dcsolve.o ${build_dir}/src/powerflow/model.o ${build_dir}/src/powerflow/dcmodel.o ${build_dir}/src/powerflow/ieee_cdf.o ${build_dir}/src/powerflow/solve.o ${build_dir}/src/petrinet/reach.o ${build_dir}/src/petrinet/srn.o ${build_dir}/src/petrinet/srnreach.o ${build_dir}/src/petrinet/mkutils.o ${build_dir}/src/petrinet/petrinet.o ${build_dir}/src/petrinet/srnssolve.o ${build_dir}/src/splinear/oper.o ${build_dir}/src/splinear/eigen.o ${build_dir}/src/splinear/utils.o ${build_dir}/src/splinear/solve.o ${build_dir}/src/splinear/matrix.o 
deps:=${build_dir}/src/graph/decompose.d ${build_dir}/src/graph/search.d ${build_dir}/src/root/newton.d ${build_dir}/src/linear/oper.d ${build_dir}/src/linear/matrix_view.d ${build_dir}/src/linear/lapack.d ${build_dir}/src/linear/utils.d ${build_dir}/src/linear/permute.d ${build_dir}/src/linear/vector_view.d ${build_dir}/src/linear/blas.d ${build_dir}/src/linear/solve.d ${build_dir}/src/linear/decomp.d ${build_dir}/src/powerflow/matpower_format.d ${build_dir}/src/powerflow/dcsolve.d ${build_dir}/src/powerflow/model.d ${build_dir}/src/powerflow/dcmodel.d ${build_dir}/src/powerflow/ieee_cdf.d ${build_dir}/src/powerflow/solve.d ${build_dir}/src/petrinet/reach.d ${build_dir}/src/petrinet/srn.d ${build_dir}/src/petrinet/srnreach.d ${build_dir}/src/petrinet/mkutils.d ${build_dir}/src/petrinet/petrinet.d ${build_dir}/src/petrinet/srnssolve.d ${build_dir}/src/splinear/oper.d ${build_dir}/src/splinear/eigen.d ${build_dir}/src/splinear/utils.d ${build_dir}/src/splinear/solve.d ${build_dir}/src/splinear/matrix.d ${build_dir}/test/main.d ${build_dir}/test/root/newton.d ${build_dir}/test/linear/matrix_view.d ${build_dir}/test/linear/blas.d ${build_dir}/test/linear/solve.d ${build_dir}/test/linear/decomp.d ${build_dir}/test/powerflow/dcsolve.d ${build_dir}/test/powerflow/model.d ${build_dir}/test/powerflow/solve.d ${build_dir}/test/petrinet/srn.d ${build_dir}/test/petrinet/srnsolve.d ${build_dir}/${gtest_dir}/src/gtest-all.d 

.PHONY: clean cleandeps
clean:
	rm -rf build
cleandeps:
	find build -name "*.d" | xargs rm -rf
-include ${deps}
