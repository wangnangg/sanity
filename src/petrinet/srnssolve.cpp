#include "srn.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
using namespace linear;

IterationResult steadyStateSOR(const Spmatrix& Q, VectorMutableView prob,
                               Real w, Real tol, uint max_iter)
{
    assert(Q.format == Spmatrix::RowCompressed);
    uint iter;
    Real error;
    auto prob_prev = Vector(prob.size());
    for (iter = 1; iter <= max_iter; iter++)
    {
        copy(constView(prob), mutableView(prob_prev));
        for (uint i = 0; i < Q.nrow; i++)
        {
            auto idx_begin = Q.idx.begin() + Q.ptr[i];
            auto idx_end = Q.idx.begin() + Q.ptr[i + 1];
            auto val_begin = Q.val.begin() + Q.ptr[i];
            Real residual = 0;
            Real a_ii = 0;
            while (idx_begin != idx_end)
            {
                if (*idx_begin == i)  // diag
                {
                    a_ii = *val_begin;
                }
                else
                {  // non diag
                    residual -= *val_begin * prob((int)*idx_begin);
                }
                idx_begin++;
                val_begin++;
            }
            prob((int)i) = w * residual / a_ii + (1 - w) * prob((int)i);
        }
        Real n1 = norm1(prob);
        scale(1.0 / n1, prob);
        error = maxDiff(prob, prob_prev);
        if (error < tol)
        {
            break;
        }
    }
    return {.nIter = iter, .error = error};
}

}  // namespace sanity::petrinet
