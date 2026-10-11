#include "ADI/Discretization/Boundary/XSweepBoundary/XEnforceBoundary.hpp"

#include <cstddef>

namespace Discretization::Implicit::Boundary::ADI::SweepDirection::X
{
    // Enforce boundary conditions on Tridiagonal Matrix For X Sweep Direction
void Enforce(::TridiagonalMatrix& A3X_Obj, const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
    {
        std::size_t Nx=A3X_Obj.GetMSize();

        //Apply Dirichlet boundary condition To MainDiagonal Values
        A3X_Obj.M(0) = 1.0;
        A3X_Obj.M(Nx-1) = 1.0;

        //Apply Dirichlet boundary condition To UpperDiagonal Values
        A3X_Obj.U(0) = 0.0;    //for Diriclet BC
        A3X_Obj.U(Nx-1) = 0.0; // unused: just for Even index

        //Apply Dirichlet boundary condition To LowerDiagonal Values
        A3X_Obj.L(0) = 0.0; // unused: just for Even index
        A3X_Obj.L(Nx-1) = 0.0; //for Diriclet BC
    }
    else if(Boundaries_Obj.GetType() == BoundaryType::Neumann || Boundaries_Obj.GetType() == BoundaryType::Robin)
    {
        throw std::invalid_argument(
            "Boundary Conditions: Neumann and Robin Boundary Conditions are not Implemented yet.");
    }
}

// Enforce boundary conditions on RHS Vector For X Sweep Direction
void Enforce(std::size_t FixedRowIndex,::RHS& RHSX_Obj,const Field& Field_n,const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
    {
        std::size_t Nx=RHSX_Obj.GetSize();
        RHSX_Obj[0] = Field_n[FixedRowIndex][0];
        RHSX_Obj[Nx-1] = Field_n[FixedRowIndex][Nx-1];
    }

}

}