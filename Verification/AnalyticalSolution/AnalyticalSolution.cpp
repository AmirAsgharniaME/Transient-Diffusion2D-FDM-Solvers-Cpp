#include "Verification/AnalyticalSolution/AnalyticalSolution.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace
{
class CompensatedSum
{
public:
    void Add(long double value)
    {
        const long double adjusted = value - Correction;
        const long double next = Sum + adjusted;
        Correction = (next - Sum) - adjusted;
        Sum = next;
    }

    [[nodiscard]] long double Get() const noexcept { return Sum; }

private:
    long double Sum = 0.0L;
    long double Correction = 0.0L;
};

struct Problem
{
    std::size_t Nx;
    std::size_t Ny;
    long double Width;
    long double Height;
};

void ValidateBoundary(const Boundary& boundary,
                      BoundaryLocation location,
                      BoundaryOrientation orientation,
                      std::size_t expectedSize)
{
    if (boundary.GetType() != BoundaryType::Dirichlet ||
        boundary.GetLocation() != location ||
        boundary.GetOrientation() != orientation ||
        boundary.GetSize() != expectedSize)
    {
        throw std::invalid_argument(
            "AnalyticalSolution: each wall must be a correctly sized Dirichlet boundary.");
    }
    for (std::size_t index = 0; index < expectedSize; ++index)
    {
        if (!std::isfinite(boundary[index]))
        {
            throw std::invalid_argument(
                "AnalyticalSolution: boundary values must be finite.");
        }
    }
}

Problem Validate(const Field& output,
                 const Geometry& geometry,
                 const Mesh& mesh,
                 const Boundaries& boundaries)
{
    const std::size_t nx = mesh.Get_Nx();
    const std::size_t ny = mesh.Get_Ny();
    const double width = geometry.GetWidth();
    const double height = geometry.GetHeight();

    if (nx < 2 || ny < 2 || output.Get_Nx() != nx || output.Get_Ny() != ny)
    {
        throw std::invalid_argument(
            "AnalyticalSolution: output dimensions must match a mesh with at least two nodes per direction.");
    }
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0.0 || height <= 0.0)
    {
        throw std::invalid_argument(
            "AnalyticalSolution: width and height must be positive and finite.");
    }

    const long double widthTolerance =
        64.0L * std::numeric_limits<double>::epsilon() * width;
    const long double heightTolerance =
        64.0L * std::numeric_limits<double>::epsilon() * height;
    for (std::size_t i = 0; i < nx; ++i)
    {
        const long double expected = static_cast<long double>(width) * i / (nx - 1);
        if (!std::isfinite(mesh.X(i)) ||
            std::abs(static_cast<long double>(mesh.X(i)) - expected) > widthTolerance)
        {
            throw std::invalid_argument(
                "AnalyticalSolution: x mesh must be uniform and span [0, Width].");
        }
    }
    for (std::size_t j = 0; j < ny; ++j)
    {
        const long double expected = static_cast<long double>(height) * j / (ny - 1);
        if (!std::isfinite(mesh.Y(j)) ||
            std::abs(static_cast<long double>(mesh.Y(j)) - expected) > heightTolerance)
        {
            throw std::invalid_argument(
                "AnalyticalSolution: y mesh must be uniform and span [0, Height].");
        }
    }

    ValidateBoundary(boundaries.TopWall, BoundaryLocation::Top,
                     BoundaryOrientation::Horizontal, nx);
    ValidateBoundary(boundaries.BottomWall, BoundaryLocation::Bottom,
                     BoundaryOrientation::Horizontal, nx);
    ValidateBoundary(boundaries.RightWall, BoundaryLocation::Right,
                     BoundaryOrientation::Vertical, ny);
    ValidateBoundary(boundaries.LeftWall, BoundaryLocation::Left,
                     BoundaryOrientation::Vertical, ny);

    return {nx, ny, width, height};
}

