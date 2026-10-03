#pragma once

enum class BoundaryLocation
{
    Top,
    Bottom,
    Right,
    Left
};

// Boundary condition types in CFD / Heat Transfer
enum class BoundaryType
{
    Dirichlet, // Fixed value (e.g., fixed temperature)
    Neumann,   // Fixed flux / derivative (e.g., insulated wall: dT/dx = 0)
    Robin      // Mixed / convection condition
};

enum class BoundaryOrientation
{
    Horizontal,
    Vertical
};