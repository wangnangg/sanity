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
    timed_scope(const char* name)
    {
        std::cout << name << " timer started." << std::endl;
        _start_time = std::chrono::high_resolution_clock::now();
        _name = name;
    }
    ~timed_scope()
    {
        std::chrono::duration<Real> time_span =
            std::chrono::duration_cast<std::chrono::duration<Real>>(
                std::chrono::high_resolution_clock::now() - _start_time);
        std::cout << _name << " used " << time_span.count() << " seconds"
                  << std::endl;
    }
};

class timer
{
    std::chrono::high_resolution_clock::time_point _start_time;
    const char* _name;

public:
    timer(const char* name)
    {
        _name = name;
        _start_time = std::chrono::high_resolution_clock::now();
    }
    void start() { _start_time = std::chrono::high_resolution_clock::now(); }
    void whatTime()
    {
        auto duration =
            std::chrono::duration_cast<std::chrono::duration<Real>>(
                std::chrono::high_resolution_clock::now() - _start_time);
        std::cout << _name << " used " << duration.count() << " seconds"
                  << std::endl;
    }
};
