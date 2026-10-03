#include "Core/Boundary/Boundary.hpp"

Boundary::Boundary(
    const Mesh& Mesh_Obj,
    const std::vector<double>& BoundaryValues_,
    BoundaryType Type_,
    BoundaryOrientation Orientation_,
    BoundaryLocation Location_
) noexcept
    : Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny()),
      Type(Type_),
      Orientation(Orientation_),
      Location(Location_)
{
    if(Orientation == BoundaryOrientation::Horizontal)
    {
        BoundaryValues.resize(Nx);
        for (std::size_t i = 0; i < Nx; ++i)
        {
            BoundaryValues[i] = BoundaryValues_[i];
        }
    }
    else if (Orientation == BoundaryOrientation::Vertical)
    {
        BoundaryValues.resize(Ny);
        for (std::size_t j = 0; j < Ny; ++j)
        {
            BoundaryValues[j] = BoundaryValues_[j];
        }

    }
}

    void Boundary::SetValue(std::vector<double>& BoundaryValues_)
    {
        if(Orientation == BoundaryOrientation::Horizontal)
        {
            if(BoundaryValues_.size() != Nx)
            {
                throw std::invalid_argument("BoundaryValues_.size() must be equal to Nx.");
            }
            for (std::size_t i = 0; i < Nx; ++i)
            {
                BoundaryValues[i] = BoundaryValues_[i];
            }
        }
        else if (Orientation == BoundaryOrientation::Vertical)
        {
            if(BoundaryValues_.size() != Ny)
            {
                throw std::invalid_argument("BoundaryValues_.size() must be equal to Ny.");
            }
            for (std::size_t j = 0; j < Ny; ++j)
            {
                BoundaryValues[j] = BoundaryValues_[j];
            }

        }
    }