#pragma once

#include <cstdint>

// Enumeration for identifying target directory categories
enum class OutputCategory : std::uint8_t
{
    Numerical,  // Target for numerical simulation datasets
    Analytical, // Target for exact analytical solutions or verification data
    Initial    // Target for initial conditions
};
