/**
 * DataInterface.h
 *
 * Interface module for type conversions between binary vectors,
 * representing to be embedded information, and specific types
 * utilized in transactions.
 *
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_DATAINTERFACE_H
#define BMS_DATAINTERFACE_H

#include "Types.h"

namespace DataInterface
{
	unsigned int EncodeDataInSequenceNr(const DataBits& Data);
	DataBits DecodeDataInSequenceNr(unsigned int sequenceNr);

	CPubKey EncodeDataInPubkey(const DataBits& Data, uint8_t nRandBits);
	DataBits DecodeDataInPubkey(const CPubKey& Pubkey, uint8_t nRandBits);

	uint32_t EmbeddableBitsInValues(uint64_t n, uint16_t k);
	std::vector<uint64_t> EncodeDataInValues(const DataBits& Data, uint64_t budget, uint16_t nParts);
	DataBits DecodeDataInValues(const std::vector<uint64_t>& Values);

	uint32_t EmbeddableBitsInPermutation(uint16_t nParts);
	std::vector<uint16_t> EncodeDataInPermutation(const DataBits& Data, uint16_t nParts);
	DataBits DecodeDataInPermutation(const std::vector<uint16_t>& Permutation);
};

#endif
