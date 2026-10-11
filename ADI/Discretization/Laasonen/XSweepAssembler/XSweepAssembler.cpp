#include "ADI/Discretization/Laasonen/XSweepAssembler/XSweepAssembler.hpp"

#include <cstddef>

// ============================================================================
// Laasonen ADI: first half-step, implicit in X and explicit in Y
// ============================================================================
//
// Index convention:
//   - The physical x-index is i.
//   - The physical y-index is j.
//   - Field[j][i] means row j (y-direction), column i (x-direction).
//
// For a fixed row j, the first half-step advances the solution from T^n to T^*:
//
//   (T[j][i]^* - T[j][i]^n) / (dt / 2)
//       = alpha * [
//             (T[j][i - 1]^* - 2*T[j][i]^* + T[j][i + 1]^*) / dx^2
//           + (T[j - 1][i]^n - 2*T[j][i]^n + T[j + 1][i]^n) / dy^2
//         ]
//
// Define the half-step diffusion numbers:
//
//   rX = alpha * (dt / 2) / dx^2
//   rY = alpha * (dt / 2) / dy^2
//
// Rearranged equation:
//
//   -rX * T[j][i - 1]^*
//   + (1 + 2*rX) * T[j][i]^*
//   -rX * T[j][i + 1]^*
//       = rY * T[j - 1][i]^n
//       + (1 - 2*rY) * T[j][i]^n
//       + rY * T[j + 1][i]^n
//
// For each fixed j, this forms a tridiagonal system along i (the x-direction).
//
// Interior indices, assuming rows are y and columns are x:
//
//   i = 1, ..., Nx - 2
//   j = 1, ..., Ny - 2
//
// Boundary values are not assembled here; they must be handled by the caller
// or by the boundary-condition treatment in the linear-system solver.
// ============================================================================

namespace Discretization::Implicit::Laasonen::ADI::SweepDirection::X
{

// Assemble the tridiagonal matrix for one X-sweep.
//
// The matrix is the same for every fixed row j on a uniform grid with constant
// alpha, dt, dx, and dy. Only the interior x-columns are assembled here.
void AssembleMatrix(TridiagonalMatrix& A3X_Obj, double DiffNumberX_) noexcept
{
    const std::size_t Nx = A3X_Obj.GetMSize();

    // The supplied diffusion number is defined using the full time step:
    //
    //   diffusionNumberX = alpha * dt / dx^2
    //
    // This sweep covers dt/2, so its diffusion number is halved.
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
// The matrix row index is the y-index j; the column index is the x-index i.
// The Y-direction is explicit in this first half-step, so its values come from
// Field_n at time level n.
void AssembleRHS(
    RHS& rhsX,
    const Field& Field_n,
    double DiffNumberY_,
    std::size_t FixedRowIndex) noexcept
{
    const std::size_t Nx = rhsX.GetSize();

    // The supplied diffusion number is defined using the full time step:
    //
    //   diffusionNumberY = alpha * dt / dy^2
    //
    // This sweep covers dt/2, so its diffusion number is halved.
    const double rY = DiffNumberY_ / 2.0;

    // For a fixed row j, assemble the RHS for interior x-columns.
    for (std::size_t i = 1; i + 1 < Nx; ++i)
    {
        rhsX[i] =
            rY * Field_n[FixedRowIndex - 1][i]
            + (1.0 - 2.0 * rY) * Field_n[FixedRowIndex][i]
            + rY * Field_n[FixedRowIndex + 1][i];
    }
}

} // namespace Discretization::Implicit::Laasonen::ADI::SweepDirection::X
