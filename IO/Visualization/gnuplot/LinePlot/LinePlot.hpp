#pragma once

#include <cstdio>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

class LinePlot {
public:
    enum class LineStyle {
        Solid,
        Dashed,
        Dotted,
        DashDotted
    };

    LinePlot(
        std::string_view title,
        std::string_view xLabel,
        std::string_view yLabel,
        bool logarithmicX = false,
        bool logarithmicY = false
    );

    ~LinePlot();

    LinePlot(const LinePlot&) = delete;
    LinePlot& operator=(const LinePlot&) = delete;

    void AddPlot(
        const std::filesystem::path& relativeDirectory,
        std::string_view fileName,
        LineStyle lineStyle,
        double lineWidth,
        std::string_view legend
    );

private:
    struct Plot {
        std::filesystem::path filePath;
        LineStyle lineStyle;
        double lineWidth;
        std::string legend;
    };

    FILE* gnuplotPipe = nullptr;
    std::string plotTitle;
    std::string xAxisLabel;
    std::string yAxisLabel;
    bool logX;
    bool logY;
    std::vector<Plot> plots;

    void configurePlot() const;
    void renderPlots() const;
};