#include "Core/PostProcessing/Contour.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace
{
std::filesystem::path FindProjectRoot(std::filesystem::path directory)
{
    while (!directory.empty())
    {
        if (std::filesystem::is_directory(directory / "Core") &&
            std::filesystem::is_directory(directory / "Config"))
        {
            return directory;
        }
        const auto parent = directory.parent_path();
        if (parent == directory)
        {
            break;
        }
        directory = parent;
    }
    return {};
}
}

void Contour::PipeCloser::operator()(FILE* pipe) const noexcept
{
    if (pipe != nullptr)
    {
        pclose(pipe); // EOF ends gnuplot; -persist keeps the plotted window visible.
    }
}

Contour::Contour(std::string_view title)
    : Contour([title] {
          WindowOptions options;
          options.Title.assign(title.begin(), title.end());
          return options;
      }())
{}

Contour::Contour(const WindowOptions& options)
    : Options(options), ProjectRoot(ResolveProjectRoot(options.ProjectRoot))
{
    if (Options.Width <= 0 || Options.Height <= 0)
    {
        throw std::invalid_argument("Window width and height must be positive.");
    }
    if (Options.ColorRange &&
        (!std::isfinite(Options.ColorRange->first) ||
         !std::isfinite(Options.ColorRange->second) ||
         Options.ColorRange->first >= Options.ColorRange->second))
    {
        throw std::invalid_argument("ColorRange must contain finite, increasing limits.");
    }

    // Validate labels before starting the process.
    std::ostringstream commands;
    commands.imbue(std::locale::classic());
    commands << "set encoding utf8\n"
             << "set terminal qt 0 size " << Options.Width << ',' << Options.Height
             << " title " << Quote(Options.Title) << " noenhanced\n"
             << "set xlabel " << Quote(Options.XLabel) << '\n'
             << "set ylabel " << Quote(Options.YLabel) << '\n'
             << "set cblabel " << Quote(Options.ColorLabel) << '\n'
             << "set palette defined (0 'blue', 1 'cyan', 2 'yellow', 3 'red')\n"
             << "set view map\nset pm3d map\nset size ratio -1\nunset key\n";

    GnuplotPipe.reset(popen("gnuplot -persist", "w"));
    if (!GnuplotPipe)
    {
        throw std::runtime_error("Failed to start gnuplot.");
    }
    SendCommands(commands.str());
}

Contour::~Contour() = default;

std::string Contour::Quote(std::string_view text)
{
    std::string quoted = "'";
    for (unsigned char character : text)
    {
        if (character < 32 || character == 127)
        {
            throw std::invalid_argument("Plot labels must not contain control characters.");
        }
        quoted += static_cast<char>(character);
        if (character == '\'')
        {
            quoted += '\''; // Gnuplot escapes single quotes by doubling them.
        }
    }
    return quoted + '\'';
}

std::filesystem::path Contour::ResolveProjectRoot(const std::filesystem::path& root)
{
    if (!root.empty())
    {
        const auto resolved = std::filesystem::canonical(root);
        if (!std::filesystem::is_directory(resolved))
        {
            throw std::invalid_argument("ProjectRoot must be a directory.");
        }
        return resolved;
    }
    auto found = FindProjectRoot(std::filesystem::current_path());
    if (found.empty() && std::filesystem::path(__FILE__).is_absolute())
    {
        found = FindProjectRoot(std::filesystem::path(__FILE__).parent_path());
    }
    if (found.empty())
    {
        throw std::invalid_argument("Cannot locate the project root; set WindowOptions::ProjectRoot.");
    }
    return std::filesystem::canonical(found);
}

Contour::ContourData Contour::ReadContour(const std::filesystem::path& file,
                                          std::string_view legend)
{
    std::ifstream input(file);
    if (!input)
    {
        throw std::runtime_error("Cannot open contour data file: " + file.string());
    }
    ContourData data;
    data.Legend.assign(legend.begin(), legend.end());
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line))
    {
        ++lineNumber;
        const auto comment = line.find('#');
        if (comment != std::string::npos)
        {
            line.erase(comment);
        }
        std::istringstream row(line);
        row.imbue(std::locale::classic());
        row >> std::ws;
        if (row.eof())
        {
            continue;
        }
        Point point{};
        std::string extra;
        if (!(row >> point.X >> point.Y >> point.Value) || (row >> extra) ||
            !std::isfinite(point.X) || !std::isfinite(point.Y) || !std::isfinite(point.Value))
        {
            throw std::invalid_argument("Expected three finite columns (x y value) in " +
                                        file.string() + " at line " + std::to_string(lineNumber));
        }
        data.Points.push_back(point);
    }
    if (input.bad())
    {
        throw std::runtime_error("Failed to read contour data file: " + file.string());
    }
    std::sort(data.Points.begin(), data.Points.end(), [](const Point& first, const Point& second) {
        return first.Y < second.Y || (first.Y == second.Y && first.X < second.X);
    });
    if (data.Points.empty())
    {
        throw std::invalid_argument("Contour data file is empty: " + file.string());
    }
    while (data.XNodes < data.Points.size() &&
           data.Points[data.XNodes].Y == data.Points.front().Y)
    {
        ++data.XNodes;
    }
    if (data.XNodes < 2 || data.Points.size() % data.XNodes != 0 ||
        data.Points.size() / data.XNodes < 2)
    {
        throw std::invalid_argument("Contour data must form a complete rectangular grid of at least 2 by 2 nodes.");
    }
    for (std::size_t k = 0; k < data.Points.size(); ++k)
    {
        const auto column = k % data.XNodes;
        const auto rowStart = k - column;
        if (data.Points[k].X != data.Points[column].X ||
            data.Points[k].Y != data.Points[rowStart].Y ||
            (column != 0 && data.Points[k].X <= data.Points[k - 1].X))
        {
            throw std::invalid_argument("Contour grid contains missing, duplicate, or inconsistent coordinates.");
        }
    }
    return data;
}

