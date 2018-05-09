#include "srn.hpp"
#include "linear.hpp"
#include "reach.hpp"

namespace sanity::petrinet
{
using namespace linear;
StochasticRewardNet SrnCreator::create() const
{
    std::vector<Transition> trans;
    std::vector<SrnTransProp> props;
    for (const auto& t : _trans)
    {
        trans.push_back(t._trans);
        props.push_back(t._prop);
    }
    return {.pnet = PetriNet(_place_count, _g_enable, std::move(trans)),
            .transProps = std::move(props)};
}

}  // namespace sanity::petrinet
