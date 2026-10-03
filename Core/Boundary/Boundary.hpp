#pragma once
#include <vector>

#include "Config/BoundaryManager.hpp"
#include "Core/Mesh/Mesh.hpp"

class Boundary
{
private:
    std::size_t Nx;
    std::size_t Ny;
    std::vector<double> BoundaryValues;
    BoundaryType Type;
     BoundaryOrientation Orientation;
    BoundaryLocation Location;
   

public:
    // Constructor directly inlined inside the class body
         Boundary
        (
        const Mesh& Mesh_Obj,
        const std::vector<double>& BoundaryValues_,
        BoundaryType Type_,
        BoundaryOrientation Orientation_,
        BoundaryLocation Location_) noexcept
        :Nx(Mesh_Obj.Get_Nx()),
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

    [[nodiscard]] std::size_t GetSize() const noexcept
    {
        return BoundaryValues.size();
    }
    
    // Public Getters
    [[nodiscard]] double GetValue(std::size_t Index_) const noexcept
    {
        return BoundaryValues[Index_];
    }

        // Direct read-only indexing operator for simple syntax: Boundary_Obj[Index_]
    [[nodiscard]] double operator[](std::size_t Index_) const noexcept
    {
        return BoundaryValues[Index_];
    }

    [[nodiscard]] BoundaryType GetType() const noexcept
    {
        return Type;
    }

    [[nodiscard]] BoundaryLocation GetLocation() const noexcept
    {
        return Location;
    }

    [[nodiscard]] BoundaryOrientation GetOrientation() const noexcept
    {
        return Orientation;
    }

    // Public Setters
    void SetValue(std::vector<double>& BoundaryValues_)
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

    void SetType(BoundaryType Type_) noexcept
    {
        Type = Type_;
    }

    void SetLocation(BoundaryLocation Location_) noexcept
    {
        Location = Location_;
    }
};
