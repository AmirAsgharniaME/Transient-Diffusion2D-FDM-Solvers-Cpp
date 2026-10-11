#pragma once

#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"

namespace Discretization::Implicit::Laasonen::ADI::SweepDirection::Y
{
// Assemble dense coefficient matrix for Gaussian Elimination
void AssembleMatrix(TridiagonalMatrix& A3Y_Obj, double DiffNumberY_) noexcept;

// Assemble Right-Hand Side (RHS) vector
void AssembleRHS(
    RHS& rhsY,
    const Field& Field_nPlus_Star,
    double DiffNumberX_,
    std::size_t FixedColIndex) noexcept;
}