long double HarmonicLifting(long double x, long double y,
                            const Problem& problem, const Boundaries& boundaries)
{
    const long double xFraction = x / problem.Width;
    const long double yFraction = y / problem.Height;
    const long double bottomLeft = boundaries.LeftWall[0];
    const long double topLeft = boundaries.LeftWall[problem.Ny - 1];
    const long double bottomRight = boundaries.RightWall[0];
    const long double topRight = boundaries.RightWall[problem.Ny - 1];

    return (1.0L - xFraction) *
               ((1.0L - yFraction) * bottomLeft + yFraction * topLeft) +
           xFraction *
               ((1.0L - yFraction) * bottomRight + yFraction * topRight);
}

std::vector<long double> WallCoefficients(const std::vector<long double>& values,
                                          long double length,
                                          std::size_t modeCount)
{
    const std::size_t intervals = values.size() - 1;
    const long double spacing = length / intervals;
    const long double pi = std::acos(-1.0L);
    std::vector<long double> coefficients(modeCount, 0.0L);

    for (std::size_t mode = 1; mode <= modeCount; ++mode)
    {
        const long double waveNumber = static_cast<long double>(mode) * pi / length;
        const long double endSign = mode % 2 == 0 ? 1.0L : -1.0L;
        CompensatedSum integral;
        integral.Add((values.front() - endSign * values.back()) / waveNumber);

        for (std::size_t index = 0; index < intervals; ++index)
        {
            const long double leftSine = index == 0
                ? 0.0L
                : std::sin(waveNumber * spacing * index);
            const long double rightSine = index + 1 == intervals
                ? 0.0L
                : std::sin(waveNumber * spacing * (index + 1));
            const long double slope = (values[index + 1] - values[index]) / spacing;
            integral.Add(slope * (rightSine - leftSine) /
                         (waveNumber * waveNumber));
        }
        coefficients[mode - 1] = 2.0L * integral.Get() / length;
    }
    return coefficients;
}

long double SinhRatio(long double waveNumber,
                      long double distance,
                      long double length)
{
    return std::exp(-waveNumber * (length - distance)) *
           (-std::expm1(-2.0L * waveNumber * distance)) /
           (-std::expm1(-2.0L * waveNumber * length));
}

void ApplyBoundaries(Field& output, const Boundaries& boundaries,
                     const Problem& problem)
{
    for (std::size_t i = 0; i < problem.Nx; ++i)
    {
        output(problem.Ny - 1, i) = boundaries.TopWall[i];
    }
    for (std::size_t i = 0; i < problem.Nx; ++i)
    {
        output(0, i) = boundaries.BottomWall[i];
    }
    for (std::size_t j = 0; j < problem.Ny; ++j)
    {
        output(j, problem.Nx - 1) = boundaries.RightWall[j];
    }
    for (std::size_t j = 0; j < problem.Ny; ++j)
    {
        output(j, 0) = boundaries.LeftWall[j];
    }
}

