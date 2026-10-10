#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "Utilities/Parameter/Parameter.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "Core/Setup/Params.hpp"

inline void Stabilization(const Mesh& Mesh_obj, Params& params)
{
    const double current_dt = params.dt.GetValue();
    const double delta_x = Mesh_obj.Get_DeltaX();
    const double delta_y = Mesh_obj.Get_DeltaY();
    const double alpha = params.alpha.GetValue();
    const double diffusion_number_x = params.DiffNumberX.GetValue();
    const double diffusion_number_y = params.DiffNumberY.GetValue();

    if (!std::isfinite(current_dt) || current_dt <= 0.0 ||
        !std::isfinite(alpha) || alpha <= 0.0 ||
        !std::isfinite(delta_x) || delta_x <= 0.0 ||
        !std::isfinite(delta_y) || delta_y <= 0.0 ||
        !std::isfinite(diffusion_number_x) || diffusion_number_x < 0.0 ||
        !std::isfinite(diffusion_number_y) || diffusion_number_y < 0.0)
    {
        throw std::invalid_argument(
            "FTCS stabilization requires finite positive dt, alpha, and mesh spacings, "
            "and finite nonnegative diffusion numbers.");
    }

    // The 2D FTCS stability condition is DiffNumberX + DiffNumberY <= 0.5.
    const double safety_factor = 0.95;
    const double stability_limit = 0.5;
    const double diffusion_rate =
        alpha * (1.0 / (delta_x * delta_x) + 1.0 / (delta_y * delta_y));
    if (!std::isfinite(diffusion_rate) || diffusion_rate <= 0.0)
    {
        throw std::invalid_argument(
            "FTCS stabilization could not compute a finite stability limit from the mesh.");
    }
    const double max_allowed_dt = safety_factor * stability_limit / diffusion_rate;

    if (diffusion_number_x + diffusion_number_y <= stability_limit)
    {
        std::cout << "[INFO] FTCS Scheme is STABLE. Current Diffusion Numbers: "
                  << "X = " << diffusion_number_x
                  << ", Y = " << diffusion_number_y << '\n';
        return;
    }

    std::cout << "[WARNING] FTCS stability requires DiffusionNumberX + "
              << "DiffusionNumberY <= 0.5, but current sum = "
              << diffusion_number_x + diffusion_number_y << '\n';
    std::cout << "[WARNING] FTCS is Unstable with dt = " << current_dt << '\n';

    const double new_dt = std::min(current_dt, max_allowed_dt);
    const double new_diffusion_number_x = alpha * new_dt / (delta_x * delta_x);
    const double new_diffusion_number_y = alpha * new_dt / (delta_y * delta_y);

    params.dt.SetValue(new_dt);
    params.DiffNumberX.SetValue(new_diffusion_number_x);
    params.DiffNumberY.SetValue(new_diffusion_number_y);

    std::cout << "[FIX] dt has been reduced for the stability of FTCS method.\n";
    std::cout << "      New dt = " << new_dt << '\n';
    std::cout << "      New Diffusion Numbers: X = " << new_diffusion_number_x
              << ", Y = " << new_diffusion_number_y << '\n';
}
