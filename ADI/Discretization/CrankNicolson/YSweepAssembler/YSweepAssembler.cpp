#include "ADI/Discretization/CrankNicolson/YSweepAssembler/YSweepAssembler.hpp"

#include <cstddef>

// ============================================================================
// Crank-Nicolson ADI: second half-step, implicit in Y and explicit in X
// ============================================================================
//
// Index convention:
//   - i is the physical x-index and the matrix column index.
//   - j is the physical y-index and the matrix row index.
//   - Field[j][i] means row j (y-direction), column i (x-direction).
//
// For a fixed column i, the intermediate field T^* is advanced to T^(n+1):
//
//   (T[j][i]^(n+1) - T[j][i]^*) / (dt / 2)
//       = alpha * [
//             delta_xx T[j][i]^*
//           + delta_yy T[j][i]^(n+1)
//         ]
//
// Define the full-step diffusion numbers:
//
//   DiffNumberX = alpha * dt / dx^2
//   DiffNumberY = alpha * dt / dy^2
//
// The tridiagonal system along j (the y-direction) is:
//
//   -DiffNumberY/2 * T[j - 1][i]^(n+1)
//   + (1 + DiffNumberY) * T[j][i]^(n+1)
//   -DiffNumberY/2 * T[j + 1][i]^(n+1)
//       = RHS
//
// where:
//
//   RHS = DiffNumberX/2 * T[j][i - 1]^*
//       + (1 - DiffNumberX) * T[j][i]^*
//       + DiffNumberX/2 * T[j][i + 1]^*
//
// Boundary values are not assembled here; they must be handled by the caller
// or by the boundary-condition treatment in the linear-system solver.
// ============================================================================

namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::Y
{

// Assemble the tridiagonal matrix for one Y-sweep.
//
// For a uniform grid and constant material properties, the same matrix is used
// for every fixed column i.
void AssembleMatrix(TridiagonalMatrix& A3Y_Obj, double DiffNumberY_) noexcept
{
    const std::size_t Ny = A3Y_Obj.GetMSize();
    const double rY = DiffNumberY_ / 2.0;

    // Interior rows are j = 1, ..., Ny - 2.
    for (std::size_t j = 1; j + 1 < Ny; ++j)
    {
        A3Y_Obj.L(j) = -rY;
        A3Y_Obj.M(j) = 1.0 + 2.0 * rY;
        A3Y_Obj.U(j) = -rY;
    }
}


// Assemble the right-hand side for one fixed matrix column i.
//
// The X-direction is explicit in this half-step, so its values come from the
// intermediate field T^*.
void AssembleRHS(
    RHS& rhsY,
    const Field& Field_nPlus_Star,
    double DiffNumberX_,
    std::size_t FixedColIndex) noexcept
{
    const std::size_t Ny = rhsY.GetSize();
    const double rX = DiffNumberX_ / 2.0;

    // Interior rows are j = 1, ..., Ny - 2.
    for (std::size_t j = 1; j + 1 < Ny; ++j)
    {
        rhsY[j] =
            rX
                * Field_nPlus_Star[j][FixedColIndex - 1]
            + (1.0 - DiffNumberX_)
                * Field_nPlus_Star[j][FixedColIndex]
            + rX
                * Field_nPlus_Star[j][FixedColIndex + 1];
    }
}

} // namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::Y