void CalculateSteady(Field& output,
                     const Mesh& mesh,
                     const Boundaries& boundaries,
                     const Problem& problem)
{
    const std::size_t maxIntervals = std::max(problem.Nx - 1, problem.Ny - 1);
    const std::size_t steadyModes =
        std::max<std::size_t>(128, 4 * std::min<std::size_t>(maxIntervals, 1024));
    const long double pi = std::acos(-1.0L);

    std::vector<long double> top(problem.Nx, 0.0L);
    std::vector<long double> bottom(problem.Nx, 0.0L);
    std::vector<long double> right(problem.Ny, 0.0L);
    std::vector<long double> left(problem.Ny, 0.0L);

    for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
    {
        top[i] = static_cast<long double>(boundaries.TopWall[i]) -
                 HarmonicLifting(mesh.X(i), problem.Height, problem, boundaries);
        bottom[i] = static_cast<long double>(boundaries.BottomWall[i]) -
                    HarmonicLifting(mesh.X(i), 0.0L, problem, boundaries);
    }
    for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
    {
        right[j] = static_cast<long double>(boundaries.RightWall[j]) -
                   HarmonicLifting(problem.Width, mesh.Y(j), problem, boundaries);
        left[j] = static_cast<long double>(boundaries.LeftWall[j]) -
                  HarmonicLifting(0.0L, mesh.Y(j), problem, boundaries);
    }

    const auto topCoefficients = WallCoefficients(top, problem.Width, steadyModes);
    const auto bottomCoefficients = WallCoefficients(bottom, problem.Width, steadyModes);
    const auto rightCoefficients = WallCoefficients(right, problem.Height, steadyModes);
    const auto leftCoefficients = WallCoefficients(left, problem.Height, steadyModes);

    output.SetAllValues(0.0);
    for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
    {
        const long double y = mesh.Y(j);
        for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
        {
            const long double x = mesh.X(i);
            CompensatedSum value;
            value.Add(HarmonicLifting(x, y, problem, boundaries));

            for (std::size_t mode = 1; mode <= steadyModes; ++mode)
            {
                const long double waveNumberX =
                    static_cast<long double>(mode) * pi / problem.Width;
                const long double waveNumberY =
                    static_cast<long double>(mode) * pi / problem.Height;
                value.Add(std::sin(waveNumberX * x) *
                    (topCoefficients[mode - 1] *
                         SinhRatio(waveNumberX, y, problem.Height) +
                     bottomCoefficients[mode - 1] *
                         SinhRatio(waveNumberX, problem.Height - y, problem.Height)));
                value.Add(std::sin(waveNumberY * y) *
                    (rightCoefficients[mode - 1] *
                         SinhRatio(waveNumberY, x, problem.Width) +
                     leftCoefficients[mode - 1] *
                         SinhRatio(waveNumberY, problem.Width - x, problem.Width)));
            }
            output(j, i) = static_cast<double>(value.Get());
        }
    }
    ApplyBoundaries(output, boundaries, problem);
}

void CalculateTransient(Field& output,
                        const Mesh& mesh,
                        const Problem& problem,
                        const std::vector<double>& initialValues,
                        double time,
                        double diffusivity)
{
    const std::size_t modesX = problem.Nx - 2;
    const std::size_t modesY = problem.Ny - 2;
    if (modesX == 0 || modesY == 0)
    {
        return;
    }

    const long double pi = std::acos(-1.0L);
    const long double normalization =
        4.0L / (static_cast<long double>(problem.Nx - 1) *
                static_cast<long double>(problem.Ny - 1));

    std::vector<long double> sineX(modesX * problem.Nx, 0.0L);
    std::vector<long double> sineY(modesY * problem.Ny, 0.0L);
    for (std::size_t mode = 0; mode < modesX; ++mode)
    {
        for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
        {
            sineX[mode * problem.Nx + i] = std::sin(
                static_cast<long double>(mode + 1) * pi * mesh.X(i) / problem.Width);
        }
    }
    for (std::size_t mode = 0; mode < modesY; ++mode)
    {
        for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
        {
            sineY[mode * problem.Ny + j] = std::sin(
                static_cast<long double>(mode + 1) * pi * mesh.Y(j) / problem.Height);
        }
    }

    std::vector<long double> projectedX(problem.Ny * modesX, 0.0L);
    for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
    {
        for (std::size_t modeX = 0; modeX < modesX; ++modeX)
        {
            CompensatedSum sum;
            for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
            {
                const long double residual =
                    static_cast<long double>(initialValues[j * problem.Nx + i]) -
                    output(j, i);
                sum.Add(residual * sineX[modeX * problem.Nx + i]);
            }
            projectedX[j * modesX + modeX] = sum.Get();
        }
    }

    std::vector<long double> coefficients(modesY * modesX, 0.0L);
    for (std::size_t modeY = 0; modeY < modesY; ++modeY)
    {
        const long double waveNumberY =
            static_cast<long double>(modeY + 1) * pi / problem.Height;
        for (std::size_t modeX = 0; modeX < modesX; ++modeX)
        {
            const long double waveNumberX =
                static_cast<long double>(modeX + 1) * pi / problem.Width;
            CompensatedSum sum;
            for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
            {
                sum.Add(projectedX[j * modesX + modeX] *
                        sineY[modeY * problem.Ny + j]);
            }
            const long double exponent =
                -static_cast<long double>(diffusivity) *
                (waveNumberX * waveNumberX + waveNumberY * waveNumberY) *
                static_cast<long double>(time);
            coefficients[modeY * modesX + modeX] =
                normalization * sum.Get() * std::exp(exponent);
        }
    }

    std::vector<long double> reconstructedY(problem.Ny * modesX, 0.0L);
    for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
    {
        for (std::size_t modeX = 0; modeX < modesX; ++modeX)
        {
            CompensatedSum sum;
            for (std::size_t modeY = 0; modeY < modesY; ++modeY)
            {
                sum.Add(coefficients[modeY * modesX + modeX] *
                        sineY[modeY * problem.Ny + j]);
            }
            reconstructedY[j * modesX + modeX] = sum.Get();
        }
    }

    for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
    {
        for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
        {
            CompensatedSum residual;
            for (std::size_t modeX = 0; modeX < modesX; ++modeX)
            {
                residual.Add(reconstructedY[j * modesX + modeX] *
                             sineX[modeX * problem.Nx + i]);
            }
            output(j, i) += static_cast<double>(residual.Get());
        }
    }
}

} // namespace

