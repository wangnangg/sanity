#include "timer.hpp"

timed_scope::timed_scope(const char* name)
{
    std::cout << name << " timer started." << std::endl;
    _start_time = std::chrono::high_resolution_clock::now();
    _name = name;
}

timed_scope::~timed_scope()
{
    std::chrono::duration<Real> time_span =
        std::chrono::duration_cast<std::chrono::duration<Real>>(
            std::chrono::high_resolution_clock::now() - _start_time);
    std::cout << _name << " used " << time_span.count() << " seconds"
              << std::endl;
}
