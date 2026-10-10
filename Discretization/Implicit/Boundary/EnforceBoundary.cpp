#include "Discretization/Implicit/Boundary/EnforceBoundary.hpp"
#include "Core/Boundary/BoundaryManager/BoundaryManager.hpp"

#include <stdexcept>

namespace Discretization::Implicit::Boundary
{
// Enforce boundary conditions on full coefficient matrix
void Enforce(::CoefficientMatrix& A_Obj, const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
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
    else if(Boundaries_Obj.GetType() == BoundaryType::Neumann || Boundaries_Obj.GetType() == BoundaryType::Robin)
    {
        throw std::invalid_argument(
            "Boundary Conditions: Neumann and Robin Boundary Conditions are not Implemented yet.");
    }
}

// Enforce boundary conditions on right-hand side vector
void Enforce(::RHS& RHS_Obj, const Field& Field_Obj, const SimulationBoundaries& Boundaries_Obj)
{
    if(Boundaries_Obj.GetType() == BoundaryType::Dirichlet)
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
    else if(Boundaries_Obj.GetType() == BoundaryType::Neumann || Boundaries_Obj.GetType() == BoundaryType::Robin)
    {
        throw std::invalid_argument(
            "Boundary Conditions: Neumann and Robin Boundary Conditions are not Implemented yet.");
    }
}
}