namespace AnalyticalSolution
{
void Pass(Field& Field_Obj,
          const Geometry& Geometry_Obj,
          const Mesh& Mesh_Obj,
          const Boundaries& Boundaries_Obj)
{
    const Problem problem = Validate(Field_Obj, Geometry_Obj, Mesh_Obj, Boundaries_Obj);
    CalculateSteady(Field_Obj, Mesh_Obj, Boundaries_Obj, problem);
}

void Pass(Field& Field_Obj,
          double Time_,
          double nu,
          const Geometry& Geometry_Obj,
          const Mesh& Mesh_Obj,
          const Boundaries& Boundaries_Obj,
          const Field& Field_0)
{
    const Problem problem = Validate(Field_Obj, Geometry_Obj, Mesh_Obj, Boundaries_Obj);
    if (!std::isfinite(Time_) || Time_ < 0.0 || !std::isfinite(nu) || nu < 0.0)
    {
        throw std::invalid_argument(
            "AnalyticalSolution: time and diffusivity must be finite and nonnegative.");
    }
    if (Field_0.Get_Nx() != problem.Nx || Field_0.Get_Ny() != problem.Ny)
    {
        throw std::invalid_argument(
            "AnalyticalSolution: initial field dimensions must match the mesh.");
    }

    std::vector<double> initialValues = Field_0.GetField();
    if (!std::all_of(initialValues.begin(), initialValues.end(),
                     [](double value) { return std::isfinite(value); }))
    {
        throw std::invalid_argument(
            "AnalyticalSolution: initial field values must be finite.");
    }

    if (Time_ == 0.0 || nu == 0.0)
    {
        for (std::size_t j = 1; j + 1 < problem.Ny; ++j)
        {
            for (std::size_t i = 1; i + 1 < problem.Nx; ++i)
            {
                Field_Obj(j, i) = initialValues[j * problem.Nx + i];
            }
        }
    }
    else
    {
        CalculateSteady(Field_Obj, Mesh_Obj, Boundaries_Obj, problem);
        CalculateTransient(Field_Obj, Mesh_Obj, problem, initialValues, Time_, nu);
    }
    ApplyBoundaries(Field_Obj, Boundaries_Obj, problem);
}
} // namespace AnalyticalSolution
