#include "Core/Mesh/Mesh.hpp"

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <iomanip>



Mesh::Mesh(       
    const Geometry& Geometry_Obj, 
    std::size_t XNodes_, 
    std::size_t YNodes_)
    :Nx(XNodes_),
     Ny(YNodes_),
     N(XNodes_ * YNodes_),
     Width(Geometry_Obj.GetWidth()),
     Height(Geometry_Obj.GetHeight()),
     DeltaX(Width / static_cast<double>(Nx-1)),
     DeltaY(Height/ static_cast<double>(Ny-1)),

     XGrid(Nx,0.0),
     YGrid(Ny,0.0)
{
    if (Nx < 2)
    {
        throw std::invalid_argument("The number of mesh X nodes must be at least 2.");
    }
    if (Ny < 2)
    {
        throw std::invalid_argument("The number of mesh Ynodes must be at least 2.");
    }
    MakeXGrid();
    MakeYGrid();
}

    // public Setters
    void Mesh::Set_Nx(std::size_t Nx_)
    {
        if (Nx_ < 2)
        {
            throw std::invalid_argument("The number of mesh Xnodes must be at least 2.");
        }
        Nx = Nx_;
        N = Nx_ * Ny;
        DeltaX = Width / static_cast<double>(Nx-1);
        XGrid.resize(Nx);
        MakeXGrid();
    }
    void Mesh::Set_Ny(std::size_t Ny_)
    {
        if (Ny_ < 2)
        {
            throw std::invalid_argument("The number of mesh Ynodes must be at least 2.");
        }
        Ny = Ny_;
        N = Nx * Ny_;
        DeltaY = Height / static_cast<double>(Ny-1);
        YGrid.resize(Ny);
        MakeYGrid();
    }


    void Mesh::MakeXGrid()
    {
        for (std::size_t i = 0; i < Nx; ++i)
        {
            XGrid[i] = static_cast<double>(i) * DeltaX;
        }
        XGrid[Nx - 1] = Width; // To guarantee that it will take the exact value of Width.
    }
    void Mesh::MakeYGrid()
    {
        for (std::size_t j = 0; j < Ny; ++j)
        {
            YGrid[j] = static_cast<double>(j) * DeltaY;
        }
        YGrid[Ny - 1] = Height; // To guarantee that it will take the exact value of Height.
    }

void Mesh::Print() const noexcept
{
    const std::ios::fmtflags oldFlags = std::cout.flags();
    const std::streamsize oldPrecision = std::cout.precision();

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(3);

    std::cout << "Mesh (" << Ny << " rows x " << Nx << " columns)\n"
              << "Spacing: deltaX = " << DeltaX
              << ", deltaY = " << DeltaY << "\n\n";

    const int labelWidth = 20;
    const int colWidth = 12;

   
    std::cout << std::setw(labelWidth) << "x ->";
    for (std::size_t i = 0; i < XGrid.size(); ++i)
    {
        std::cout << std::setw(colWidth) << i;
    }
    std::cout << '\n';

  
    std::cout << std::setw(labelWidth) << " ";
    for (double x : XGrid)
    {
        std::cout << std::setw(colWidth) << x;
    }
    std::cout << '\n';

   
    for (std::size_t row = 0; row < YGrid.size(); ++row)
    {
        const std::size_t reversedRow = YGrid.size() - 1 - row;


        std::string label = "j = " + std::to_string(reversedRow) + " y = ";
        std::cout << std::left << std::setw(12) << label
                  << std::right << std::setw(7) << YGrid[reversedRow] << " ";

      
        for (std::size_t column = 0; column < XGrid.size(); ++column)
        {
            std::cout << std::setw(colWidth) << "o";
        }
        std::cout << '\n';

       
        if (row != (YGrid.size() - 1))
        {
            std::cout << std::setw(labelWidth) << " ";
            for (std::size_t column = 0; column < XGrid.size(); ++column)
            {
                std::cout << std::setw(colWidth) << "|";
            }
            std::cout << '\n';
        }
    }

    std::cout.flags(oldFlags);
    std::cout.precision(oldPrecision);
}
