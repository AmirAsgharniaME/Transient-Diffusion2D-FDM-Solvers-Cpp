#pragma once

#include <filesystem>
#include "PathType.hpp"
#include "SolverScheme.hpp"


namespace Path
{
    // Generates a resolved relative directory path based on category and numerical scheme
    [[nodiscard]] inline std::filesystem::path Create(OutputCategory category, SolverScheme scheme = SolverScheme::FTCS)
    {
        namespace fs = std::filesystem;

        // Map category to its respective root directory
        switch (category)
        {
            case OutputCategory::Initial:
                return fs::path("Results/Initial_Field");
                
            case OutputCategory::Numerical:
                return fs::path("Results/Numerical_Field") / To_String(scheme);

            case OutputCategory::Analytical:
                return "Results/Analytical_Field";

            default:
                return "Results/Unknown";
        }
    }
}
