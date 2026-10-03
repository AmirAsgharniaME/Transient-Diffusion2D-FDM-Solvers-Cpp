#pragma once
#include <vector>
#include <cstddef>

namespace SolverInputs 
{

    namespace Geometry 
    {
        inline constexpr double Width = 0.04; //[m]
        inline constexpr double Height = 0.04; //[m]
    }

    namespace Physics 
    {
        inline constexpr double alpha = 0.001; //[m^2/s]
    }

    namespace Solver 
    {
        inline constexpr std::size_t NumTimeLevels = 20000;
        inline constexpr double dt = 0.001; //[s]
        inline constexpr double Tolerance = 1e-6;
    }

    namespace Mesh 
    {
        inline constexpr std::size_t Nx = 7;
        inline constexpr std::size_t Ny = 7;
        inline constexpr std::size_t N = Nx * Ny;
    }

    namespace InitialCondition 
    {
        inline const std::vector<double> InitialProfile(Mesh::N, 0.0); //Flatted 2D initial condition
    }

    namespace BoundaryCondition 
    {
        inline const std::vector<double> TopWallValue(Mesh::Nx, 0.0);
        inline const std::vector<double> BottomWallValue(Mesh::Nx, 200.0);
        inline const std::vector<double> RightWallValue(Mesh::Ny, 0.0);
        inline const std::vector<double> LeftWallValue(Mesh::Ny, 200.0);
       
    }

} // namespace SolverInputs
