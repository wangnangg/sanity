#include "utils.hpp"
#include <iomanip>
#include <iostream>

namespace sanity::splinear
{
std::ostream& operator<<(std::ostream& os, const Spmatrix& spmat)
{
    os << "sparse [" << std::endl;
    for (uint i = 0; i < spmat.nrow; i++)
    {
        for (uint j = 0; j < spmat.ncol; j++)
        {
            os << std::defaultfloat << std::setw(10) << std::setprecision(4)
               << std::setfill(' ') << SpmatrixGet(spmat, i, j);
        }
        os << std::endl;
    }
    os << "]" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const SpmatrixTriple& spmat)
{
    os << "sptriple " << spmat.nrow << " by " << spmat.ncol << std::endl;
    for (uint i = 0; i < spmat.triples.size(); i++)
    {
        std::cout << spmat.triples[i].row << " " << spmat.triples[i].col
                  << " " << spmat.triples[i].val << std::endl;
    }
    return os;
}
}  // namespace sanity::splinear
