#include "solve.hpp"
#include "lapack.hpp"
namespace sanity::linear
{
void solveLU(MatrixMutableView A, VectorMutableView b)
{
    assert(A.nrow() == A.ncol());
    assert(A.nrow() == b.size());
    int res =
        lapack::gesv(A, MatrixMutableView(&b(0), b.size(), 1, 1), nullptr);
    assert(res == 0);
}

}  // namespace sanity::linear
