#include "ImplicitSolvers/ImplicitSolvers.hpp"

#include <cstddef>

namespace DiffusionEQ
{

namespace Laasonen
{

void PassCoefficient(
    CoefficientMatrix& A_Obj,
    double DiffNumberX_,
    double DiffNumberY_
) noexcept
{
    // ================
    // Laasonen Scheme
    // ================
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
/*
void PassCoefficient(TridiagonalMatrix& A3_Obj, double DiffNumber_) noexcept
{

     // ================
     // Lassonen Scheme
     // =========================================================================================
     // EQ : -r * U[i - 1][n + 1] + (1.0 + 2.0 * r) * U[i][n + 1] - r * U[i + 1][n + 1] = RHS(T_n)
     // RHS(T_n) = U[i][n]
     // Create : 
     // *LowerDiagonal For Lassonen Scheme
     // *MainDiagonal For Lassonen Scheme
     // *UpperDiagonal For Lassonen Scheme
     // ========================================
     std::size_t N= A3_Obj.GetMSize();
     double r = DiffNumber_;
     // Iterate through the interior nodes
     for (std::size_t i = 1; i < N - 1; ++i)
     {
         A3_Obj.L(i) = -r; //A[i][i - 1] = -r;

         A3_Obj.M(i) = 1.0 + 2.0 * r; //A[i][i] = 1.0 + 2.0 * r;

         A3_Obj.U(i) = -r; //A[i][i + 1] = -r;
     }

}
*/
namespace RightHandSide
{

void PassRHS(RHS& RHS_Obj, const Field& Field_Obj) noexcept
{
    // ==============
    // RHS For Lassonen Method
    // ===============
    // EQ :-d_x * u(j-1, i)^{n+1} - d_y * u(j, i-1)^{n+1} + (1 + 2*d_x + 2*d_y) * u(j, i)^{n+1} - d_x * u(j+1, i)^{n+1} - d_y * u(j, i+1)^{n+1} = u(j, i)^n
    // RHS = U[j][i][n]
    // Create RHS for Laasonen Method
    std::size_t p;
    std::size_t Nx = RHS_Obj.Get_Nx();
    std::size_t Ny = RHS_Obj.Get_Ny();

    // for internal nodes
    for (std::size_t j = 1; j < Ny - 1; j++)
    {
        for (std::size_t i = 1; i < Nx - 1; i++)
        {
            p = j * Nx + i;
            RHS_Obj[p] = Field_Obj[j][i];
        }
    }
}

} // namespace RightHandSide

} // namespace Laasonen

namespace CrankNicolson
{

void PassCoefficient(
    CoefficientMatrix& A_Obj,
    double DiffNumberX_,
    double DiffNumberY_
) noexcept
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
/*
void PassCoefficient(TridiagonalMatrix& A3_Obj, double DiffNumber_) noexcept
{
     // =====================================================================================
     // CrankNicolson Scheme
     // =====================================================================================
     //  EQ : -(r / 2) * U[i - 1]^(n + 1) + (1 + r) * U[i]^(n + 1) - (r / 2) * U[i + 1]^(n + 1)
     //  = (r / 2) * U[i - 1]^n + (1 - r) * U[i]^n + (r / 2) * U[i + 1]^n
     //  Create : 
     //     *LowerDiagonal For CrankNicolson Scheme
     //     *MainDiagonal For CrankNicolson Scheme
     //     *UpperDiagonal For CrankNicolson Scheme
     // =================================================
     std::size_t N = A3_Obj.GetMSize();
     double r = DiffNumber_;
     double r_half = DiffNumber_ / 2.0;

    // Iterate through the interior nodes
    for (std::size_t i = 1 ; i < N-1; ++i)
    {
        //LowerDiagonal For CrankNicolson Scheme
        A3_Obj.L(i) = -r_half; // A[i][i - 1] = -r / 2.0;
        A3_Obj.M(i) = 1.0 + r; // A[i][i] = 1.0 + r;
        A3_Obj.U(i) = -r_half; //A[i][i + 1] = -r / 2.0;
    }

}
*/

namespace RightHandSide
{

void PassRHS(
    RHS& RHS_Obj,
    const Field& Field_Obj,
    double DiffNumberX_,
    double DiffNumberY_
) noexcept

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

} // namespace RightHandSide

} // namespace CrankNicolson

} // namespace DiffusionEQ


/*


// IMPORTANT:
// LowerDiagonalValues[0] and UpperDiagonalValues[numNodes - 1] are unused
// placeholder elements. They only exist so that L[i], d[i], and u[i]
// can all be accessed using the same row index i.
//
// However, UpperDiagonalValues[0] and LowerDiagonalValues[numNodes - 1]
// are NOT unused:
//   - UpperDiagonalValues[0] is the coefficient of U[1] in the first row.
//   - LowerDiagonalValues[numNodes - 1] is the coefficient of U[numNodes - 2]
//     in the last row.
//
// For Dirichlet boundary conditions, the first and last matrix rows must
// directly enforce:
//   U[0] = leftBoundaryValue
//   U[numNodes - 1] = rightBoundaryValue
//
// Therefore, the boundary rows must be identity rows:
//   First row: d[0] = 1.0 and u[0] = 0.0
//   Last row:  L[numNodes - 1] = 0.0 and d[numNodes - 1] = 1.0
//
// The Crank-Nicolson coefficients L = -r/2, d = 1+r, and u = -r/2
// are valid only for the internal rows i = 1, ..., numNodes - 2.
// Do not assign -r/2 to u[0] or L[numNodes - 1], because doing so
// couples the prescribed boundary values to their neighboring nodes
// and prevents the matrix from directly enforcing the Dirichlet BCs.*/
// 

}
