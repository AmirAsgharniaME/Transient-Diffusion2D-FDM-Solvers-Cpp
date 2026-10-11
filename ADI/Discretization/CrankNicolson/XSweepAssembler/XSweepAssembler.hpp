#pragma once

#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"

namespace Discretization::Implicit::CrankNicolson::ADI::SweepDirection::X
{

void AssembleMatrix(TridiagonalMatrix& A3X_Obj, double DiffNumberX_) noexcept;

// Assemble the right-hand side for the first ADI half-step.
void AssembleRHS(
    RHS& rhsX,
    const Field& Field_n,
    double DiffNumberY_,
    std::size_t FixedRowIndex) noexcept;
}
