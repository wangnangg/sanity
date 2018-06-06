#include "common.hpp"

using namespace sanity::simulate;
class GpnObLoadServed : public GpnObserver
{
    Context* _ct;

    Real _acc_reward;
    Real _last_load;
    Real _last_time;

    std::vector<Real> _samples;

protected:
    virtual void reset(const GpnSimulator::Event& evt,
                       const GpnSimulator::State& state,
                       const GpnSimulator::EventQueue& queue)
    {
        _acc_reward = 0.0;
    }
    virtual void houseKeeping(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue)
    {
        _last_load = servedLoad(*_ct, &state.currMarking);
        _last_time = evt.time;
    }
    virtual void updateReward(const GpnSimulator::Event& evt,
                              const GpnSimulator::State& state,
                              const GpnSimulator::EventQueue& queue)
    {
        Real time;
        if (evt.time >= _end_time)
        {
            time = _end_time;
        }
        else
        {
            time = evt.time;
        }
        _acc_reward += (time - _last_time) * _last_load;
    }
    virtual void end(const GpnSimulator::Event& evt,
                     const GpnSimulator::State& state,
                     const GpnSimulator::EventQueue& queue)
    {
        _samples.push_back(_acc_reward / timeSpan());
    }

public:
    GpnObLoadServed(Context& ct, Real begin_time, Real end_time)
        : GpnObserver(begin_time, end_time), _ct(&ct)
    {
    }
    const std::vector<Real>& samples() const { return _samples; }
};

TEST(power_grid_model, ieee14_sim)
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

    std::cout << std::endl;
    timed_scope t1("total");

    auto srn = exp2srn_sim(ct, bus_fail, bus_repair, load_fail, load_repair,
                           gen_fail, gen_repair, line_fail, line_repair);
    auto gpn = srn2gpn(srn);
    auto init_mk = createInitMarking(srn, ct);

    auto simulator = gpnSimulator(gpn, init_mk, UniformSampler());

    Real start_time = 0;
    Real end_time = 10000 * 100;
    auto load_ob = GpnObLoadServed(ct, start_time, end_time);
    auto log = GpnObLog(false);
    simulator.addObserver(log);
    simulator.addObserver(load_ob);

    timer sim_timer("ieee14_sim");
    sim_timer.start();
    uint nSample = 900;
    uint report_gap = 30;
    for (uint i = 0; i < nSample; i++)
    {
        simulator.begin();
        simulator.runFor(end_time);
        simulator.end();
        if ((i + 1) % report_gap == 0)
        {
            sim_timer.whatTime();
            std::cout << "load: "
                      << confidenceInterval(load_ob.samples(), 0.95)
                      << std::endl;
        }
    }
    std::cout << "load: " << confidenceInterval(load_ob.samples(), 0.95)
              << std::endl;
}
