/**
 * Math.cpp
 *
 * Standard and custom mathematical functions with
 * arbitrary integer precision.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include "Maths.h"
#include <numeric>

namespace mp = boost::multiprecision;


namespace Math
{

/**
 * Computes the factorial for a given integer \p n.
 * @param n Integer of which the factorial is to be computed.
 * @return Factorial of the given integer.
 */
BigInt Factorial(uint16_t n)
{
	BigInt fac(1);

	for (int i = 2; i <= n; i++)
		fac *= i;

	return fac;
}

/**
 * Computes the binomial coefficient n-choose-k.
 * @param n First parameter of the binomial coefficient.
 * @param k Second parameter of the binomial coefficient.
 * @return Resulting binomial coefficient.
 */
BigInt BinomialCoefficient(uint64_t n, uint64_t k)
{
	if (k > n)
		return 0;

	BigInt result(1);
	k = std::min(k, n - k);

	for (unsigned i = 1; i < k + 1; ++i)
	{
		result *= BigInt(n - k + i);
		result /= BigInt(i);
	}

	return result;
}

/**
 * Computes the number of combinatorial compositions of an integer \p n into \p k parts.
 * @param n Integer to be split.
 * @param k Number of parts.
 * @return Number of compositions.
 */
BigInt NumberCompositions(uint64_t n, uint16_t k)
{
	return BinomialCoefficient(n + k - 1, k - 1);
}

/**
 * Computes the lexicographic index for a given permutation.
 * @param Permutation The permutation.
 * @return Lexicographic index of the permutation.
 */
BigInt PermutationToInteger(const vector<uint16_t>& Permutation)
{
	const unsigned int PermSize = Permutation.size();
	BigInt idx = 0;

	for (unsigned int i = 0; i <= PermSize - 2; i++)
	{
		int a = 0;
		for (unsigned int j = i + 1; j <= PermSize - 1; j++)
		{
			if (Permutation[i] > Permutation[j])
				a++;
		}

		idx += a * Factorial(PermSize - i - 1);
	}

	return idx;
}

/**
 * Computes the permutation from its lexicographic index and number of elements.
 * @param idx Lexicographic index of the permutation.
 * @param k Number of elements in the permutation.
 * @return The resulting permutation.
 */
vector<uint16_t> IntegerToPermutation(BigInt idx, uint16_t k)
{
	vector<uint16_t> perm(k);
	vector<uint16_t> buf(k);
	std::iota(buf.begin(), buf.end(), 0);

	BigInt x = 0;
	for (int i = 0; i <= k - 1; i++)
	{
		x = (idx / Factorial(k - i - 1));
		perm[i] = buf[x.convert_to<int>()];

		idx -= x * Factorial(k - i - 1);
		buf.erase(std::remove(buf.begin(), buf.end(), perm[i]), buf.end());
	}

	return perm;
}

/**
 * Computes the lexicographic index for a given combinatorial composition.
 * @param Composition Compositions of which the index is to be computed.
 * @return Lexicographic index of the composition.
 */
BigInt CompositionToInteger(const vector<uint64_t>& Composition)
{
	const uint64_t N = std::accumulate(Composition.begin(), Composition.end(), (uint64_t) 0);
	const uint16_t K = Composition.size();

	BigInt idx = 0;
	uint64_t n = N;
	uint16_t k = K;

	for (uint16_t i = 0; i <= K - 2; i++)
	{
		idx += NumberCompositions(n, k) - NumberCompositions(n - Composition[i], k);

		k--;
		n -= Composition[i];
	}

	return idx;
}

/**
 * Computes the combinatorial composition from its lexicographic index,
 * the integer \p n and its number of elements \p k.
 * @param n The integer of the composition.
 * @param k Number of composition parts.
 * @return The resulting composition.
 */
vector<uint64_t> IntegerToComposition(BigInt idx, uint64_t n, uint16_t k)
{
	const uint16_t K = k;
	vector<uint64_t> composition(K);

	BigInt A;
	BigFloat root;
	uint64_t mu;

	for (int i = 0; i <= K - 2; i++)
	{
		if (n == 0)
			break;

		A = (NumberCompositions(n, k) - idx) * Factorial(k - 1);
		root = mp::round(mp::pow(BigFloat(A), BigFloat(1.0 / (k - 1))));
		mu = n - root.convert_to<uint64_t>() - (k - 1) / 2;

		while (!(NumberCompositions(n - mu - 1, k)
				< NumberCompositions(n, k) - idx))
			mu++;
		while (!(NumberCompositions(n, k) - idx <= NumberCompositions(n - mu, k)))
			mu--;

		composition[i] = mu;

		idx -= (NumberCompositions(n, k) - NumberCompositions(n - mu, k));
		n -= mu;
		k -= 1;
	}

	composition[K - 1] = n;

	return composition;
}

}
