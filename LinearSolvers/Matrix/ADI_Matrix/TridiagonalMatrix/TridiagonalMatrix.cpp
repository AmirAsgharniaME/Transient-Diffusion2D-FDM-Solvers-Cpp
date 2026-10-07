#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"

TridiagonalMatrix::TridiagonalMatrix(
    SweepDirection SweepDirection_,
    const Mesh& Mesh_Obj
)
    : Direction(SweepDirection_),
      Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny())
{
    n = (Direction == SweepDirection::X) ? Nx : Ny;
    LowerDiagonal.assign(n, 0.0);
    MainDiagonal.assign(n, 0.0);
    UpperDiagonal.assign(n, 0.0);
}
