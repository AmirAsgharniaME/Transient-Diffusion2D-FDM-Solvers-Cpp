#pragma once

#include <iostream>
#include <iomanip>
#include <cstddef>
#include <string_view>
#include <ios>

namespace StatusPrinter 
{
    inline void Print(
        std::string_view Convergence_Criteria,
        std::size_t TimeLevel,
        double Current_Tolerance,
        double dt_,
        bool Converged = false) noexcept
    {
        // Save current stream state to restore it before leaving
        const std::ios::fmtflags original_flags = std::cout.flags();
        const std::streamsize original_precision = std::cout.precision();

        const double Time = static_cast<double>(TimeLevel) * dt_;

        // Print progress line
        std::cout << "TimeLevel: " << std::setw(6) << TimeLevel
                  << " | Time: " << std::fixed << std::setprecision(2)
                  << std::setw(10) << Time << " s"
                  << " | " << Convergence_Criteria << ": " << std::scientific
                  << std::setprecision(2) << Current_Tolerance << '\n';

        if (Converged)
        {
            // Print Professional Summary Block
            std::cout << "========================================\n"
                      << "       Steady State Solution Achieved   \n"
                      << "----------------------------------------\n"
                      << std::left << std::setw(18) << "Final TimeLevel" << ": " << TimeLevel << '\n'
                      << std::left << std::setw(18) << "Time Step (dt)"  << ": " << std::defaultfloat << dt_ << '\n'
                      << std::left << std::setw(18) << "Total Time"      << ": " << std::fixed << std::setprecision(3) << Time << " s" << '\n'
                      << std::left << std::setw(18) << "Final Tolerance" << ": " << std::scientific << std::setprecision(4) << Current_Tolerance << '\n'
                      << "========================================" << std::endl;
        }

        // Restore original stream state
        std::cout.flags(original_flags);
        std::cout.precision(original_precision);
    }
}