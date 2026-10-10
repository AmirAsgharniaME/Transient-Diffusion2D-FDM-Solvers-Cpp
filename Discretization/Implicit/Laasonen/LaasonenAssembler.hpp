#pragma once

#include "LinearSolvers/Matrix/GaussianElimination_Matrix/CoefficientMatrix/CoefficientMatrix.hpp"
#include "LinearSolvers/Matrix/GaussianElimination_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"

namespace Discretization::Implicit::Laasonen
{
// Assemble dense coefficient matrix for Gaussian Elimination
void AssembleMatrix(CoefficientMatrix& A_Obj,double DiffNumberX_,double DiffNumberY_) noexcept;

// Assemble Right-Hand Side (RHS) vector
void AssembleRHS(RHS& rhs, const Field& field_n) noexcept;


}
