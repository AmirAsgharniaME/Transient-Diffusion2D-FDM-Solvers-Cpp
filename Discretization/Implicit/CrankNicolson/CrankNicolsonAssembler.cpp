#include "Discretization/Implicit/CrankNicolson/CrankNicolsonAssembler.hpp"

#include <cstddef>

namespace Discretization::Implicit::CrankNicolson
{
// Assemble dense coefficient matrix for Gaussian Elimination
void AssembleMatrix(CoefficientMatrix& A_Obj, double DiffNumberX_,double DiffNumberY_) noexcept
{
    // ===============================================
    // 2D Diffusion Equation - Crank-Nicolson Scheme
    // ==============================================
    //   Full equation:
    //     -0.5*dx*u[j][i-1][n+1] - 0.5*dx*u[j][i+1][n+1] + (1+dx+dy)*u[j][i][n+1]
    //    -0.5*dy*u[j-1][i][n+1] - 0.5*dy*u[j+1][i][n+1]
    //    =
    //     0.5*dx*u[j][i-1][n] + 0.5*dx*u[j][i+1][n] + (1-dx-dy)*u[j][i][n]
    //     +0.5*dy*u[j-1][i][n] + 0.5*dy*u[j+1][i][n]
    // Create A for Crank-Nicolson Scheme
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
            A_Obj[p][p - 1] = -0.5 * dx;       // West Neighbor (i - 1)
            A_Obj[p][p] = 1.0 + dx + dy;        // Center (i, j)
            A_Obj[p][p + 1] = -0.5 * dx;        // East Neighbor (i + 1)
            A_Obj[p][p - Nx] = -0.5 * dy;       // South / Bottom Neighbor (j - 1)
            A_Obj[p][p + Nx] = -0.5 * dy;       // North / Top Neighbor (j + 1)
        }
    }
}

// // Assemble compact tridiagonal matrix for Thomas Algorithm (TDMA)
// void AssembleMatrix(TridiagonalMatrix& A, double diffNumber) noexcept;

// Assemble Right-Hand Side (RHS) vector
void AssembleRHS( RHS& RHS_Obj,
                  const Field& Field_Obj,
                  double DiffNumberX_,
                  double DiffNumberY_) noexcept
{
    // ===================
    // CrankNicolson Method
    // ====================
    // LHS (Unknowns at time step n+1):
    // -0.5*d_x * u(j, i-1)^{n+1} - 0.5*d_y * u(j-1, i)^{n+1} + (1 + d_x + d_y) * u(j, i)^{n+1} - 0.5*d_x * u(j, i+1)^{n+1} - 0.5*d_y * u(j+1, i)^{n+1}
    // RHS (Knowns at time step n):
    // 0.5*d_x * u(j, i-1)^n + 0.5*d_y * u(j-1, i)^n + (1 - d_x - d_y) * u(j, i)^n + 0.5*d_x * u(j, i+1)^n + 0.5*d_y * u(j+1, i)^n
    //=============================================================================================================================
    // Create RHS for CrankNicolson Method
    std::size_t p;
    std::size_t Nx = RHS_Obj.Get_Nx();
    std::size_t Ny = RHS_Obj.Get_Ny();
    const double d_x = DiffNumberX_;
    const double d_y = DiffNumberY_;

    // for internal nodes
    for (std::size_t j = 1; j < Ny - 1; j++)
    {
        for (std::size_t i = 1; i < Nx - 1; i++)
        {
            p = j * Nx + i;

            // for internal nodes
            double value =
                0.5 * d_x * Field_Obj[j][i - 1] +
                0.5 * d_y * Field_Obj[j - 1][i] +
                (1 - d_x - d_y) * Field_Obj[j][i] +
                0.5 * d_x * Field_Obj[j][i + 1] +
                0.5 * d_y * Field_Obj[j + 1][i];
            RHS_Obj[p] = value;
        }
    }
}
}
