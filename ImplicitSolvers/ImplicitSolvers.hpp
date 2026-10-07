#pragma once

#include "LinearSolvers/Matrix/CoefficientMatrix.hpp"
#include "LinearSolvers/Matrix/RHS.hpp"
#include "LinearSolvers/Matrix/TridiagonalMatrix.hpp"
#include "Core/Field/Field.hpp"

namespace DiffusionEQ
{
namespace Laasonen
{
void PassCoefficient(CoefficientMatrix& A_Obj,double DiffNumberX_,double DiffNumberY_) noexcept;
// void PassCoefficient(TridiagonalMatrix& A3_Obj, double DiffNumber_) noexcept;
namespace RightHandSide
{
void PassRHS(RHS& RHS_Obj, const Field& Field_Obj) noexcept;
} // namespace RightHandSide
} // namespace Laasonen

namespace CrankNicolson
{
void PassCoefficient(CoefficientMatrix& A_Obj, double DiffNumberX_,double DiffNumberY_) noexcept;
// void PassCoefficient(TridiagonalMatrix& A3_Obj, double DiffNumber_) noexcept;
namespace RightHandSide
{
void PassRHS(
    RHS& RHS_Obj,
    const Field& Field_Obj,
    double DiffNumberX_,
    double DiffNumberY_
) noexcept;
} // namespace RightHandSide
} // namespace CrankNicolson

namespace DirichletBoundaryConditions
{
inline void Apply(CoefficientMatrix& A_Obj) noexcept
{

    std::size_t Nx=A_Obj.Get_Nx();
    std::size_t Ny=A_Obj.Get_Ny();

    // 1. Top Boundary (j = Ny - 1)
    {
        const std::size_t j = Ny - 1;
        for (std::size_t i = 0; i < Nx; ++i)
        {
            const std::size_t p = j * Nx + i;
            A_Obj[p][p] = 1.0;
        }
    }

    // 2. Bottom Boundary (j = 0)
    {
        const std::size_t j = 0;
        for (std::size_t i = 0; i < Nx; ++i)
        {
            const std::size_t p = j * Nx + i;
            A_Obj[p][p] = 1.0;
        }
    }

    // 3. Right Boundary (i = Nx - 1)
    {
        const std::size_t i = Nx - 1;
        for (std::size_t j = 0; j < Ny; ++j)
        {
            const std::size_t p = j * Nx + i;
            A_Obj[p][p] = 1.0;
        }
    }

    // 4. Left Boundary (i = 0)
    {
        const std::size_t i = 0;
        for (std::size_t j = 0; j < Ny; ++j)
        {
            const std::size_t p = j * Nx + i;
            A_Obj[p][p] = 1.0;
        }
    }
}

// inline void Apply(TridiagonalMatrix& A3_Obj) noexcept
// {
//     std::size_t N = A3_Obj.GetMSize();

//     //Apply Dirichlet boundary condition To MainDiagonal Values
//     A3_Obj.M(0) = 1.0; //for Diriclet BC
//     A3_Obj.M(N - 1) = 1.0; //for Diriclet BC

//     //Apply Dirichlet boundary condition To UpperDiagonal Values
//     A3_Obj.U(0) = 0.0; //for Diriclet BC
//     A3_Obj.U(N - 1) = 0.0; // unused: just for Even index

//     //Apply Dirichlet boundary condition To LowerDiagonal Values
//     A3_Obj.L(0) = 0.0; // unused: just for Even index
//     A3_Obj.L(N - 1) = 0.0; //for Diriclet BC
// }

inline void Apply(RHS& RHS_Obj, const Field& Field_Obj) noexcept
{
{
    std::size_t Nx=Field_Obj.Get_Nx();
    std::size_t Ny= Field_Obj.Get_Ny();
    std::size_t p;

    // 1. Top Boundary (j = Ny - 1)
    {
        const std::size_t j = Ny - 1;
        for (std::size_t i = 0; i < Nx; ++i)
        {
            p = j*Nx + i;
            RHS_Obj[p] = Field_Obj[j][i];
        }
    }
    // 2. Bottom Boundary (j = 0)
    {
        const std::size_t j = 0;
        for (std::size_t i = 0; i < Nx; ++i)
        {
            p = j*Nx + i;
            RHS_Obj[p] = Field_Obj[j][i];
        }
    }

    // 3. Right Boundary (i = Nx - 1)
    {
        const std::size_t i = Nx - 1;
        for (std::size_t j = 0; j < Ny; ++j)
        {
            p = j*Nx + i;
            RHS_Obj[p] =Field_Obj[j][i];
        }
    }

    // 4. Left Boundary (i = 0)
    {
        const std::size_t i = 0;
        for (std::size_t j = 0; j < Ny; ++j)
        {
            p = j*Nx + i;
            RHS_Obj[p] = Field_Obj[j][i];
        }
    }

    
}
}

} // namespace DirichletBoundaryConditions
} // namespace DiffusionEQ
