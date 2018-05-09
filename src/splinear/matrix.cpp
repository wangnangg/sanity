#include "matrix.hpp"
#include <algorithm>

namespace sanity::splinear
{
static bool compTripleRow(const SpmatrixTriple::Triple& e1,
                          const SpmatrixTriple::Triple& e2)
{
    if (e1.row < e2.row)
    {
        return true;
    }
    else if (e1.row > e2.row)
    {
        return false;
    }
    else if (e1.col < e2.col)
    {
        return true;
    }
    else
    {
        return false;
    }
}

static bool compTripleCol(const SpmatrixTriple::Triple& e1,
                          const SpmatrixTriple::Triple& e2)
{
    if (e1.col < e2.col)
    {
        return true;
    }
    else if (e1.col > e2.col)
    {
        return false;
    }
    else if (e1.row < e2.row)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void sortTriples(SpmatrixTriple& spmat, bool rowFirst)
{
    if (rowFirst)
    {
        std::sort(spmat.triples.begin(), spmat.triples.end(), compTripleRow);
    }
    else
    {
        std::sort(spmat.triples.begin(), spmat.triples.end(), compTripleCol);
    }
}

Spmatrix triple2compressed(SpmatrixTriple& trimat,
                           Spmatrix::Format target_format)
{
    sortTriples(trimat, target_format == Spmatrix::RowCompressed);
    Spmatrix csmat(trimat.nrow, trimat.ncol, target_format);
    if (target_format == Spmatrix::RowCompressed)
    {
        uint m = trimat.nrow;
        csmat.ptr.reserve(trimat.nrow + 1);
        csmat.ptr.push_back(0);
        csmat.idx.reserve(trimat.triples.size());
        csmat.val.reserve(trimat.triples.size());
        auto itor = trimat.triples.begin();
        for (uint i = 0; i < m; i++)
        {
            uint counter = 0;
            while (itor != trimat.triples.end() && itor->row == i)
            {
                uint idx = itor->col;
                Real val = itor->val;
                itor += 1;
                while (itor != trimat.triples.end() && itor->row == i &&
                       itor->col == idx)
                {
                    val += itor->val;
                    itor += 1;
                }
                if (val != 0.0)
                {
                    csmat.idx.push_back(idx);
                    csmat.val.push_back(val);
                    counter += 1;
                }
            }
            csmat.ptr.push_back(counter + csmat.ptr[i]);
        }
    }
    else
    {
        uint n = trimat.ncol;
        csmat.ptr.reserve(n + 1);
        csmat.ptr.push_back(0);
        csmat.idx.reserve(trimat.triples.size());
        csmat.val.reserve(trimat.triples.size());
        auto itor = trimat.triples.begin();
        for (uint i = 0; i < n; i++)
        {
            uint counter = 0;
            while (itor != trimat.triples.end() && itor->col == i)
            {
                uint idx = itor->row;
                Real val = itor->val;
                itor += 1;
                while (itor != trimat.triples.end() && itor->col == i &&
                       itor->row == idx)
                {
                    val += itor->val;
                    itor += 1;
                }
                if (val != 0.0)
                {
                    csmat.idx.push_back(idx);
                    csmat.val.push_back(val);
                    counter += 1;
                }
            }
            csmat.ptr.push_back(counter + csmat.ptr[i]);
        }
    }
    return csmat;
}

Real SpmatrixGet(const Spmatrix& spmat, size_t row, size_t col)
{
    if (spmat.format == Spmatrix::RowCompressed)
    {
        const uint* begin = &*(spmat.idx.begin() + spmat.ptr[row]);
        const uint* end = &*(spmat.idx.begin() + spmat.ptr[row + 1]);
        const uint* first = std::lower_bound(begin, end, col);
        if (first != end && *first == col)
        {
            return spmat.val[(uint)(first - &spmat.idx[0])];
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        const uint* begin = &*(spmat.idx.begin() + spmat.ptr[col]);
        const uint* end = &*(spmat.idx.begin() + spmat.ptr[col + 1]);
        const uint* first = std::lower_bound(begin, end, row);
        if (first != end && *first == row)
        {
            return spmat.val[(uint)(first - &spmat.idx[0])];
        }
        else
        {
            return 0.0;
        }
    }
}

}  // namespace sanity::splinear
