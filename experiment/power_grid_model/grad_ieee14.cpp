#include "common.hpp"

TEST(power_grid_model, ieee14_grad)
{
    auto cdf = readIeeeCdfModel(data_base + "ieee_cdf_models/ieee14cdf.txt");

    Context context;
    context.model = ieeeCdfModel2ExpModel(cdf, 1.2);

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
        {2, 0, 1, 2},  //
        {2, 1, 1, 2},  //
        {3, 1, 1, 2},  //
    };

    uint base = trunc.size() - 1;
    Real org_reward = 2.48054;
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
        Real gain = std::abs(org_reward - new_res.reward);
        if (gain > max)
        {
            max = gain;
            max_i = (int)i;
        }
        std::cout << "gain: " << gain << std::endl;
    }
    std::cout << "max_i : " << max_i << ", max gain: " << max << std::endl;
    std::cout << "{ " << candi[(uint)max_i].bus << ", "
              << candi[(uint)max_i].load << ", " << candi[(uint)max_i].gen
              << ", " << candi[(uint)max_i].line << " }" << std::endl;
}
