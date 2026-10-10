#pragma once

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Each object owns one gnuplot session and one comparison window.
class Contour
{
public:
    struct WindowOptions
    {
        std::string Title = "Solution Comparison";
        int Width = 1500;
        int Height = 800;
        std::string XLabel = "x";
        std::string YLabel = "y";
        std::string ColorLabel = "Field value";
        std::size_t Columns = 0; // Zero selects an automatic layout.
        std::optional<std::pair<double, double>> ColorRange = std::nullopt;
        // Empty: discover the root from the working directory or source location.
        std::filesystem::path ProjectRoot;
    };

    explicit Contour(std::string_view title = "Solution Comparison");
    explicit Contour(const WindowOptions& options);
    ~Contour();

    Contour(const Contour&) = delete;
    Contour& operator=(const Contour&) = delete;
    Contour(Contour&&) = delete;
    Contour& operator=(Contour&&) = delete;

    // Directory is relative to ProjectRoot; fileName is a single filename.
    // Each call adds a panel and redraws all panels in the same window.
    void AddContour(const std::filesystem::path& relativeDirectory,
                    std::string_view fileName,
                    std::string_view legend);

    [[nodiscard]] std::size_t GetContourCount() const noexcept
    {
        return Contours.size();
    }

private:
    struct PipeCloser
    {
        void operator()(FILE* pipe) const noexcept;
    };

    struct Point
    {
        double X;
        double Y;
        double Value;
    };

    struct ContourData
    {
        std::string Legend;
        std::vector<Point> Points;
        std::size_t XNodes = 0;
    };

    WindowOptions Options;
    std::filesystem::path ProjectRoot;
    std::unique_ptr<FILE, PipeCloser> GnuplotPipe;
    std::vector<ContourData> Contours;

    [[nodiscard]] static std::string Quote(std::string_view text);
    [[nodiscard]] static std::filesystem::path ResolveProjectRoot(
        const std::filesystem::path& root);
    [[nodiscard]] static ContourData ReadContour(
        const std::filesystem::path& file, std::string_view legend);
    [[nodiscard]] std::string BuildPlotCommands() const;
    void SendCommands(const std::string& commands);
};
