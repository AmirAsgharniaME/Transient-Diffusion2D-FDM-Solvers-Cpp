

// C++ Libraries
#include <iostream>

//Local Files
#include "ADI/Discretization/Boundary/XSweepBoundary/XEnforceBoundary.hpp"
#include "ADI/Discretization/Boundary/YSweepBoundary/YEnforceBoundary.hpp"
#include "ADI/Discretization/CrankNicolson/XSweepAssembler/XSweepAssembler.hpp"
#include "ADI/Discretization/CrankNicolson/YSweepAssembler/YSweepAssembler.hpp"
#include "Discretization/SolverScheme/SolverScheme.hpp"
#include "SimulationSetup/SimulationContext/SimulationContext.hpp"
#include "Utilities/KeyboardHandler/KeyboardHandler.hpp"
// #include "Discretization/Implicit/Laasonen/LaasonenAssembler.hpp"
// #include "LinearSolvers/Matrix/GaussianElimination_Matrix/CoefficientMatrix/CoefficientMatrix.hpp"
// #include "Discretization/Implicit/Boundary/EnforceBoundary.hpp"
// #include "LinearSolvers/GaussianElimination/GaussianElimination.hpp"
#include "SimulationSetup/ADI_Method/ADI.hpp"
#include "LinearSolvers/ThomasAlgorithm/ThomasAlgorithm.hpp"
int main() 
{
//-------------------------------------------------------------------------------------------------
//Simulation Setup
SimulationContext ctx(SolverScheme::CrankNicolson);
ADI adi(ctx);
//-----------------------------------------------------------------------------------------------
//Setup
ctx.StartUp(); //U_n = U_0

//-------------------------------------------------------------------------------------------------
//Create Coefficient Matrix For Gaussian Elimination

// CoefficientMatrix A(ctx.grid);
// Discretization::Implicit::Laasonen::AssembleMatrix(
//     A,
//     ctx.params.DiffNumberX.GetValue(),
//     ctx.params.DiffNumberY.GetValue());
// Discretization::Implicit::Boundary::Enforce(
//     A,
//     ctx.boundaries);

//-------------------------------------------------------------------------------------------------


ctx.WriteInitialOutputs();

std::size_t TotalTimeLevel = ctx.params.NumTimeLevels.GetValue();
std::size_t Nx = ctx.grid.Get_Nx();
std::size_t Ny = ctx.grid.Get_Ny();
for(std::size_t TimeLevel_n = 1; TimeLevel_n <= TotalTimeLevel; TimeLevel_n++)
{

//This Condition Stops The Loop Solver Iterations by Pressing ESC
if (isEscPressed())
{
    std::cout << "\nESC pressed. Exiting program now..." << '\n';
    return 0;
}
//Gaussian Elimination Setup(This Solver is Too Expensive, use ADI instead)
// CoefficientMatrix A_Copy = A;
// RHS RHS1D(ctx.grid);
// Discretization::Implicit::Laasonen::AssembleRHS(RHS1D, ctx.T_n);
// Discretization::Implicit::Boundary::Enforce(RHS1D, ctx.T_n,ctx.boundaries);
// GaussianElimination::Solve_nPlus1(A_Copy,RHS1D,ctx.T_nPlus1);

// =====================
//ADI Method
// ====================


//SweepDirection::X

adi.A3.Reset(SweepDirection::X);
adi.rhs.Reset(SweepDirection::X);
//for Uniform Grid(DeltaX = DeltaY) the Tridiagonal Matrices are the same For each fixed Row
Discretization::Implicit::CrankNicolson::ADI::SweepDirection::X::AssembleMatrix(
adi.A3, 
ctx.params.DiffNumberX.GetValue());
Discretization::Implicit::Boundary::ADI::SweepDirection::X::Enforce(
    adi.A3,
    ctx.boundaries);

for (std::size_t J = 1; J < Ny- 1; J++)
{
TridiagonalMatrix A3_Copy = adi.A3;
Discretization::Implicit::CrankNicolson::ADI::SweepDirection::X::AssembleRHS(
    adi.rhs,
     ctx.T_n, 
     ctx.params.DiffNumberY.GetValue(),
     J);
Discretization::Implicit::Boundary::ADI::SweepDirection::X::Enforce(
    J,
    adi.rhs,
    ctx.T_n,
    ctx.boundaries);

ThomasAlgorithm::Solve(A3_Copy, adi.rhs, adi.T_nPlus1_Star_Row);

adi.T_nPlus1_Star.SetRow(J, adi.T_nPlus1_Star_Row);
}



//SweepDirection::Y
adi.A3.Reset(SweepDirection::Y);
adi.rhs.Reset(SweepDirection::Y);
//for Uniform Grid(DeltaX = DeltaY) the Tridiagonal Matrices are the same For each fixed Column
Discretization::Implicit::CrankNicolson::ADI::SweepDirection::Y::AssembleMatrix(
adi.A3, 
ctx.params.DiffNumberY.GetValue());
Discretization::Implicit::Boundary::ADI::SweepDirection::Y::Enforce(
    adi.A3,
    ctx.boundaries);


for (std::size_t I = 1; I < Nx - 1; ++I)
{

TridiagonalMatrix A3_Copy = adi.A3;
Discretization::Implicit::CrankNicolson::ADI::SweepDirection::Y::AssembleRHS(
    adi.rhs,
    adi.T_nPlus1_Star,
    ctx.params.DiffNumberX.GetValue(),
    I);
Discretization::Implicit::Boundary::ADI::SweepDirection::Y::Enforce(
    I,
    adi.rhs,
    ctx.T_n,
    ctx.boundaries);

ThomasAlgorithm::Solve(A3_Copy, adi.rhs, adi.T_nPlus1_Col);
ctx.T_nPlus1.SetCol(I, adi.T_nPlus1_Col);
}


ctx.Common(TimeLevel_n);

if(ctx.GetBreakCondition())
{
    break;
}

}//End of Solver Loop


ctx.plot.LiveWindow.Close();

std::cout << "Done" << '\n';
std::cin.get();
return 0;
}//End of main
