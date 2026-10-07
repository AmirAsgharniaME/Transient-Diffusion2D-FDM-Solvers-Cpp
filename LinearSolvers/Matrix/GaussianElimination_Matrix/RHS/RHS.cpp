#include "LinearSolvers/Matrix/GaussianElimination_Matrix/RHS/RHS.hpp"

RHS::RHS(const Mesh& Mesh_Obj)
    : Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny()),
      N(Nx * Ny),
      rhs(N, 0.0)
{}
