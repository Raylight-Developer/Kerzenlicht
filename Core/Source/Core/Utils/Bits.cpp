#include "Core/Utils/Bits.hpp"

// DECL
uint32 packTwoUint16(const uint16& low, const uint16& high) {
	return (static_cast<uint32>(high) << 16) | low;
}