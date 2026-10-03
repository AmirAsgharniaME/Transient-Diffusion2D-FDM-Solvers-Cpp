#pragma once

#include <stdexcept>

class Geometry
{
private:
    double Width; //Horizontal
    double Height;  //Vertical


public:
    explicit Geometry(double Width_,double Height_)
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

    [[nodiscard]] double GetWidth() const noexcept
    {
        return Width;
    }


    [[nodiscard]] double GetHeight() const noexcept
    {
        return Height;
    }


    void SetWidth(double Width_)
    {
        if (Width_ <= 0.0)
        {
            throw std::invalid_argument("Width must be greater than zero.");
        }
        Width = Width_;
    }


    void SetHeight(double Height_)
    {
        if (Height_ <= 0.0)
        {
            throw std::invalid_argument("Height must be greater than zero.");
        }
        Height = Height_;
    }

};


