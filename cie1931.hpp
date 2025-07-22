#ifndef CIE1931_HPP
#define CIE1931_HPP

#include <stdint.h>
#include <stddef.h>
#include <avr/pgmspace.h>

/**
 * @class Cie1931
 * @brief Creates a compile-time CIE 1931 L* lightness lookup table (LUT) stored in PROGMEM.
 *
 * This class generates a LUT to convert linear light levels (0-InputMax) to
 * perceptually uniform brightness levels (0-OutputMax) based on the CIE 1931 formula.
 * The table is generated at compile-time and stored entirely in flash memory to save SRAM.
 *
 * @tparam InputMax The maximum input value for the linear scale (e.g., 1000 for 0-1000).
 * @tparam OutputMax The maximum output value for the brightness scale (e.g., 255 for an 8-bit value).
 * @tparam ResultType The data type for the LUT values (e.g., uint8_t, uint16_t).
 */
template <
    size_t InputMax,
    size_t OutputMax,
    typename ResultType = uint8_t>
class Cie1931
{
public:
    /**
     * @brief Provides access to the lookup table.
     * @param index The linear input value (0 to InputMax).
     * @return The corresponding perceptually uniform brightness value.
     */
    ResultType operator[](size_t index) const
    {
        // Clamp index to the valid range to prevent reading out of bounds.
        if (index > InputMax)
        {
            index = InputMax;
        }

        // Read the value from the PROGMEM table based on its size.
        if constexpr (sizeof(ResultType) == 1)
        {
            return pgm_read_byte(&table[index]);
        }
        else if constexpr (sizeof(ResultType) == 2)
        {
            return pgm_read_word(&table[index]);
        }
        else if constexpr (sizeof(ResultType) == 4)
        {
            return pgm_read_dword(&table[index]);
        }
        else
        {
            // Fallback for other types, though byte, word, and dword cover most cases.
            ResultType value;
            memcpy_P(&value, &table[index], sizeof(ResultType));
            return value;
        }
    }

    /**
     * @brief Returns the total size of the lookup table.
     * @return The number of elements in the table (InputMax + 1).
     */
    constexpr size_t size() const
    {
        return InputMax + 1;
    }

private:
    /**
     * @brief Calculates a single CIE 1931 lightness value at compile time.
     */
    static constexpr ResultType calculateValue(size_t input)
    {
        // Scale input from 0-InputMax to a 0-100 lightness value (L*).
        const double L = static_cast<double>(input) * 100.0 / InputMax;
        double result = 0.0;

        if (L <= 8.0)
        {
            result = L / 903.3;
        }
        else
        {
            const double term = (L + 16.0) / 116.0;
            result = term * term * term; // Simplified from ipow(term, 3)
        }

        // Scale result to the desired output range and round to the nearest integer.
        return static_cast<ResultType>((result * OutputMax) + 0.5);
    }

    /**
     * @brief Helper struct to generate the lookup table at compile time.
     * This uses an index sequence to initialize the PROGMEM array.
     */
    template <size_t... Is>
    struct TableHolder
    {
        static constexpr ResultType table[sizeof...(Is)] PROGMEM = {calculateValue(Is)...};
    };

    /**
     * @brief A recursive template to generate an index sequence from 0 to N.
     */
    template <size_t N, size_t... Is>
    struct MakeIndices : MakeIndices<N - 1, N - 1, Is...>
    {
    };

    template <size_t... Is>
    struct MakeIndices<0, Is...>
    {
        // When N reaches 0, use the generated index pack (Is...) to create the table.
        using type = TableHolder<Is...>;
    };

    // The actual lookup table, generated and stored in PROGMEM.
    static constexpr auto &table = MakeIndices<InputMax + 1>::type::table;
};

#endif // CIE1931_HPP
