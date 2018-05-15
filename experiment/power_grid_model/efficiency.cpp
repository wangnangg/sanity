#include "common.hpp"
struct DiffTrunc
{
    uint bus;
    uint load;
    uint gen;
    uint line;
};

struct DiffRes
{
    Real reward;
    uint nMarkings;
};
DiffRes solveDiff(Context& context, DiffTrunc tr)
{
    Real bus_fail = 0.0001;
    Real bus_repair = 0.01;

    Real load_fail = 0.001;
    Real load_repair = 1;

    Real gen_fail = 0.001;
    Real gen_repair = 0.1;

    Real line_fail = 0.005;
    Real line_repair = 0.5;

    std::cout << std::endl;
    timed_scope t1("total");
    std::cout << "failures allowed: bus  " << tr.bus << ", load " << tr.load
              << ", gen" << tr.gen << ", line " << tr.line << std::endl;

    auto srn = exp2srn_diff(context, bus_fail, bus_repair, load_fail,
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
            [=](const Spmatrix& A, VectorMutableView x, VectorConstView b) {
                auto res = solveSor(A, x, b, w, tol, max_iter);
                if (res.error > tol || std::isnan(res.error))
                {
                    std::cout << "Sor. nIter: " << res.nIter;
                    std::cout << ", error: " << res.error << std::endl;
                    throw std::invalid_argument("Sor failed to converge.");
                }
            });
    }
    auto reward_load =
        srnProbReward(srn, sol, rg.nodeMarkings, [&context](auto state) {
            return servedLoad(context, *state.marking);
        });

    std::cout << "load severed in average: " << reward_load << std::endl;

    return {.reward = reward_load, .nMarkings = (uint)rg.nodeMarkings.size()};
}

TEST(power_grid_model, ieee14_effi)
{
    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");

    Context context;
    context.model = ieeeCdfModel2ExpModel(cdf);

    std::cout << "# bus: " << context.model.nbus;
    std::cout << ", # load: " << context.model.nload;
    std::cout << ", # gen: " << context.model.ngen;
    std::cout << ", # line: " << context.model.nline << std::endl;
    std::vector<DiffTrunc> trunc = {
        {0, 0, 0, 0},  //
        {0, 0, 1, 0},  //
        {1, 0, 1, 0},  //
        {1, 0, 2, 0},  //
        {2, 0, 2, 0},  //
        {2, 0, 2, 1},  //
        {2, 0, 2, 2},  //
        {3, 0, 2, 2}   //
    };
    trunc = {
        {0, 0, 0, 0},  //
        {0, 0, 1, 0},  //
        {1, 0, 1, 0},  //
        {1, 0, 2, 0},  //
        {2, 0, 2, 0},  //
        {2, 1, 2, 1},  //
    };
    uint base = trunc.size() - 1;
    auto org_res = solveDiff(context, trunc[base]);
    std::vector<DiffTrunc> candi;
    if (trunc[base].bus < context.model.nbus)
    {
        auto tr = trunc[base];
        tr.bus += 1;
        candi.push_back(tr);
    }
    if (trunc[base].load < context.model.nload)
    {
        auto tr = trunc[base];
        tr.load += 1;
        candi.push_back(tr);
    }
    if (trunc[base].gen < context.model.ngen)
    {
        auto tr = trunc[base];
        tr.gen += 1;
        candi.push_back(tr);
    }
    if (trunc[base].line < context.model.nline)
    {
        auto tr = trunc[base];
        tr.line += 1;
        candi.push_back(tr);
    }
    Real max = 0;
    int max_i = -1;
    for (uint i = 0; i < candi.size(); i++)
    {
        auto new_res = solveDiff(context, candi[i]);
        Real gain = (org_res.reward - new_res.reward) /
                    ((Real)new_res.nMarkings - (Real)org_res.nMarkings);
        if (gain > max)
        {
            max = gain;
            max_i = (int)i;
        }
        std::cout << "gain: " << gain << std::endl;
    }
    std::cout << "max_i : " << max_i << std::endl;
    std::cout << "{ " << candi[(uint)max_i].bus << ", "
              << candi[(uint)max_i].load << ", " << candi[(uint)max_i].gen
              << ", " << candi[(uint)max_i].line << " }" << std::endl;
}
