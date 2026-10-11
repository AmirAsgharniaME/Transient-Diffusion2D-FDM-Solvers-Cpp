#include "ADI/Discretization/Boundary/YSweepBoundary/YEnforceBoundary.hpp"

#include <cstddef>

namespace Discretization::Implicit::Boundary::ADI::SweepDirection::Y
{
    // Enforce boundary conditions on Tridiagonal Matrix For X Sweep Direction
void Enforce(::TridiagonalMatrix& A3Y_Obj, const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
    {
        std::size_t Ny=A3Y_Obj.GetMSize();

        //Apply Dirichlet boundary condition To MainDiagonal Values
        A3Y_Obj.M(0) = 1.0;
        A3Y_Obj.M(Ny-1) = 1.0;

        //Apply Dirichlet boundary condition To UpperDiagonal Values
        A3Y_Obj.U(0) = 0.0;    //for Diriclet BC
        A3Y_Obj.U(Ny-1) = 0.0; // unused: just for Even index

        //Apply Dirichlet boundary condition To LowerDiagonal Values
        A3Y_Obj.L(0) = 0.0; // unused: just for Even index
        A3Y_Obj.L(Ny-1) = 0.0; //for Diriclet BC
    }
    else if(Boundaries_Obj.GetType() == BoundaryType::Neumann || Boundaries_Obj.GetType() == BoundaryType::Robin)
    {
        throw std::invalid_argument(
            "Boundary Conditions: Neumann and Robin Boundary Conditions are not Implemented yet.");
    }
}

// Enforce boundary conditions on RHS Vector For X Sweep Direction
void Enforce(std::size_t FixedColIndex,::RHS& RHSY_Obj,const Field& Field_n,const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
    {
        std::size_t Ny=RHSY_Obj.GetSize();
        RHSY_Obj[0] = Field_n[0][FixedColIndex];
        RHSY_Obj[Ny-1] = Field_n[Ny-1][FixedColIndex];
    }

}

}