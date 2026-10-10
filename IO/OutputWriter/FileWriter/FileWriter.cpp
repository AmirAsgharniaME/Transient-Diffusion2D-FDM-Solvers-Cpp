#include "Core/FileWriter/FileWriter.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <stdexcept>

namespace FileWriter
{
void WriteField(
    std::string_view FileName_,
    const Field& Field_Obj,
    const Mesh& Mesh_Obj,
    const std::filesystem::path& RelativePath)
{
    if (Field_Obj.Get_Nx() != Mesh_Obj.Get_Nx() ||
        Field_Obj.Get_Ny() != Mesh_Obj.Get_Ny())
    {
        throw std::invalid_argument("FileWriter: field dimensions must match the mesh.");
    }

    std::filesystem::path fileName(FileName_.begin(), FileName_.end());
    if (FileName_.empty() || fileName.has_parent_path() || fileName == "." || fileName == "..")
    {
        throw std::invalid_argument("FileWriter: supply a filename and a separate directory.");
    }
    if (fileName.extension() != ".dat")
    {
        fileName += ".dat";
    }

    // Validate before truncating an existing file. Contour accepts only finite data.
    const auto isFinite = [](double value) { return std::isfinite(value); };
    if (!std::all_of(Mesh_Obj.Get_XGrid().begin(), Mesh_Obj.Get_XGrid().end(), isFinite) ||
        !std::all_of(Mesh_Obj.Get_YGrid().begin(), Mesh_Obj.Get_YGrid().end(), isFinite) ||
        !std::all_of(Field_Obj.GetField().begin(), Field_Obj.GetField().end(), isFinite))
    {
        throw std::invalid_argument("FileWriter: coordinates and field values must be finite.");
    }

    if (!RelativePath.empty())
    {
        std::filesystem::create_directories(RelativePath);
    }
    const auto fullFilePath = RelativePath / fileName;
    std::ofstream file;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    file.open(fullFilePath);
    file.imbue(std::locale::classic());
    // Preserve distinct coordinates on tiny meshes and round-trip double values.
    file << std::scientific << std::setprecision(std::numeric_limits<double>::max_digits10);

    for (std::size_t j = 0; j < Mesh_Obj.Get_Ny(); ++j)
    {
        for (std::size_t i = 0; i < Mesh_Obj.Get_Nx(); ++i)
        {
            file << Mesh_Obj.X(i) << ' '
                 << Mesh_Obj.Y(j) << ' '
                 << Field_Obj(j, i) << '\n';
        }
        if (j + 1 < Mesh_Obj.Get_Ny())
        {
            file << '\n';
        }
    }
    file.close(); // Report buffered write/close errors before returning.
}
}
