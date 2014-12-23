/**
 * DataInterface.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "Utilities.h"
#include "DataInterface.h"

using namespace DataInterface;


BOOST_AUTO_TEST_SUITE(DataInterfaceTests)

BOOST_AUTO_TEST_CASE(DataEncoding_Pubkey)
{
	for (unsigned int i = 0; i < 1000; i++)
	{
		DataBits originalData = Utilities::GenerateRandomBits(1+(i % 250));
		DataBits recoveredData;

		CPubKey pk = EncodeDataInPubkey(originalData, 255 - (1 + i % 250));
		recoveredData = DecodeDataInPubkey(pk, 255 - (1 + i % 250));

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_CASE(DataEncoding_SeqNr)
{
	for (unsigned int i = 0; i < 1000; i++)
	{
		DataBits originalData = Utilities::GenerateRandomBits(32);
		DataBits recoveredData;

		unsigned int seqnr = EncodeDataInSequenceNr(originalData);
		recoveredData = DecodeDataInSequenceNr(seqnr);

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_CASE(EmbeddableBits_TxAmounts)
{
	uint32_t nBits1 = EmbeddableBitsInValues(pow(10,8), 20);
	uint32_t nBits2 = EmbeddableBitsInValues(pow(10,15), 39);

	BOOST_REQUIRE(nBits1 == 448);
	BOOST_REQUIRE(nBits2 == 1744);
}

BOOST_AUTO_TEST_CASE(DataEncoding_TxAmounts)
{
	for (unsigned int i = 0; i < 1000; i++)
	{
		uint32_t MaxBits = EmbeddableBitsInValues(pow(10,3+(i%13)), 2+(i%29));

		DataBits originalData = Utilities::GenerateRandomBits(MaxBits);
		DataBits recoveredData;

		std::vector<uint64_t> vals = EncodeDataInValues(originalData, pow(10,3+(i%13)), 2+(i%29));
		recoveredData = DecodeDataInValues(vals);

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_CASE(EmbeddableBits_TxPermutation)
{
	uint32_t nBits1 = EmbeddableBitsInPermutation(15);
	uint32_t nBits2 = EmbeddableBitsInPermutation(90);

	BOOST_REQUIRE(nBits1 == 40);
	BOOST_REQUIRE(nBits2 == 458);
}

BOOST_AUTO_TEST_CASE(DataEncoding_TxPermutations)
{
	for (unsigned int i = 0; i < 1000; i++)
	{
		uint32_t MaxBits = EmbeddableBitsInPermutation(2+(i % 39));

		DataBits originalData = Utilities::GenerateRandomBits(MaxBits);
		DataBits recoveredData;

		std::vector<uint16_t> perm = EncodeDataInPermutation(originalData, 2+(i % 39));
		recoveredData = DecodeDataInPermutation(perm);

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_SUITE_END()

