#pragma once

#include <cstddef>
#include <vector>
#include "Core/Geometry/Geometry.hpp"

class Mesh
{
private:
    std::size_t Nx; // Number of nodes in x-direction; intervals = Nx - 1
    std::size_t Ny; // Number of nodes in y-direction; intervals = Ny - 1
    std::size_t N;  // Total number of nodes
    double Width;   // Width of the domain
    double Height;  // Height of the domain
    double DeltaX;  // Grid spacing in x-direction
    double DeltaY;  // Grid spacing in y-direction

    std::vector<double> XGrid; // x-coordinates of grid nodes : i = 0, 1, ..., Nx-1
    std::vector<double> YGrid; // y-coordinates of grid nodes : j = 0, 1, ..., Ny-1

private:
    void MakeXGrid();
    void MakeYGrid();

public:
    // Constructor
    explicit Mesh(
        const Geometry& Geometry_Obj, 
        std::size_t XNodes_, 
        std::size_t YNodes_);

public:
    
    [[nodiscard]] std::size_t Get_Nx() const noexcept { return Nx; }
    [[nodiscard]] std::size_t Get_Ny() const noexcept { return Ny; }
    [[nodiscard]] std::size_t Get_N() const noexcept { return N; }
    [[nodiscard]] double Get_DeltaX() const noexcept { return DeltaX; }
    [[nodiscard]] double Get_DeltaY() const noexcept { return DeltaY; }
    [[nodiscard]] const std::vector<double>& Get_XGrid() const noexcept { return XGrid; }
    [[nodiscard]] const std::vector<double>& Get_YGrid() const noexcept { return YGrid; }

    [[nodiscard]] double X(std::size_t i) const noexcept
    {return XGrid[i];}

    [[nodiscard]] double Y(std::size_t j) const noexcept
    {return YGrid[j];}


    void Set_Nx(std::size_t Nx_);
    void Set_Ny(std::size_t Ny_);
    void Print() const noexcept;


    };

