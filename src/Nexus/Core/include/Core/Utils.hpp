#pragma once

// BIT(x)
// Produces a bitmask with bit x set.
// Example: BIT(3) → (1 << 3) → 0b00001000
#define BIT(x) (1 << x)

// STRINGIFY(x)
// Converts the literal token x into a string *without expanding it*.
// Example: STRINGIFY(VALUE) → "VALUE"
#define STRINGIFY(x) #x

// TOSTRING(x)
// Expands x first, then stringifies the expanded result.
// This is the standard two-step macro stringification pattern.
// Example:
//   #define VALUE 42
//   TOSTRING(VALUE) → "42"
#define TOSTRING(x) STRINGIFY(x)
