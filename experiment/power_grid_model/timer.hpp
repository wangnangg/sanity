#pragma once
#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>
#include "type.hpp"

class timed_scope
{
    std::chrono::high_resolution_clock::time_point _start_time;
    const char* _name;

public:
    timed_scope(const char* name);
    ~timed_scope();
};

class timer
{
    std::chrono::duration<Real> _duration =
        std::chrono::duration<Real>::zero();
    std::chrono::high_resolution_clock::time_point _start_time;
    const char* _name;

public:
    timer(const char* name) { _name = name; }
    void start() { _start_time = std::chrono::high_resolution_clock::now(); }
    void stop()
    {
        _duration += std::chrono::duration_cast<std::chrono::duration<Real>>(
            std::chrono::high_resolution_clock::now() - _start_time);
        _start_time = std::chrono::high_resolution_clock::now();
    }
    ~timer()
    {
        std::cout << _name << " used " << _duration.count() << " seconds"
                  << std::endl;
    }
};
