#pragma once

#include <vector>
#include <cstddef>
#include "Core/Mesh/Mesh.hpp"
#include "ADI/SweepDirection/SweepDirection.hpp"



class RHS
{

private:
    SweepDirection Direction;
    std::size_t Nx;
    std::size_t Ny;
    std::size_t n;

    //flat
    std::vector<double> rhs;

public:

    explicit RHS(SweepDirection SweepDirection_,const Mesh& Mesh_Obj);

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

    void Reset(SweepDirection Direction_) noexcept
    {
            n = (Direction_ == SweepDirection::X) ? Nx : Ny;
            rhs.assign(n, 0.0);
    }

};
