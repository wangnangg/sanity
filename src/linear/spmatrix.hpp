#pragma once
#include <vector>
#include "type.hpp"

namespace sanity::linear
{
// idx is sorted.
struct Spmatrix
{
    enum Format
    {
        RowCompressed,
        ColCompressed
    } format;
    std::vector<uint> ptr;
    std::vector<uint> idx;
    std::vector<Real> val;
    uint nrow;
    uint ncol;
    Spmatrix(uint nrow, uint ncol, Format format)
        : format(format), nrow(nrow), ncol(ncol)
    {
    }
};

// triples are always sorted (row major); duplicated items are allowed
struct SpmatrixTriple
{
    struct Triple
    {
        uint row;
        uint col;
        Real val;
    };
    std::vector<Triple> triples;
    uint nrow;
    uint ncol;
    SpmatrixTriple(uint nrow, uint ncol) : nrow(nrow), ncol(ncol) {}
};

void sortTriples(SpmatrixTriple& spmat);

Spmatrix triple2compressed(const SpmatrixTriple& trimat,
                           Spmatrix::Format target_format);

Real SpmatrixGet(const Spmatrix& spmat, size_t row, size_t col);

}  // namespace sanity::linear
