#pragma once

#include "SimulationSetup/SimulationBoundaries/SimulationBoundaries.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
#include "Core/Field/Field.hpp"
namespace Discretization::Implicit::Boundary::ADI::SweepDirection::X
{
// Enforce boundary conditions on Tridiagonal Matrix For X Sweep Direction
void Enforce(::TridiagonalMatrix& A3X_Obj, const SimulationBoundaries& Boundaries_Obj);

// Enforce boundary conditions on RHS Vector For X Sweep Direction
void Enforce(std::size_t FixedRowIndex,::RHS& RHSX_Obj,const Field& Field_n,const SimulationBoundaries& Boundaries_Obj);
}