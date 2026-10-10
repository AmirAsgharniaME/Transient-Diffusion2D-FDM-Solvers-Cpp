#pragma once

#include "Core/Field/Field.hpp"
#include "Core/Mesh/Mesh.hpp"
#include <filesystem>
#include <string_view>

namespace FileWriter
{
// Writes every node as x y value, with a blank line between successive y rows.
// FileName_ accepts a filename with or without .dat; the directory is created.
// RelativePath is resolved from the working directory. Run from the project
// root to share relative paths with Contour::AddContour.
// Invalid input and filesystem/write errors are reported through exceptions.
void WriteField(
        std::string_view FileName_,
        const Field& Field_Obj,
        const Mesh& Mesh_Obj,
        const std::filesystem::path& RelativePath);

}

