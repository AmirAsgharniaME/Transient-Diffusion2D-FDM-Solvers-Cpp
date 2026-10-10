#pragma once

#include <array>
#include "Core/Field/Field.hpp"

namespace Discretization::Explicit::DuFortFrankel
{
    [[nodiscard]] inline std::array<double, 3> CalculateCoefficients(
        double DiffNumberX,
        double DiffNumberY
    ) noexcept
    {
        const double denominator = 1.0 + 2.0 * DiffNumberX + 2.0 * DiffNumberY;
        const double c0 = (1.0 - 2.0 * DiffNumberX - 2.0 * DiffNumberY) / denominator;
        const double cy = (2.0 * DiffNumberY) / denominator;
        const double cx = (2.0 * DiffNumberX) / denominator;
        return {c0, cy, cx};
    }

    // Updates Field from time levels (n-1, n) to n+1
    void Step(
        const Field& Field_nMinus1,
        const Field& Field_n,
        Field& Field_nPlus1,
        std::array<double, 3> Coeffs
    ) noexcept;
}
