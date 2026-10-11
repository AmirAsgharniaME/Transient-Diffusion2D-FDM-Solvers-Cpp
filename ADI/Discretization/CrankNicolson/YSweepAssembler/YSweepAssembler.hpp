#pragma once

#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"

namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::Y
{
// Assemble the tridiagonal matrix for the second ADI half-step.
void AssembleMatrix(TridiagonalMatrix& A3Y_Obj, double DiffNumberY_) noexcept;

// Assemble the right-hand side using the intermediate field from the X sweep.
void AssembleRHS(
    RHS& rhsY,
    const Field& Field_nPlus_Star,
    double DiffNumberX_,
    std::size_t FixedColIndex) noexcept;
}
