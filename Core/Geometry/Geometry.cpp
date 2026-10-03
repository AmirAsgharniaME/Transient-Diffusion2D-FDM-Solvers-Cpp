#include "Core/Geometry/Geometry.hpp"

Geometry::Geometry(double Width_, double Height_)
    : Width(Width_),
      Height(Height_)
{
    if (Width_ <= 0.0)
    {
        throw std::invalid_argument("Width must be greater than zero.");
    }
    if (Height_ <= 0.0)
    {
        throw std::invalid_argument("Height must be greater than zero.");
    }
}
