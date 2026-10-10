#include "LivePointPlotter.hpp"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>

#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

namespace {

std::string EscapeGnuplotString(const std::string& text)
{
    std::string escaped;
    escaped.reserve(text.size());

    for (char character : text) {
        if (character == '\\' || character == '\'') {
            escaped += '\\';
        }
        escaped += character;
    }

    return escaped;
}

} // namespace

LivePointPlotter::LivePointPlotter(
    std::string plotTitle,
    std::string xLabel,
    std::string yLabel,
    bool logarithmicY)
    : plotTitle_(std::move(plotTitle)),
      xLabel_(std::move(xLabel)),
      yLabel_(std::move(yLabel)),
      logarithmicY_(logarithmicY)
{
}

LivePointPlotter::~LivePointPlotter()
{
    Close();
}

bool LivePointPlotter::AddSeries(
    const std::string& seriesId,
    const std::string& legend)
{
    if (seriesId.empty()) {
        std::cerr << "Error: Series ID cannot be empty.\n";
        return false;
    }

    for (const Series& series : series_) {
        if (series.id == seriesId) {
            std::cerr << "Error: Series ID already exists: "
                      << seriesId << '\n';
            return false;
        }
    }

    series_.push_back({
        seriesId,
        legend.empty() ? seriesId : legend,
        {}
    });

    return true;
}

bool LivePointPlotter::AddPoint(
    const std::string& seriesId,
    double iteration,
    double value)
{
    for (Series& series : series_) {
        if (series.id == seriesId) {
            if (!std::isfinite(iteration) ||
                !std::isfinite(value) ||
                (logarithmicY_ && value <= 0.0)) {
                std::cerr << "Error: Invalid point for series: "
                          << seriesId << '\n';
                return false;
            }

            series.points.push_back({iteration, value});
            return UpdatePlot();
        }
    }

    std::cerr << "Error: Unknown series ID: "
              << seriesId << '\n';
    return false;
}

bool LivePointPlotter::AddPoints(
    double iteration,
    const std::map<std::string, double>& values)
{
    if (!std::isfinite(iteration)) {
        std::cerr << "Error: Iteration must be finite.\n";
        return false;
    }

    // ابتدا همهٔ ورودی‌ها اعتبارسنجی می‌شوند تا افزودن داده نیمه‌کاره نماند.
    for (const auto& [seriesId, value] : values) {
        bool exists = false;

        for (const Series& series : series_) {
            if (series.id == seriesId) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            std::cerr << "Error: Unknown series ID: "
                      << seriesId << '\n';
            return false;
        }

        if (!std::isfinite(value) ||
            (logarithmicY_ && value <= 0.0)) {
            std::cerr << "Error: Invalid point for series: "
                      << seriesId << '\n';
            return false;
        }
    }

    for (Series& series : series_) {
        const auto it = values.find(series.id);

        if (it != values.end()) {
            series.points.push_back({iteration, it->second});
        }
    }

    return UpdatePlot();
}

bool LivePointPlotter::Initialize()
{
    if (gnuplotPipe_ != nullptr) {
        return true;
    }

    gnuplotPipe_ = POPEN("gnuplot -persist", "w");

    if (gnuplotPipe_ == nullptr) {
        std::cerr << "Error: Could not start gnuplot. "
                     "Ensure it is installed and available in PATH.\n";
        return false;
    }

    const std::string title = EscapeGnuplotString(plotTitle_);
    const std::string xLabel = EscapeGnuplotString(xLabel_);
    const std::string yLabel = EscapeGnuplotString(yLabel_);

    bool ok =
        std::fprintf(gnuplotPipe_, "set title '%s'\n", title.c_str()) >= 0 &&
        std::fprintf(gnuplotPipe_, "set xlabel '%s'\n", xLabel.c_str()) >= 0 &&
        std::fprintf(gnuplotPipe_, "set ylabel '%s'\n", yLabel.c_str()) >= 0 &&
        std::fprintf(gnuplotPipe_, "set grid\n") >= 0 &&
        std::fprintf(gnuplotPipe_, "set key outside right\n") >= 0;

    if (ok && logarithmicY_) {
        ok = std::fprintf(gnuplotPipe_, "set logscale y\n") >= 0;
    }

    if (ok) {
        ok = std::fflush(gnuplotPipe_) == 0;
    }

    if (!ok) {
        std::cerr << "Error: Failed to initialize gnuplot.\n";
        Close();
    }

    return ok;
}

bool LivePointPlotter::UpdatePlot()
{
    if (series_.empty()) {
        return true;
    }

    if (!Initialize()) {
        return false;
    }

    bool ok = std::fprintf(gnuplotPipe_, "plot ") >= 0;

    for (std::size_t i = 0; ok && i < series_.size(); ++i) {
        const std::string legend =
            EscapeGnuplotString(series_[i].legend);

        ok = std::fprintf(
                 gnuplotPipe_,
                 "%s'-' using 1:2 with lines lw 2 title '%s'",
                 i == 0 ? "" : ", ",
                 legend.c_str()) >= 0;
    }

    if (ok) {
        ok = std::fprintf(gnuplotPipe_, "\n") >= 0;
    }

    // به‌ازای هر '-' در دستور plot، یک مجموعه داده و پایان‌دهندهٔ e لازم است.
    for (const Series& series : series_) {
        if (!ok) {
            break;
        }

        for (const Point& point : series.points) {
            ok = std::fprintf(
                     gnuplotPipe_,
                     "%.17g %.17g\n",
                     point.x,
                     point.y) >= 0;

            if (!ok) {
                break;
            }
        }

        if (ok) {
            ok = std::fprintf(gnuplotPipe_, "e\n") >= 0;
        }
    }

    if (ok) {
        ok = std::fflush(gnuplotPipe_) == 0;
    }

    if (!ok) {
        std::cerr << "Error: Failed to update gnuplot.\n";
        Close();
    }

    return ok;
}

void LivePointPlotter::Close() noexcept
{
    if (gnuplotPipe_ != nullptr) {
        PCLOSE(gnuplotPipe_);
        gnuplotPipe_ = nullptr;
    }
}
