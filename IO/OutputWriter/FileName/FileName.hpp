#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include "Config/FieldName.hpp"
#include "Config/Label.hpp"

namespace FileName
{


// ============================================================================
// File Name Generators (e.g., "T_0.003", "U_Initial")
// ============================================================================

// Overload 1: Generates file name with numerical timestamp (e.g., "T_0.003")
[[nodiscard]] inline std::string Create(const FieldName FieldName_, const double Time_)
{
    std::ostringstream stream;
    stream << To_String_View(FieldName_) << '_' << Time_;
    return stream.str();
}


// Overload 2: Generates file name with descriptive string label (e.g., "U_Initial", "U_Analytical")
[[nodiscard]] inline std::string Create(const FieldName FieldName_, const Label Label_)
{
    const std::string_view fieldStr = To_String_View(FieldName_);
    const std::string_view labelStr = To_String_View(Label_);

    std::string result;
    result.reserve(fieldStr.size() + 1 + labelStr.size()); // field + '_' + label

    result += fieldStr;
    result += '_';
    result += labelStr;

    return result;
}

}