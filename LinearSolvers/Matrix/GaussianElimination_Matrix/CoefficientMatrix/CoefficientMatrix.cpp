#include "LinearSolvers/Matrix/GaussianElimination_Matrix/CoefficientMatrix/CoefficientMatrix.hpp"

CoefficientMatrix::CoefficientMatrix(const Mesh& Mesh_Obj)
    : Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny()),
      N(Nx * Ny),
      A(N * N, 0.0)
{}
