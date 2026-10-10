#include "Discretization/Implicit/Laasonen/LaasonenAssembler.hpp"

#include <cstddef>

namespace Discretization::Implicit::Laasonen
{
// Assemble dense coefficient matrix for Gaussian Elimination
void AssembleMatrix(CoefficientMatrix& A_Obj,double DiffNumberX_,double DiffNumberY_) noexcept
{
    // ===================================
    // Laasonen Scheme
    // ===================================
    // EQ : (1 + 2*dx + 2*dy) * u_(n+1)[j][i]
    //     - dx * u_(n+1)[j][i - 1]
    //     - dx * u_(n+1)[j][i + 1]
    //     - dy * u_(n+1)[j - 1][i]
    //     - dy * u_(n+1)[j + 1][i]
    //     = u_(n)[j][i]
    // 1D numbering : p = j * Nx + i
    // RHS = U[j][i][n]
    // Create A for Laasonen Scheme
    // ========================================
    std::size_t Nx = A_Obj.Get_Nx();
    std::size_t Ny = A_Obj.Get_Ny();
    const double dx = DiffNumberX_;
    const double dy = DiffNumberY_;
    // Iterate through the interior nodes
    for (std::size_t j = 1; j < Ny - 1; ++j)
    {
        for (std::size_t i = 1; i < Nx - 1; ++i)
        {
            const std::size_t p = j * Nx + i;
            A_Obj[p][p - 1] = -dx;                               // West Neighbor (i - 1)
            A_Obj[p][p] = 1.0 + 2.0 * dx + 2.0 * dy;            // Center (i, j)
            A_Obj[p][p + 1] = -dx;                              // East Neighbor (i + 1)
            A_Obj[p][p - Nx] = -dy;                            // South / Bottom Neighbor (j - 1)
            A_Obj[p][p + Nx] = -dy;                           // North / Top Neighbor (j + 1)
        }
    }
}

void AssembleRHS(RHS& rhs, const Field& field_n) noexcept
{
    // ==============
    // RHS For Lassonen Method
    // ===============
    // EQ :-d_x * u(j-1, i)^{n+1} - d_y * u(j, i-1)^{n+1} + (1 + 2*d_x + 2*d_y) * u(j, i)^{n+1} - d_x * u(j+1, i)^{n+1} - d_y * u(j, i+1)^{n+1} = u(j, i)^n
    // RHS = U[j][i][n]
    // Create RHS for Laasonen Method
    std::size_t p;
    std::size_t Nx = rhs.Get_Nx();
    std::size_t Ny = rhs.Get_Ny();

    // for internal nodes
    for (std::size_t j = 1; j < Ny - 1; j++)
    {
        for (std::size_t i = 1; i < Nx - 1; i++)
        {
            p = j * Nx + i;
            rhs[p] = field_n[j][i];
        }
    }
}
}
