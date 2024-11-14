#pragma once

#include <iostream>
#include <chrono>
#include "Containers/String.h"

#define PROFILE_SCOPE(name) \
    struct EndScope_##name { \
        std::chrono::time_point<std::chrono::high_resolution_clock> start; \
        EndScope_##name() : start(std::chrono::high_resolution_clock::now()) {} \
        ~EndScope_##name() { \
        auto end = std::chrono::high_resolution_clock::now(); \
        std::chrono::duration<double, std::milli> duration = end - start; \
        std::cout << "Execution time of " << #name << ": " << duration.count() << " ms" << std::endl; \
    } \
} endScope_##name


const LString NAME_None = "NAME_None";