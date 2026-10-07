#pragma once
#include <vector>
#include <cstddef>
#include "Core/Mesh/Mesh.hpp"
#include "Config/SweepDirection.hpp"

class TridiagonalMatrix
{

private:
    SweepDirection Direction;
    std::size_t Nx;
    std::size_t Ny;
    std::vector<double> LowerDiagonal;
    std::vector<double> MainDiagonal;
    std::vector<double> UpperDiagonal;


public:
    TridiagonalMatrix(SweepDirection SweepDirection_, const Mesh& Mesh_Obj)
        :Direction(SweepDirection_),
         Nx(Mesh_Obj.Get_Nx()),
         Ny(Mesh_Obj.Get_Ny())

    {
         const std::size_t n =(Direction == SweepDirection::X) ? Nx : Ny;
         LowerDiagonal.assign(n, 0.0);
         MainDiagonal.assign(n, 0.0);
         UpperDiagonal.assign(n, 0.0);
    }


std::size_t GetMSize() const noexcept
{
    return MainDiagonal.size();
}
std::size_t GetUSize()const noexcept
{
    return UpperDiagonal.size();
}

std::size_t GetLSize() const noexcept
{
    return LowerDiagonal.size();
}

double GetMidValue(std::size_t Index_) const noexcept
{
    return MainDiagonal[Index_];
}
double GetUValue(std::size_t Index_) const noexcept
{
    return UpperDiagonal[Index_];
}
double GetLValue(std::size_t Index_) const noexcept
{
    return LowerDiagonal[Index_];
}


void SetMValue(std::size_t Index_, double Value_) noexcept
{
    MainDiagonal[Index_] = Value_;
}

void SetUValue(std::size_t Index_, double Value_) noexcept
{
    UpperDiagonal[Index_] = Value_;
}

void SetLValue(std::size_t Index_, double Value_) noexcept
{
    LowerDiagonal[Index_] = Value_;
}

// ==========================================
// 1. Main Diagonal Access
// ==========================================
// Read-only access (returns by const reference to avoid copy)
[[nodiscard]] const double& M(std::size_t Index_) const noexcept
{
    return MainDiagonal[Index_];
}

// Mutable access (returns by reference for reading and writing)
[[nodiscard]] double& M(std::size_t Index_) noexcept
{
    return MainDiagonal[Index_];
}

// Overloading operator[] for intuitive access to the Main Diagonal
[[nodiscard]] const double& operator[](std::size_t Index_) const noexcept
{
    return MainDiagonal[Index_];
}

[[nodiscard]] double& operator[](std::size_t Index_) noexcept
{
    return MainDiagonal[Index_];
}

// ==========================================
// 2. Upper Diagonal Access
// ==========================================
// Read-only access (returns by const reference)
[[nodiscard]] const double& U(std::size_t Index_) const noexcept
{
    return UpperDiagonal[Index_];
}

// Mutable access (returns by reference)
[[nodiscard]] double& U(std::size_t Index_) noexcept
{
    return UpperDiagonal[Index_];
}

// ==========================================
// 3. Lower Diagonal Access
// ==========================================
// Read-only access (returns by const reference)
[[nodiscard]] const double& L(std::size_t Index_) const noexcept
{
    return LowerDiagonal[Index_];
}

// Mutable access (returns by reference)
[[nodiscard]] double& L(std::size_t Index_) noexcept
{
    return LowerDiagonal[Index_];
}
    
};


