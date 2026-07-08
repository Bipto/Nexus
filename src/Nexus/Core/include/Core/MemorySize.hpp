#pragma once

#include <compare>

namespace Nexus
{
    /// @brief An enum class representing the units of a series of bytes
    enum class MemoryUnit
    {
        /// @brief The size is stored in the smallest address unit of memory
        Bytes,

        /// @brief The size of the data is a multiple of 1024 bytes
        Kilobytes,

        /// @brief The size of the data is a multiple of 1000 bytes
        Kibibytes,

        /// @brief The size of the data is a multiple of 1024 kilobytes
        Megabytes,

        /// @brief The size of the data is a multiple of 1000 kibibytes
        Mibibytes,

        /// @brief The size of the data is a multiple of 1024 megabytes
        Gigabytes,

        /// @brief The size of the data is a multiple of 1000 mibibytes
        Gibibytes,

        /// @brief The size of the data is a multiple of 1024 gigabyes
        Terabytes,

        /// @brief The size of the data is a multiple of 1000 gibibytes
        Tibibytes,

        /// @brief The size of the data is a multiple of 1024 terabytes
        Petabytes,

        /// @brief The size of the data is a multiple of 1000 tibibytes
        Pebibytes,
    };

    /// @brief A utility class for converting between different memory sizes
    class MemorySizeConverter
    {
      public:
        /// @brief A static method  that converts between 2 different memory sizes
        /// @param value The size of the memory to represent
        /// @param from The unit that the size is represented as
        /// @param to The unit that the size should be represented as
        /// @return
        static double Convert(double value, MemoryUnit from, MemoryUnit to);
    };

    /// @brief A class that stores a size in bytes and allows converting to/from
    /// different units
    class MemorySize
    {
      public:
        /// @brief A constructor that creates a MemorySize with a given size and unit
        /// @param value The value that should be represented, will be converted into
        /// bytes internally
        /// @param unit The unit that the size in represented in, used to convert to
        /// bytes internally
        explicit MemorySize(double value, MemoryUnit unit);

        /// @brief A default constructor creating a MemorySize with a size of 0
        explicit MemorySize() = default;

        /// @brief A method	that converts from this size into the specified units
        /// @param unit The unit to convert into
        /// @return The size in bytes, converted into the requested units
        double To(MemoryUnit unit) const;

        /// @brief An operator that allows adding two MemorySize's together
        /// @param a The first MemorySize to use in the addition operation
        /// @param b The second MemorySize to use in the addition operation
        /// @return A new MemorySize object containing the result of the addition
        friend MemorySize operator+(const MemorySize &a, const MemorySize &b);

        /// @brief An operator that allows subtracting two MemorySize's together
        /// @param a The first MemorySize to use in the subtraction operation
        /// @param b The second MemorySize to use in the subtraction operation
        /// @return A new MemorySize object containing the result of the subtraction
        friend MemorySize operator-(const MemorySize &a, const MemorySize &b);

        /// @brief An operator that implements all 5 comparison operators for this
        /// object
        /// @param The MemorySize to compare this instance against
        /// @return A value indicating whether the comparison was successful
        auto operator<=>(const MemorySize &) const = default;

      private:
        /// @brief A member that stores the size of the memory as bytes
        double m_Bytes = 0;
    };
} // namespace Nexus