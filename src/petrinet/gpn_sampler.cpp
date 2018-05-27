#include "gpn_sampler.hpp"

namespace sanity::petrinet
{
// x =  1 - e^(-l * t)
// t = -1/l * log(1-x)
Real GpnExpSampler::operator()(PetriNetState pstate,
                               std::function<Real()>& uniform_sampler) const
{
    Real unif_val = uniform_sampler();
    Real mean_time = 1.0 / _rate(pstate);
    return -mean_time * std::log(unif_val);
}
}  // namespace sanity::petrinet
