#pragma once
#include <vector>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include "Core/Mesh/Mesh.hpp"
#include "LinearSolvers/Matrix/RHS.hpp"

class CoefficientMatrix
{


private:

    std::size_t Nx;
    std::size_t Ny;
    std::size_t N;

    // Flat 1D vector of size N*N for A[j*ncols + i] in 1D : N = nrows = ncols
    std::vector<double> A; 

public:
    CoefficientMatrix(const Mesh& Mesh_Obj)
        :Nx(Mesh_Obj.Get_Nx()),
         Ny(Mesh_Obj.Get_Ny()),
         N(Nx * Ny),
         A(N * N, 0.0)
    {}

    [[nodiscard]] std::size_t GetRows() const noexcept
    {return N;}

    [[nodiscard]] std::size_t GetCols() const noexcept
    {return N;}

    [[nodiscard]] std::size_t Get_Nx() const noexcept
    {return Nx;}

    [[nodiscard]] std::size_t Get_Ny() const noexcept
    {return Ny;}

    [[nodiscard]] double GetValue(std::size_t j, std::size_t i) const noexcept
    {
        return A[j * N + i];
    }

    void SetValue(std::size_t j, std::size_t i, double Value_) noexcept
    {
        A[j * N + i] = Value_;
    }

    // Returns a pointer to the start of the row 'j' for row-major access (A[j][i]).
    // This enables 2D array syntax on a flat 1D vector with zero runtime overhead.
    [[nodiscard]] double* operator[](std::size_t j) noexcept
    {
        return &A[j * N];
    }

    // Returns a const pointer to the start of the row 'j' for read-only row-major access.
    [[nodiscard]] const double* operator[](std::size_t j) const noexcept
    {
        return &A[j * N];
    }

    


    void Print(const RHS& rhs) const noexcept
{
    // Print column header with node coordinates, location, RHS value, and matrix row entries
    std::cout << "Node(j,i) | Location |   RHS Value   | Matrix Row Content" << std::endl;
    std::cout << "----------------------------------------------------------------------------------------" << std::endl;

    for (std::size_t row = 0; row < N; row++)
    {
        // Convert the flat matrix row index back to 2D grid coordinates (j, i)
        std::size_t j = row / Nx; // nrows is Ny, ncols is Nx
        std::size_t i = row % Nx;

        // Determine boundary or interior location based on defined priority: Top, Bottom, Right, Left
        std::string location = "Interior";

        if (j == Ny - 1)
        {
            location = "Top     ";
        }
        else if (j == 0)
        {
            location = "Bottom  ";
        }
        else if (i == Nx - 1)
        {
            location = "Right   ";
        }
        else if (i == 0)
        {
            location = "Left    ";
        }

        // Print node coordinates, location label, and the corresponding RHS value
        std::cout << "(" << j << "," << i << ") | " 
                  << location << " | "
                  << std::scientific << std::setprecision(2) << std::setw(13) << rhs.GetValue(row) << " | ";

        // Print the row elements of the coefficient matrix
        for (std::size_t col = 0; col < N; col++)
        {
            if (std::abs(A[row*N + col]) < 1e-12)
            {
                std::cout << "       .      ";
            }
            else
            {
                std::cout << std::scientific
                          << std::setprecision(2)
                          << std::setw(12)
                          << A[row*N + col];
            }
        }
        std::cout << std::endl;
    }
}


};
