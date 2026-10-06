#include "ExplicitSolvers/ExplicitSolvers.hpp"

#include <cstddef>

namespace DiffusionEQ
{
namespace FTCS
{
void Solve_nPlus1(
    const Field& Field_n_Obj,
    Field& Field_nPlus1_Obj,
    double DiffNumberX,
    double DiffNumberY) noexcept
{
    const std::size_t Nx = Field_n_Obj.Get_Nx();
    const std::size_t Ny = Field_n_Obj.Get_Ny();

    // FTCS 2D explicit solve for n+1 on internal nodes
    for (std::size_t j = 1; j < Ny - 1; ++j)
    {
        for (std::size_t i = 1; i < Nx - 1; ++i)
        {
            const double center = Field_n_Obj[j][i];
            const double dxx = Field_n_Obj[j][i + 1] - 2.0 * center + Field_n_Obj[j][i - 1];
            const double dyy = Field_n_Obj[j + 1][i] - 2.0 * center + Field_n_Obj[j - 1][i];
            const double RHS = center + (DiffNumberX * dxx) + (DiffNumberY * dyy);
            Field_nPlus1_Obj[j][i] = RHS;
        }
    }
}
}




namespace DUFORT_FRANKEL
{

void Solve_nPlus1(
    const Field& Field_n_Obj,
    const Field& Field_nminus1_Obj,
    Field& Field_nplus1_Obj,
    std::array<double, 3> CoeffsVector_) noexcept
{
    const std::size_t Nx = Field_n_Obj.Get_Nx();
    const std::size_t Ny = Field_n_Obj.Get_Ny();

    // DuFort-Frankel 2D explicit solve for n+1 on internal nodes
    for (std::size_t j = 1; j < Ny - 1; ++j)
    {
        for (std::size_t i = 1; i < Nx - 1; ++i)
        {
            // CoeffsVector_[0] -> Time (n-1) center node
            // CoeffsVector_[1] -> Space Y-direction neighbors (j+1, j-1)
            // CoeffsVector_[2] -> Space X-direction neighbors (i+1, i-1)
            const double RHS = CoeffsVector_[0] * Field_nminus1_Obj[j][i]
                             + CoeffsVector_[1] * (Field_n_Obj[j + 1][i] + Field_n_Obj[j - 1][i])
                             + CoeffsVector_[2] * (Field_n_Obj[j][i + 1] + Field_n_Obj[j][i - 1]);

            Field_nplus1_Obj[j][i] = RHS;
        }
    }
}


} // namespace DUFORT_FRANKEL

}
