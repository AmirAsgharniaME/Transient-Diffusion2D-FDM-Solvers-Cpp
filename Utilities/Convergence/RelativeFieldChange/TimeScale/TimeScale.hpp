#pragma once
#include "Core/Geometry/Geometry.hpp"

/**
 * @brief Computes the 2D diffusion characteristic time scale for a rectangular plate.
 * 
 * Formula: t_scale = (1 / alpha) * ( (Lx^2 * Ly^2) / (Lx^2 + Ly^2) )
 * 
 * @param Lx Physical length of the domain in X direction [m]
 * @param Ly Physical length of the domain in Y direction [m]
 * @param alpha Thermal diffusivity of the material [m^2 / s]
 * @return constexpr double The physical time scale (t_scale) [s]
 */


namespace TimeScale 
{

[[nodiscard]] constexpr double Return(const Geometry& Geometry_Obj,double alpha_)
{
    if (alpha_ <= 0.0)
    {
        throw std::invalid_argument("alpha must be greater than zero.");
    }
    if (Geometry_Obj.GetWidth() <= 0.0)
    {
        throw std::invalid_argument("Width must be greater than zero.");
    }
    if (Geometry_Obj.GetHeight() <= 0.0)
    {
        throw std::invalid_argument("Height must be greater than zero.");
    }

    double Lx = Geometry_Obj.GetWidth();
    double Ly = Geometry_Obj.GetHeight();


        const double Lx2 = Lx * Lx;
        const double Ly2 = Ly * Ly;

        // Harmonic combination of spatial scales for 2D diffusion
        const double effective_length = (Lx2 * Ly2) / (Lx2 + Ly2);

        return effective_length / alpha_;
}

}

