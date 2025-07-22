[![arduino-library-badge](https://www.ardu-badge.com/badge/CIE1931.svg?)](https://www.ardu-badge.com/CIE1931)

# CIE1931 LED Linear Brightness Ramp

This is an improved version of [nitz/Cie1931](https://github.com/nitz/Cie1931) that generates a lookup table for translating a PWM duty cycle scale into a mostly linear brightness output.

This library is designed for Atmega328 / Atmega168 and similar platforms. The math is all done at compile time.

## Improvements
It stores the look up table in flash memory (PROGMEM) instead of SRAM. This allows for a 16x larger table which is useful for higher PWM frequencies and longer fade intervals.

Additionally, it fixes [an error in the calculation](https://github.com/nitz/Cie1931/pull/3) and [fixes imports for Arduino IDE](https://github.com/nitz/Cie1931/issues/2#issuecomment-2902522632).

## Purpose

Does your LED brightness ramp like this as you increase your PWM duty cycle?

![sad led noises](https://github.com/nitz/Cie1931/blob/main/images/leds_uncorrected.png?raw=true)

But you'd rather them ramp up like this, smooth and pleasing to the eye?

![giggles in diode emmited light](https://github.com/nitz/Cie1931/blob/main/images/leds_corrected.png?raw=true)

Then do I have the tiny library for you!

## Usage
### NOTE:
You may need to edit your compiler options (inside platform.txt in Arduino IDE) and modify the `compiler.cpp.flags`
- change to: `-std=gnu++17`
- add: `-ftemplate-depth=10000` (or another large number)


Using it is easy. Declare the curve with the settings you want, and use it like a lookup table. Here's an Arduino example:

```cpp

#include "cie1931.hpp"

constexpr Cie1931<> cie1931;

// Optionally you may specify via template pameters:
//  - the max duty cycle value of your PWM (InputMax)
//  - the max resultant duty cycle value (OutputMax)
//  - the lookup table variable type (ResultType)
// e.g.: Cie1931<512, 1023, uint16_t> curve;
// Would specify a curve with 512 input values
// that produces a range from 0-1023, stored
// in 16 bit unsigned integers.

size_t pwmStep = 0;
constexpr int PWM_PIN = 9;

void setup()
{
	// no need to do any setup, the curve is already ready!
}

void loop()
{
	// restart our step if we go past the length
	if (pwmStep >= cie1931.Length)
	{
		pwmStep = 0;
	}
	
	// step through each value in the generated curve
	uint8_t pwmVal = cie1931[pwmStep++];

	// set our PWM DC
	analogWrite(PWM_PIN, pwmVal);
	
	// a short pause before moving to the next value
	delay(50);
}
```

## Acknowledgments & Licenses

This project is MIT licensed. See `LICENSE` for more information.

Special thanks to [nitz/Cie1931](https://github.com/nitz/Cie1931) for the original library and [Jared Sanson](https://github.com/jorticus)'s [blog post](https://jared.geek.nz/2013/feb/linear-led-pwm) for their approach to the generation of a lightness curve. Images are also via Jared.

Please feel free to open an issue or PR if you find a bug or have a suggestion!
