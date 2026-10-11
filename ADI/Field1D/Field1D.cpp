#include "ADI/Field1D/Field1D.hpp"

Field1D::Field1D(SweepDirection Direction_, Mesh& Mesh_Obj)
    : Direction(Direction_),
      Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny()),
      n((Direction == SweepDirection::X) ? Nx : Ny)
{
    field1D.assign(n, 0.0);
}
