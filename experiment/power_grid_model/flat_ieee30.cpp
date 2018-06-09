#include "common.hpp"

TEST(power_grid_model, ieee30_flat)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee30cdf.txt");

    Context ct;
    ct.model = ieeeCdfModel2ExpModel(cdf, 1.2);

    std::cout << "# bus: " << ct.model.nbus;
    std::cout << ", # load: " << ct.model.nload;
    std::cout << ", # gen: " << ct.model.ngen;
    std::cout << ", # line: " << ct.model.nline << std::endl;
    for (uint f = 2; f <= 2; f++)
    {
        std::cout << std::endl;
        timed_scope t1("total");
        std::cout << "failures allowed: " << f << std::endl;
        auto srn =
            exp2srn_flat(ct, bus_fail, bus_repair, load_fail, load_repair,
                         gen_fail, gen_repair, line_fail, line_repair, f);
        auto init_mk = createInitMarking(srn, ct);

        ReducedReachGenResult rg;
        {
            timed_scope t2("generation");
            rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);
        }
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        SrnSteadyStateSol sol;
        {
            timed_scope t2("solution");
            sol = srnSteadyStateSor(rg.graph, rg.edgeRates, w, tol, max_iter);
            auto reward_load = srnProbReward(
                srn, sol, rg.nodeMarkings,
                [&ct](auto state) { return servedLoad(ct, state.marking); });

            std::cout << "load severed in average: " << reward_load
                      << std::endl;
        }
    }
}
