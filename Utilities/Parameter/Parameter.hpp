#pragma once

#include <cstddef>
#include <type_traits>

// T is the data type stored by this parameter object.
template <typename T>
class Parameter
{
    // Restrict supported types to double, std::size_t, int, and unsigned int.
    static_assert(
        std::is_same_v<T, double> ||
        std::is_same_v<T, std::size_t> ||
        std::is_same_v<T, int> ||
        std::is_same_v<T, unsigned int>,
        "Parameter only supports double, std::size_t, int, and unsigned int."
    );

private:
    T ParameterValue = T{1};

public:
    // T{1} creates the default value 1 with the correct type.
    explicit Parameter()
    {}

    void SetValue(T Parameter_Value_)
    {
        ParameterValue = Parameter_Value_;
    }

    // The return type is exactly the template type T.
    [[nodiscard]] T GetValue() const noexcept
    {
        return ParameterValue;
    }
};
