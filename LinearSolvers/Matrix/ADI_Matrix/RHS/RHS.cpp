#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"

RHS::RHS(SweepDirection SweepDirection_,const Mesh& Mesh_Obj)
    : Direction(SweepDirection_),
      Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny())
{
    std::size_t n = (Direction == SweepDirection::X) ? Nx : Ny;
    rhs.assign(n, 0.0);
}
