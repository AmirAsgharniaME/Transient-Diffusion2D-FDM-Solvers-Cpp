#pragma once

#include <cstddef>
#include <vector>
#include <utility>
#include "Core/Mesh/Mesh.hpp"
#include "ADI/SweepDirection/SweepDirection.hpp"

class Field1D
{

private:
    SweepDirection Direction;
    std::size_t Nx;
    std::size_t Ny;
    std::size_t n;
    std::vector<double> field1D;

public:
    // Constructor
    explicit Field1D(SweepDirection Direction_,Mesh& Mesh_Obj);


    // Getters
    [[nodiscard]] std::size_t GetSize() const noexcept
    {
        return n;
    }

    [[nodiscard]] double GetValue(std::size_t Index_) const noexcept
    {
        return field1D[Index_];
    }

    [[nodiscard]] const std::vector<double>& GetField() const noexcept
    {
        return field1D;
    }
    
    // Just For Reading
    [[nodiscard]] double operator[](std::size_t Index_) const noexcept
    {
        return field1D[Index_];
    }
    //For both Reading and Writing
    [[nodiscard]] double& operator[](std::size_t Index_) noexcept
    {
        return field1D[Index_];
    }

    // Setters
    void SetValue(std::size_t Index_, double Value_) noexcept
    {
        field1D[Index_] = Value_;
    }

    void SetAllValues(double Value_) noexcept
{
    for (std::size_t i = 0; i < n; ++i)
    {
        field1D[i] = Value_;
    }   
}



    /*
    if the line orientation is horizontal, the boundary condition is applied at the left and right boundaries
    if the line orientation is vertical, the boundary condition is applied at the bottom and top boundaries
  
 
    void ApplyBoundaryCondition(const Boundary& Boundary_Obj) noexcept
    {

        if (Boundary_Obj.GetOrientation() == Orientation::Horizontal)
        {
            if (Boundary_Obj.GetLocation() == BoundaryLocation::Left)
            {
                field[0] = Boundary_Obj.GetValue();
            }
            if (Boundary_Obj.GetLocation() == BoundaryLocation::Right)
            {
                field[N - 1] = Boundary_Obj.GetValue();
            }

        }
        else if (Boundary_Obj.GetOrientation()  == Orientation::Vertical)
        {
            if (Boundary_Obj.GetLocation() == BoundaryLocation::Bottom)
            {
                 field[0] = Boundary_Obj.GetValue();
            }
            if (Boundary_Obj.GetLocation() == BoundaryLocation::Top)
            {
                field[N - 1] = Boundary_Obj.GetValue();
            }

        }

    } 
  */
    

    // Methods
    // Inlined in header: Called frequently in the time-marching loop (T_old <-> T_new)
    void Swap(Field1D& Other) noexcept
    {
        using std::swap;
        swap(n, Other.n);
        field1D.swap(Other.field1D); // O(1) pointer swap inside std::vector
    }

    // void Print() const noexcept;


};
