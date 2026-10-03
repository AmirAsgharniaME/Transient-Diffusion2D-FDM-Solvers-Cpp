#pragma once
#include <vector>

#include "Core/Boundary/BoundaryManager/BoundaryManager.hpp"
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
    Boundary(
        const Mesh& Mesh_Obj,
        const std::vector<double>& BoundaryValues_,
        BoundaryType Type_,
        BoundaryOrientation Orientation_,
        BoundaryLocation Location_
    ) noexcept;

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
    void SetValue(std::vector<double>& BoundaryValues_);


    void SetType(BoundaryType Type_) noexcept
    {
        Type = Type_;
    }

    void SetLocation(BoundaryLocation Location_) noexcept
    {
        Location = Location_;
    }
};
