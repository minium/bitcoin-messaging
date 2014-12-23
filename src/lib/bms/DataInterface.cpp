/**
 * DataInterface.cpp
 *
 * Interface module for type conversions between binary vectors,
 * representing to be embedded information, and specific types
 * utilized in transactions.
 *
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include "DataInterface.h"
#include "Maths.h"
#include "Utilities.h"

#include <numeric>

using std::vector;
namespace mp = boost::multiprecision;

namespace DataInterface
{

/**
 * Converts a binary vector into an unsigned integer.
 * @param Bits A binary vector.
 * @result An unsigned integer representing the binary vector.
 */
unsigned int EncodeDataInSequenceNr(const DataBits& Bits)
{
	assert(Bits.size() == 32);

	Data data = BitsToData(Bits);
	uint seqnr = 0;

	seqnr |= data[0] << 24;
	seqnr |= data[1] << 16;
	seqnr |= data[2] << 8;
	seqnr |= data[3];

	return seqnr;
}

/**
 * Converts an unsigned integer into a binary vector.
 * @param sequenceNr An unsigned integer.
 * @result A binary vector representing the unsigned integer.
 */
DataBits DecodeDataInSequenceNr(unsigned int sequenceNr)
{
	Data tmp;

	tmp.push_back((sequenceNr >> 24) & 0xFF);
	tmp.push_back((sequenceNr >> 16) & 0xFF);
	tmp.push_back((sequenceNr >> 8) & 0xFF);
	tmp.push_back(sequenceNr & 0xFF);

	return DataToBits(tmp);
}

/**
 * Converts a binary vector into a public key.
 * @param Bits A binary vector.
 * @param nRandBits Number of random bits.
 * @result Public key representing the binary vector.
 */
CPubKey EncodeDataInPubkey(const DataBits& Data, uint8_t nRandBits)
{
	assert(1 <= Data.size());
	assert(5 <= nRandBits);
	assert(255 - Data.size() == nRandBits);

	CPubKey pk;
	DataBits prefix;
	DataBits data;
	DataBits random;

	prefix.insert(prefix.end(), 6, false);
	prefix.insert(prefix.end(), true);
	prefix.insert(prefix.end(), false);

	data.insert(data.end(), false);
	data.insert(data.end(), Data.begin(), Data.end());
	data.insert(data.end(), 256 - data.size() - nRandBits, false);

	do
	{
		random = Utilities::GenerateRandomBits(nRandBits);

		DataBits tmp;
		tmp.insert(tmp.end(), prefix.begin(), prefix.end());
		tmp.insert(tmp.end(), data.begin(), data.end());
		tmp.insert(tmp.end(), random.begin(), random.end());

		pk = CPubKey(BitsToData(tmp));
	} while (!pk.IsFullyValid());

	return pk;
}

/**
 * Converts a public key into a binary vector.
 * @param Bits A public key.
 * @param nRandBits Number of random bits.
 * @result Binary vector representing the public key.
 */
DataBits DecodeDataInPubkey(const CPubKey& Pubkey, uint8_t nRandBits)
{
	DataBits buf = DataToBits(Data(Pubkey.begin(), Pubkey.end()));

	buf.erase(buf.begin(), buf.begin() + 9);
	buf.erase(buf.end() - nRandBits, buf.end());

	return buf;
}

/**
 * Computes the number of bits that can be embedded in a combinatorial composition.
 * @param n Integer of the combinatorial composition.
 * @param k Number of parts.
 * @result Number of bits that can be embedded.
 */
uint32_t EmbeddableBitsInValues(uint64_t n, uint16_t k)
{
	BigFloat result;
	result = mp::log(BigFloat(Math::NumberCompositions(n, k)));
	result /= mp::log(BigFloat(2));

	return (uint32_t) result;
}


/**
 * Converts a binary vector into a combinatorial composition of the budget.
 * @param Data A binary vector.
 * @param budget Integer of the combinatorial composition.
 * @param nParts Number of parts.
 * @result Combinatorial composition of the budget representing the binary vector.
 */
vector<uint64_t> EncodeDataInValues(const DataBits& Data, uint64_t budget, uint16_t nParts)
{
	const uint32_t MaxBits = EmbeddableBitsInValues(budget, nParts);

	assert(Data.size() == MaxBits);
	assert(nParts >= 2);

	BigInt idx = DataBitsToInt(Data);
	vector<uint64_t> composition = Math::IntegerToComposition(idx, budget, nParts);

	return composition;
}

/**
 * Converts a combinatorial composition of the budget into a binary vector.
 * @param Values The combinatorial composition.
 * @result Binary vector representing the combinatorial composition of the budget.
 */
DataBits DecodeDataInValues(const vector<uint64_t>& Values)
{
	const uint64_t Budget = std::accumulate(Values.begin(), Values.end(), (uint64_t) 0);
	const uint16_t nParts = Values.size();

	assert(nParts >= 2);

	const uint32_t MaxBits = EmbeddableBitsInValues(Budget, nParts);

	BigInt idx = Math::CompositionToInteger(Values);

	DataBits data = IntToDataBits(idx);
	data.insert(data.begin(), MaxBits - data.size(), false);

	return data;
}

/**
 * Computes the number of bits that can be embedded in a permutation.
 * @param nParts Number of elements in the permutation.
 * @result Number of bits that can be embedded.
 */
uint32_t EmbeddableBitsInPermutation(uint16_t nParts)
{
	BigFloat result;
	result = mp::log(BigFloat(Math::Factorial(nParts)));
	result /= mp::log(BigFloat(2));

	return (uint32_t) result;
}


/**
 * Converts a binary vector into a permutation with a specified number of elements.
 * @param Data A binary vector.
 * @param nParts Number of elements in the permutation.
 * @result Permutation representing the binary vector.
 */
vector<uint16_t> EncodeDataInPermutation(const DataBits& Data, uint16_t nParts)
{
	const uint32_t MaxBits = EmbeddableBitsInPermutation(nParts);

	assert(nParts >= 2);
	assert(Data.size() == MaxBits);

	BigInt idx = DataBitsToInt(Data);
	vector<uint16_t> perm = Math::IntegerToPermutation(idx, nParts);

	return perm;
}

/**
 * Converts a permutation into a binary vector.
 * @param Permutation The permutation.
 * @result Binary vector representing the permutation.
 */
DataBits DecodeDataInPermutation(const vector<uint16_t>& Permutation)
{
	const uint16_t Size = Permutation.size();
	const uint32_t MaxBits = EmbeddableBitsInPermutation(Size);

	assert(Size >= 2);

	BigInt idx = Math::PermutationToInteger(Permutation);
	DataBits data = IntToDataBits(idx);

	data.insert(data.begin(), MaxBits - data.size(), false);

	return data;
}

}
