#pragma once

#include "SimulationSetup/SimulationBoundaries/SimulationBoundaries.hpp"
#include "LinearSolvers/Matrix/GaussianElimination_Matrix/CoefficientMatrix/CoefficientMatrix.hpp"
#include "LinearSolvers/Matrix/GaussianElimination_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"

namespace Discretization::Implicit::Boundary
{
// Enforce boundary conditions on full coefficient matrix
void Enforce(::CoefficientMatrix& A_Obj, const SimulationBoundaries& Boundaries_Obj);

// Enforce boundary conditions on right-hand side vector
void Enforce(::RHS& RHS_Obj, const Field& Field_Obj, const SimulationBoundaries& Boundaries_Obj);
}
