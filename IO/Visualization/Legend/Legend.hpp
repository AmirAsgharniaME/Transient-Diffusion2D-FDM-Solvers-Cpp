#pragma once

#include <string>
#include <sstream>
#include <string_view>
#include "Config/FieldName.hpp"
#include "Config/Label.hpp"

namespace Legend
{

// ============================================================================
// Gnuplot Plot Legend Generators (e.g., "U (t = 0.003 s)", "U (Steady_State)")
// ============================================================================

// Overload 3: Generates clean Gnuplot legend with physical units (e.g., "U (t = 0.003 s)")
[[nodiscard]] inline std::string Create(const FieldName FieldName_, const double Time_)
{
    std::ostringstream stream;
    stream << To_String_View(FieldName_) << " (t = " << Time_ << " s)";
    return stream.str();
}

// Overload 4: Generates clean Gnuplot legend with strongly typed label (e.g., "U (Steady_State)")
[[nodiscard]] inline std::string Create(const FieldName FieldName_, const Label Label_)
{
    const std::string_view fieldStr = To_String_View(FieldName_);
    const std::string_view labelStr = To_String_View(Label_);

    std::string result;
    // Length breakdown: fieldStr + " (" (2 bytes) + labelStr + ")" (1 byte) -> Total extra: 3 bytes
    result.reserve(fieldStr.size() + 2 + labelStr.size() + 1);

    result += fieldStr;
    result += " (";
    result += labelStr;
    result += ')';

    return result;
}

}