#include "Discretization/Explicit/DuFortFrankel/DuFortFrankel.hpp"

#include <cstddef>

namespace Discretization::Explicit::DuFortFrankel
{

void Step(
    const Field& Field_nMinus1,
    const Field& Field_n,
    Field& Field_nPlus1,
    std::array<double, 3> Coeffs
) noexcept
{
    const std::size_t Nx = Field_n.Get_Nx();
    const std::size_t Ny = Field_n.Get_Ny();

    for (std::size_t j = 1; j < Ny - 1; ++j)
    {
        for (std::size_t i = 1; i < Nx - 1; ++i)
        {
            const double value = Coeffs[0] * Field_nMinus1[j][i]
                               + Coeffs[1] * (Field_n[j + 1][i] + Field_n[j - 1][i])
                               + Coeffs[2] * (Field_n[j][i + 1] + Field_n[j][i - 1]);
            Field_nPlus1[j][i] = value;
        }
    }
}

} // namespace Discretization::Explicit::DuFortFrankel
