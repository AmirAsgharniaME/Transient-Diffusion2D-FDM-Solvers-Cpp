#include "Discretization/Explicit/FTCS/FTCS.hpp"

#include <cstddef>

namespace Discretization::Explicit::FTCS
{
void Step(
    const Field& Field_n,
    Field& Field_nPlus1,
    double DiffNumberX,
    double DiffNumberY
) noexcept
{
    const std::size_t Nx = Field_n.Get_Nx();
    const std::size_t Ny = Field_n.Get_Ny();

    // Update internal nodes using the 2D FTCS discretization.
    for (std::size_t j = 1; j < Ny - 1; ++j)
    {
        for (std::size_t i = 1; i < Nx - 1; ++i)
        {
            const double center = Field_n[j][i];
            const double dxx = Field_n[j][i + 1] - 2.0 * center + Field_n[j][i - 1];
            const double dyy = Field_n[j + 1][i] - 2.0 * center + Field_n[j - 1][i];
            const double value = center + DiffNumberX * dxx + DiffNumberY * dyy;
            Field_nPlus1[j][i] = value;
        }
    }
}

} // namespace Discretization::Explicit::FTCS
