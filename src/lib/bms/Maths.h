/**
 * Math.h
 *
 * Standard and custom mathematical functions with
 * arbitrary integer precision.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_MATH_H
#define BMS_MATH_H

#include "Types.h"

namespace Math
{
	BigInt Factorial(uint16_t n);
	BigInt BinomialCoefficient(uint64_t n, uint64_t k);
	BigInt NumberCompositions(uint64_t n, uint16_t k);

	BigInt PermutationToInteger(const std::vector<uint16_t>& Permutation);
	std::vector<uint16_t> IntegerToPermutation(BigInt idx, uint16_t k);

	BigInt CompositionToInteger(const std::vector<uint64_t>& Composition);
	std::vector<uint64_t> IntegerToComposition(BigInt idx, uint64_t n, uint16_t k);
}

#endif
