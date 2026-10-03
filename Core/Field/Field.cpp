#include "Core/Field/Field.hpp"


#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>


Field::Field(const Mesh& Mesh_Obj)
    : Nx(Mesh_Obj.Get_Nx()),
      Ny(Mesh_Obj.Get_Ny()),
      N(Nx * Ny),
      field(N, 0.0)
{}


void Field::SetAllValues(double Value_) noexcept
{
    for (std::size_t i = 0; i < N; ++i)
    {
        field[i] = Value_;
    }
}

void  Field::SetIntitialProfile(const std::vector<double>& InitialProfile_)
{
    if (InitialProfile_.size() != N)
    {
        throw std::invalid_argument("InitialProfile_ must have the same size as the mesh.");
    }
    else
    {
        for(std::size_t j = 0; j < Ny; ++j)
        {
            for (std::size_t i = 0; i < Nx; ++i)
            {
                field[j * Nx + i] = InitialProfile_[j * Nx + i];
            }
        }
    }

}


void Field::SetRow(std::size_t j,const Field1D& Field1D_Obj)
{
    for (std::size_t i = 0; i < Nx; i++)
    { 
        field[j*Nx + i] = Field1D_Obj[i];
    }
   
}
void Field::SetCol(std::size_t i,const Field1D& Field1D_Obj)
{
    for (std::size_t j = 0; j < Ny; j++)
    {
        field[j*Nx + i] = Field1D_Obj[j];
    }
}

//Print Method
void Field::Print(const Mesh& Mesh_Obj) const
{
    if (Mesh_Obj.Get_Nx() != Nx || Mesh_Obj.Get_Ny() != Ny)
    {
        throw std::invalid_argument("Mesh dimensions must match the field dimensions for printing.");
    }

    const std::ios::fmtflags oldFlags = std::cout.flags();
    const std::streamsize oldPrecision = std::cout.precision();

    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(3);

    std::cout << "Field (" << Ny << " rows x "
              << Nx << " columns)\n\n";

    const int labelWidth = 20;
    const int colWidth = 12;

   
    std::cout << std::setw(labelWidth) << "x ->";
    for (std::size_t i = 0; i < Nx; ++i)
    {
        std::cout << std::setw(colWidth) << i;
    }
    std::cout << '\n';

   
    std::cout << std::setw(labelWidth) << " ";
    for (std::size_t i = 0; i < Nx; ++i)
    {
        std::cout << std::setw(colWidth) << Mesh_Obj.X(i);
    }
    std::cout << '\n';

   
    for (std::size_t j = 0; j < Ny; ++j)
    {
        const std::size_t yIndex = Ny - 1 - j;

      
        std::string label = "j = " + std::to_string(yIndex) + " y = ";
        std::cout << std::left << std::setw(12) << label
                  << std::right << std::setw(7) << Mesh_Obj.Y(yIndex) << " ";

       
        for (std::size_t i = 0; i < Nx; ++i)
        {
            std::cout << std::setw(colWidth) << field[yIndex * Nx + i];
        }
        std::cout << '\n';
    }

    std::cout.flags(oldFlags);
    std::cout.precision(oldPrecision);
}
