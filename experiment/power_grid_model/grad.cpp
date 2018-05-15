#include "common.hpp"

struct DiffTrunc
{
    uint bus;
    uint load;
    uint gen;
    uint line;
};

TEST(power_grid_model, ieee14_diff)
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

    Context context;
    context.model = ieeeCdfModel2ExpModel(cdf);

    std::cout << "# bus: " << context.model.nbus;
    std::cout << ", # load: " << context.model.nload;
    std::cout << ", # gen: " << context.model.ngen;
    std::cout << ", # line: " << context.model.nline << std::endl;
    std::vector<DiffTrunc> trunc = {
        {0, 0, 0, 0},  //
        {1, 0, 0, 0},  //
        {1, 0, 1, 0},  //
        {1, 0, 1, 1},  //
        {2, 0, 1, 1},  //
        {2, 0, 2, 1},  // 3
        {2, 0, 2, 2}   // 2
    };
    for (uint i = 0; i < trunc.size(); i++)
    {
        const auto& tr = trunc[i];
        std::cout << std::endl;
        timed_scope t1("total");
        std::cout << "failures allowed: bus  " << tr.bus << ", load "
                  << tr.load << ", gen" << tr.gen << ", line " << tr.line
                  << std::endl;
        auto srn =
            exp2srn_diff(context, bus_fail, bus_repair, load_fail,
                         load_repair, gen_fail, gen_repair, line_fail,
                         line_repair, tr.bus, tr.load, tr.gen, tr.line);
        auto init_mk = createInitMarking(srn, context);

        ReducedReachGenResult rg;
        {
            timed_scope t2("generation");
            rg = genReducedReachGraph(srn, init_mk, 1e-6, 100);
        }
        std::cout << "# markings: " << rg.nodeMarkings.size() << std::endl;

        uint max_iter = 1000;
        Real tol = 1e-6;
        Real w = 0.8;
        GeneralSrnSteadyStateSol sol;
        {
            timed_scope t2("solution");
            sol = srnSteadyStateDecomp(
                rg.graph, rg.edgeRates, rg.initProbs,
                [=](const Spmatrix& A, VectorMutableView x,
                    VectorConstView b) {
                    auto res = solveSor(A, x, b, w, tol, max_iter);
                    if (res.error > tol || std::isnan(res.error))
                    {
                        std::cout << "Sor. nIter: " << res.nIter;
                        std::cout << ", error: " << res.error << std::endl;
                        throw std::invalid_argument(
                            "Sor failed to converge.");
                    }
                });
        }
        auto reward_load =
            srnProbReward(srn, sol, rg.nodeMarkings, [&context](auto state) {
                return servedLoad(context, *state.marking);
            });

        std::cout << "load severed in average: " << reward_load << std::endl;
    }
}