/**
 * DataCompression.h
 *
 * Module with auxiliary functions for the Huffman coding scheme.
 * It comprises a function to compute the Huffman coding itself as
 * well as a compression and decompression function.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_DATACOMPRESSION_H
#define BMS_DATACOMPRESSION_H

const char EoF = (char)0x03;

#include "Types.h"

#include <vector>
#include <map>
#include <string>
#include <boost/bimap.hpp>

typedef std::map<char,int> FreqMap;
typedef std::vector<bool> HuffCode;
typedef boost::bimap<char, HuffCode> HuffCodeMap;

namespace HuffmanCoding
{
	void TransformCharDomain(std::string& text);
	FreqMap ComputeFrequencies(const std::string& Text);
	HuffCodeMap GenerateCodes(const FreqMap& Frequencies);

	DataBits Compress(const Data& Data, const HuffCodeMap& Codes);
	Data Decompress(const DataBits& Data, const HuffCodeMap& Codes);
};

#endif
