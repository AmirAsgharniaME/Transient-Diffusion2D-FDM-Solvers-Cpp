#include "IO/Visualization/gnuplot/LinePlot/LinePlot.hpp"

#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <system_error>//temp
#include <iostream>

#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

namespace {
    // Escape text used inside single-quoted Gnuplot strings.
    std::string escapeGnuplotString(const std::string& text) {
        std::string escaped;
        escaped.reserve(text.size());

        for (char ch : text) {
            switch (ch) {
                case '\\':
                    escaped += "\\\\";
                    break;
                case '\'':
                    escaped += "\\'";
                    break;
                case '\n':
                case '\r':
                    escaped += ' ';
                    break;
                default:
                    escaped += ch;
                    break;
            }
        }

        return escaped;
    }

    int dashType(LinePlot::LineStyle style) {
        switch (style) {
            case LinePlot::LineStyle::Solid:
                return 1;
            case LinePlot::LineStyle::Dashed:
                return 2;
            case LinePlot::LineStyle::Dotted:
                return 3;
            case LinePlot::LineStyle::DashDotted:
                return 4;
        }

        throw std::invalid_argument("Unsupported line style.");
    }
}

LinePlot::LinePlot(
    std::string_view title,
    std::string_view xLabel,
    std::string_view yLabel,
    bool logarithmicX,
    bool logarithmicY
)
    : plotTitle(title),
      xAxisLabel(xLabel),
      yAxisLabel(yLabel),
      logX(logarithmicX),
      logY(logarithmicY) {
    gnuplotPipe = POPEN("gnuplot -persist", "w");

    if (gnuplotPipe == nullptr) {
        throw std::runtime_error("Failed to open Gnuplot.");
    }

    configurePlot();
}

LinePlot::~LinePlot() {
    if (gnuplotPipe != nullptr) {
        std::fprintf(gnuplotPipe, "exit\n");
        PCLOSE(gnuplotPipe);
        gnuplotPipe = nullptr;
    }
}

void LinePlot::configurePlot() const {
    std::fprintf(
        gnuplotPipe,
        "set terminal qt size 1200,800 enhanced font 'Sans,12'\n"
    );

    std::fprintf(
        gnuplotPipe,
        "set title '%s'\n",
        escapeGnuplotString(plotTitle).c_str()
    );

    std::fprintf(
        gnuplotPipe,
        "set xlabel '%s'\n",
        escapeGnuplotString(xAxisLabel).c_str()
    );

    std::fprintf(
        gnuplotPipe,
        "set ylabel '%s'\n",
        escapeGnuplotString(yAxisLabel).c_str()
    );

    std::fprintf(gnuplotPipe, "set grid\n");
    std::fprintf(gnuplotPipe, "set key outside right\n");
    std::fprintf(gnuplotPipe, "set autoscale\n");

    std::fprintf(
        gnuplotPipe,
        "%s logscale x\n",
        logX ? "set" : "unset"
    );

    std::fprintf(
        gnuplotPipe,
        "%s logscale y\n",
        logY ? "set" : "unset"
    );

    std::fflush(gnuplotPipe);
}

void LinePlot::AddPlot(
    const std::filesystem::path& relativeDirectory,
    std::string_view fileName,
    LineStyle lineStyle,
    double lineWidth,
    std::string_view legend
) {
    std::filesystem::path namePath{std::string(fileName)};

    if (fileName.empty() || namePath.has_parent_path()) {
        throw std::invalid_argument(
            "fileName must contain only a file name, not a directory."
        );
    }

if (namePath.extension() != ".dat") {
    namePath += ".dat";
}


    if (!std::isfinite(lineWidth) || lineWidth <= 0.0) {
        throw std::invalid_argument(
            "The line width must be a positive finite number."
        );
    }

    // Reject unsupported enum values before adding the plot.
    dashType(lineStyle);

    // const std::filesystem::path filePath =
    //     relativeDirectory / namePath;

    // if (!std::filesystem::is_regular_file(filePath)) {
    //     throw std::runtime_error(
    //         "Plot data file not found: " + filePath.string()
    //     );
    // }
    //--------------------------------------------------------
const std::filesystem::path filePath =
    relativeDirectory / namePath;

std::error_code fileError;
const bool fileExists =
    std::filesystem::exists(filePath, fileError);

std::error_code typeError;
const bool isRegularFile =
    std::filesystem::is_regular_file(filePath, typeError);

if (!isRegularFile) {
    std::cerr << "cwd: " << std::filesystem::current_path() << '\n'
              << "path: " << std::filesystem::absolute(filePath) << '\n'
              << "exists: " << (fileExists ? "true" : "false") << '\n'
              << "exists error: " << fileError.message() << '\n'
              << "is_regular_file error: " << typeError.message()
              << '\n';

    throw std::runtime_error(
        "Plot data file not found: " + filePath.string()
    );
}

//--------------------------------------------------------
    plots.push_back({
        filePath,
        lineStyle,
        lineWidth,
        std::string(legend)
    });

    renderPlots();
}

void LinePlot::renderPlots() const {
    if (plots.empty()) {
        return;
    }

    std::fprintf(gnuplotPipe, "plot ");

    for (std::size_t i = 0; i < plots.size(); ++i) {
        const Plot& plot = plots[i];

        if (i != 0) {
            std::fprintf(gnuplotPipe, ", ");
        }

        std::fprintf(
            gnuplotPipe,
            "'%s' using 1:2 with lines dashtype %d linewidth %.6g title '%s'",
            escapeGnuplotString(plot.filePath.string()).c_str(),
            dashType(plot.lineStyle),
            plot.lineWidth,
            escapeGnuplotString(plot.legend).c_str()
        );
    }

    std::fprintf(gnuplotPipe, "\n");
    std::fflush(gnuplotPipe);
}