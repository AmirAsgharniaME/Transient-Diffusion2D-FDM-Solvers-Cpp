#pragma once

#include <vector>
#include <cstddef>
#include "Core/Mesh/Mesh.hpp"


class RHS
{

private:

    std::size_t Nx;
    std::size_t Ny;
    std::size_t N;

    //flat
    std::vector<double> rhs;

public:

    explicit RHS(const Mesh& Mesh_Obj)
        :Nx(Mesh_Obj.Get_Nx()),
         Ny(Mesh_Obj.Get_Ny()),
         N(Nx*Ny),
         rhs(N, 0.0)
    {}

    void SetValue(std::size_t PIndex, double Value_) noexcept
    {rhs[PIndex] = Value_;}

   [[nodiscard]]  double GetValue(std::size_t PIndex) const noexcept
    {return rhs[PIndex];}

   [[nodiscard]]  std::size_t GetSize() const noexcept
    {return rhs.size();}

    [[nodiscard]] std::size_t Get_Nx() const noexcept
    {return Nx;}

    [[nodiscard]] std::size_t Get_Ny() const noexcept
    {return Ny;}
  
    // Just For Reading
    [[nodiscard]] double operator[](std::size_t PIndex) const noexcept
    {
        return rhs[PIndex];
    }

    //For both Reading and Writing
    [[nodiscard]] double& operator[](std::size_t PIndex) noexcept
    {
        return rhs[PIndex];
    }

};