void Contour::AddContour(const std::filesystem::path& relativeDirectory,
                         std::string_view fileName,
                         std::string_view legend)
{
    std::filesystem::path name(fileName.begin(), fileName.end());
    if (relativeDirectory.is_absolute() || fileName.empty() || name.has_parent_path() ||
        name == "." || name == "..")
    {
        throw std::invalid_argument("Use a project-relative directory and a separate filename.");
    }
    if (name.extension() != ".dat")
    {
        name += ".dat";
    }
    const auto file = std::filesystem::weakly_canonical(ProjectRoot / relativeDirectory / name);
    const auto relative = file.lexically_relative(ProjectRoot);
    if (relative.empty() || *relative.begin() == "..")
    {
        throw std::invalid_argument("Contour data file must be inside ProjectRoot.");
    }
    (void)Quote(legend);
    auto data = ReadContour(file, legend);
    Contours.push_back(std::move(data));
    try
    {
        SendCommands(BuildPlotCommands());
    }
    catch (...)
    {
        Contours.pop_back();
        throw;
    }
}

std::string Contour::BuildPlotCommands() const
{
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = minX;
    double maxY = maxX;
    double minValue = minX;
    double maxValue = maxX;
    std::ostringstream commands;
    commands.imbue(std::locale::classic());
    commands << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (std::size_t index = 0; index < Contours.size(); ++index)
    {
        const auto& data = Contours[index];
        commands << "$Contour" << index << " << EOD\n";
        for (std::size_t k = 0; k < data.Points.size(); ++k)
        {
            const auto& point = data.Points[k];
            if (k != 0 && k % data.XNodes == 0)
            {
                commands << '\n'; // Separate y rows for pm3d, regardless of input order.
            }
            commands << point.X << ' ' << point.Y << ' ' << point.Value << '\n';
            minX = std::min(minX, point.X);
            maxX = std::max(maxX, point.X);
            minY = std::min(minY, point.Y);
            maxY = std::max(maxY, point.Y);
            minValue = std::min(minValue, point.Value);
            maxValue = std::max(maxValue, point.Value);
        }
        commands << "EOD\n";
    }
    if (Options.ColorRange)
    {
        minValue = Options.ColorRange->first;
        maxValue = Options.ColorRange->second;
    }
    else if (minValue == maxValue)
    {
        const double padding = std::max(1.0, std::abs(minValue) * 0.01);
        minValue -= padding;
        maxValue += padding;
    }
    const auto columns = Options.Columns == 0
        ? static_cast<std::size_t>(std::ceil(std::sqrt(static_cast<double>(Contours.size()))))
        : std::min(Options.Columns, Contours.size());
    const auto rows = 1 + (Contours.size() - 1) / columns;
    commands << "set xrange [" << minX << ':' << maxX << "]\n"
             << "set yrange [" << minY << ':' << maxY << "]\n"
             << "set cbrange [" << minValue << ':' << maxValue << "]\n"
             << "set multiplot layout " << rows << ',' << columns
             << " title " << Quote(Options.Title) << '\n';
    for (std::size_t index = 0; index < Contours.size(); ++index)
    {
        commands << "set title " << Quote(Contours[index].Legend) << '\n'
                 << "splot $Contour" << index << " using 1:2:3 with pm3d notitle\n";
    }
    commands << "unset multiplot\n";
    return commands.str();
}

void Contour::SendCommands(const std::string& commands)
{
    if (std::fwrite(commands.data(), 1, commands.size(), GnuplotPipe.get()) != commands.size() ||
        std::fflush(GnuplotPipe.get()) != 0)
    {
        throw std::runtime_error("Failed to send plot commands to gnuplot.");
    }
}
