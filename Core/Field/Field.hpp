#pragma once
#include <stdexcept>
#include <cstddef>
#include <vector>
#include <utility>
#include "Core/Field/Field1D.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "Core/Boundary/Boundary.hpp"

class Field
{
private:
    std::size_t Nx;
    std::size_t Ny;
    std::size_t N;
    std::vector<double> field; // Flattened 2D Field field[j * Nx + i]

public:
    // Constructor
    explicit Field(const Mesh& Mesh_Obj);

    // Getters
    [[nodiscard]] std::size_t Get_Nx() const noexcept
    {
        return Nx;
    }

    [[nodiscard]] std::size_t Get_Ny() const noexcept
    {
        return Ny;
    }

    [[nodiscard]] std::size_t Get_N() const noexcept
    {
        return N;
    }

    [[nodiscard]] double GetValue(std::size_t j, std::size_t i) const noexcept
    {
        return field[j * Nx + i];
    }

    [[nodiscard]] const std::vector<double>& GetField() const noexcept
    {
        return field;
    }
//------------------------------------------------------------------------------
    // Direct  access using operator() for zero-overhead matrix indexing Field_Obj(j, i)
    [[nodiscard]] double operator()(std::size_t j, std::size_t i) const noexcept
    {
        return field[j * Nx + i];
    }

    [[nodiscard]] double& operator()(std::size_t j, std::size_t i) noexcept
    {
        return field[j * Nx + i];
    }
//------------------------------------------------------------------------------
    // Returns a pointer to the start of the row 'j' for row-major access: field_Obj[j][i]
    // Enables zero-overhead  indexing syntax on flat storage
    [[nodiscard]] double* operator[](std::size_t j) noexcept
    {
        return &field[j * Nx]; // Fixed: j * Nx instead of j * N
    }

    // Returns a const pointer to the start of the row 'j' for read-only access
    [[nodiscard]] const double* operator[](std::size_t j) const noexcept
    {
        return &field[j * Nx]; // Fixed: j * Nx instead of j * N
    }
//------------------------------------------------------------------------------

    // Setters
    void SetValue(std::size_t j, std::size_t i, double Value_) noexcept
    {
        field[j * Nx + i] = Value_;
    }

    void SetAllValues(double Value_) noexcept;

    void SetIntitialProfile(const std::vector<double>& InitialProfile_);


    // Boundary Conditions
    // j = 0 and i= 0 -> Nx-1: Bottom
    // j = Ny-1 and i= 0 -> Nx-1: Top
    // i = 0 and j = 0 -> Ny-1: Left
    // i = Nx-1 and j = 0 -> Ny-1: Right
    void ApplyBoundaryCondition(const Boundary& Boundary_Obj)
    {

        if (Boundary_Obj.GetOrientation() == BoundaryOrientation::Horizontal)
        {
            if (Boundary_Obj.GetSize() != Nx)
            {
                throw std::invalid_argument("Boundary_Obj size must be equal to Nx.");
            }

            if (Boundary_Obj.GetLocation() == BoundaryLocation::Top)
            {
                const std::size_t j = Ny - 1;
                for (std::size_t i = 0; i < Nx; ++i)
                {
                    field[j * Nx + i] = Boundary_Obj[i];
                }
            }
            else if (Boundary_Obj.GetLocation() == BoundaryLocation::Bottom)
            {
                const std::size_t j = 0;
                for (std::size_t i = 0; i < Nx; ++i)
                {
                    field[j * Nx + i] = Boundary_Obj[i];
                }
            }
        }
        else if (Boundary_Obj.GetOrientation() == BoundaryOrientation::Vertical)
        {
            if (Boundary_Obj.GetSize() != Ny)
            {
                throw std::invalid_argument("Boundary_Obj size must be equal to Ny.");
            }
            if (Boundary_Obj.GetLocation() == BoundaryLocation::Right)
            {
                const std::size_t i = Nx - 1;
                for (std::size_t j = 0; j < Ny; ++j)
                {
                    field[j * Nx + i] = Boundary_Obj[j];
                }
            }
            else if (Boundary_Obj.GetLocation() == BoundaryLocation::Left)
            {
                const std::size_t i = 0;
                for (std::size_t j = 0; j < Ny; ++j)
                {
                    field[j * Nx + i] = Boundary_Obj[j];
                }
            }
        }

    }

    void SetRow(std::size_t Row_Index,const Field1D& Field1D_Obj);
    void SetCol(std::size_t Col_Index,const Field1D& Field1D_Obj);

    // Methods
    // Inlined in header: Called frequently in the time-marching loop (T_old <-> T_new)
    void Swap(Field& Other) noexcept
    {
        using std::swap;
        swap(Nx, Other.Nx);
        swap(Ny, Other.Ny);
        swap(N, Other.N);
        field.swap(Other.field); // O(1) pointer swap inside std::vector
    }

    void Print(const Mesh& Mesh_Obj) const;
};
