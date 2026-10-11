#include "ADI/Discretization/CrankNicolson/XSweepAssembler/XSweepAssembler.hpp"

#include <cstddef>

// ============================================================================
// Crank-Nicolson ADI: first half-step, implicit in X and explicit in Y
// ============================================================================
//
// Index convention:
//   - The physical x-index is i.
//   - The physical y-index is j.
//   - Field[j][i] means row j (y-direction), column i (x-direction).
//
// Peaceman-Rachford ADI is a factorization of the full-step Crank-Nicolson
// scheme. The time-centered contributions are recovered by combining both
// sweeps; an additional average of the X derivative is not used in this sweep.
// For a fixed row j, the first sweep computes the intermediate field T^*:
//
//   (T[j][i]^* - T[j][i]^n) / (dt / 2)
//       = alpha * [
//             delta_xx T[j][i]^*
//           + delta_yy T[j][i]^n
//         ]
//
// Centered spatial derivatives on a uniform grid:
//
//   delta_xx T[j][i] = (T[j][i - 1] - 2*T[j][i] + T[j][i + 1]) / dx^2
//   delta_yy T[j][i] = (T[j - 1][i] - 2*T[j][i] + T[j + 1][i]) / dy^2
//
// Define the full-step diffusion numbers:
//
//   DiffNumberX = alpha * dt / dx^2
//   DiffNumberY = alpha * dt / dy^2
//
// The tridiagonal system along i (the x-direction) is:
//
//   -DiffNumberX/2 * T[j][i - 1]^*
//   + (1 + DiffNumberX) * T[j][i]^*
//   -DiffNumberX/2 * T[j][i + 1]^*
//       = RHS
//
// where:
//
//   RHS = DiffNumberY/2 * T[j - 1][i]^n
//       + (1 - DiffNumberY) * T[j][i]^n
//       + DiffNumberY/2 * T[j + 1][i]^n
//
// The x-direction is implicit at the intermediate level, while the y-direction
// is explicit at time level n.
//
// Interior indices:
//
//   i = 1, ..., Nx - 2
//   j = 1, ..., Ny - 2
//
// Boundary values are not assembled here; they must be handled by the caller
// or by the boundary-condition treatment in the linear-system solver.
// ============================================================================

namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::X
{

// Assemble the tridiagonal matrix for one X-sweep.
//
// On a uniform grid with constant alpha, dt, and dx, the same matrix is used
// for every fixed row j.
void AssembleMatrix(
    TridiagonalMatrix& A3X_Obj, double DiffNumberX_) noexcept
{
    const std::size_t Nx = A3X_Obj.GetMSize();
    const double rX = DiffNumberX_ / 2.0;

    // Interior columns are i = 1, ..., Nx - 2.
    for (std::size_t i = 1; i + 1 < Nx; ++i)
    {
        A3X_Obj.L(i) = -rX;
        A3X_Obj.M(i) = 1.0 + 2.0 * rX;
        A3X_Obj.U(i) = -rX;
    }
}


// Assemble the right-hand side for one fixed matrix row j.
//
// The right-hand side contains the old field and the explicit Y contribution.
// The unused X diffusion-number parameter preserves the existing interface;
// its contribution is assembled only in the implicit matrix.
void AssembleRHS(
    RHS& rhsX,
    const Field& Field_n,
    double DiffNumberY_,
    std::size_t FixedRowIndex) noexcept
{
    const std::size_t Nx = rhsX.GetSize();
    const double rY = DiffNumberY_ / 2.0;

    // Interior columns are i = 1, ..., Nx - 2.
    for (std::size_t i = 1; i + 1 < Nx; ++i)
    {
        rhsX[i] =
            rY * Field_n[FixedRowIndex - 1][i]
            + (1.0 - 2.0 * rY) * Field_n[FixedRowIndex][i]
            + rY * Field_n[FixedRowIndex + 1][i];
    }
}

} // namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::X
