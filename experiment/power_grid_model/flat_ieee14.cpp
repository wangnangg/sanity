#include "common.hpp"

TEST(power_grid_model, three_nodes)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    Context ct;

    auto b1 = ct.model.addBus(ExpBusType::PV, 0, 1.0);
    auto b2 = ct.model.addBus(ExpBusType::PV, 10 / 100.0, 63 / 100.0);
    auto b3 = ct.model.addBus(ExpBusType::PQ, 90 / 100.0, 0);
    ct.model.addLine(b1, b2, 0.0576, 250.0 / 100.0);
    ct.model.addLine(b2, b3, 0.092, 250.0 / 100.0);
    ct.model.addLine(b1, b3, 0.17, 150.0 / 100.0);

    for (uint f = 0; f <= 10; f++)
    {
        auto srn =
            exp2srn_flat(ct, bus_fail, bus_repair, load_fail, load_repair,
                         gen_fail, gen_repair, line_fail, line_repair, f);
        auto init_mk = createInitMarking(srn, ct);

        auto rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        auto sol = srnSteadyStateDecomp(
            rg.graph, rg.edgeRates, rg.initProbs,
            [=](const Spmatrix& A, VectorMutableView x, VectorConstView b) {
                auto res = solveSor(A, x, b, w, tol, max_iter);
                if (res.error > tol || std::isnan(res.error))
                {
                    std::cout << "Sor. nIter: " << res.nIter;
                    std::cout << ", error: " << res.error << std::endl;
                    throw std::invalid_argument("Sor failed to converge.");
                }
            });
        // printSrnSol2(rg.nodeMarkings, sol.matrix2node, sol.nTangibles,
        //             sol.solution);
        Real prob = 0.0;
        for (uint i = sol.nTransient; i < rg.nodeMarkings.size(); i++)
        {
            prob += sol.solution(i);
        }
        ASSERT_NEAR(prob, 1.0, tol);
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        auto reward_load = srnProbReward(
            srn, sol, rg.nodeMarkings,
            [&ct](auto state) { return servedLoad(ct, state.marking); });

        std::cout << "load severed in average: " << reward_load << std::endl;
    }
}

TEST(power_grid_model, ieee14_flat)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");

    Context ct;
    ct.model = ieeeCdfModel2ExpModel(cdf, 1.2);

    std::cout << "# bus: " << ct.model.nbus;
    std::cout << ", # load: " << ct.model.nload;
    std::cout << ", # gen: " << ct.model.ngen;
    std::cout << ", # line: " << ct.model.nline << std::endl;
    for (uint f = 0; f <= 3; f++)
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
        IrreducibleSrnSteadyStateSol sol;
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
