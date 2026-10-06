#pragma once

#include <array>
#include "Core/Field/Field.hpp"


namespace DiffusionEQ
{

namespace  FTCS
{
   void Solve_nPlus1(
    const Field& Field_n_Obj,
    Field& Field_nPlus1_Obj, 
    double DiffNumberX,
    double DiffNumberY
)noexcept;
}

namespace  DUFORT_FRANKEL
{
   
   void Solve_nPlus1(
    const Field& Field_n_Obj,
    const Field& Field_nminus1_Obj,
    Field& Field_nplus1_Obj,
    std::array<double, 3>  CoeffsVector_
) noexcept;



//  * Design Choice: Returns std::array<double, 2> instead of std::vector.
//  * - Stack Allocation: Avoids dynamic heap memory management overhead.
//  * - Performance: Guarantees zero runtime allocations, which is critical for CFD hot-path calculations.
//  * - Cache Locality: Contiguous stack storage improves CPU cache utilization and enables better optimization by the compiler.
// Calculates discrete scheme coefficients based on diffusion number (r)
// Computes coefficients for 2D DuFort-Frankel: {c0, cy, cx}
[[nodiscard]] constexpr std::array<double, 3> ReturnCoeffs(
    double DiffNumberX,
    double DiffNumberY) noexcept
{
    const double denominator = 1.0 + 2.0 * DiffNumberX + 2.0 * DiffNumberY;
    const double c0 = (1.0 - 2.0 * DiffNumberX - 2.0 * DiffNumberY) / denominator;
    const double cy = (2.0 * DiffNumberY) / denominator;
    const double cx = (2.0 * DiffNumberX) / denominator;

    return {c0, cy, cx};
}

}


}

