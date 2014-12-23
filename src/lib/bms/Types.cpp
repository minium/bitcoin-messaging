/**
 * Types.cpp
 *
 * Types module with definitions of various types and
 * a set of auxiliary type conversion and manipulation operations.
 *
 *  @author Krzysztof Okupski
 *  @version 1.0
 */

#include "Types.h"
#include "util.h"

#include <stdlib.h>
#include <iostream>
#include <random>

using std::string;
using std::vector;
using std::stringstream;


/**
 * Pads the binary vector with \p nBits zero's.
 * @param bits The binary vector to be padded.
 * @param nBits Number of padding bits.
 */
void PadBits(DataBits& bits, uint32_t nBits)
{
	bits.insert(bits.end(), nBits, false);
}

/**
 * Cuts the first \p nBits bits out of the binary vector and returns them.
 * @param bits The binary vector to be sliced.
 * @param nBits Number of leading bits to be cut out.
 */
DataBits SliceBits(DataBits& bits, uint32_t nBits)
{
	DataBits slice = DataBits(bits.begin(), bits.begin()+nBits);
	bits.erase(bits.begin(), bits.begin()+nBits);

	return slice;
}


/**
 * Converts a binary vector with exactly 8 elements into a character.
 * @param Vec Binary vector to be converted into a character.
 * @result Converted character.
 */
char BoolVecToChar(const vector<bool>& Vec)
{
	assert(Vec.size() == 8);

	char c = 0x00;
	for (unsigned int i = 0; i < 8; i++)
	{
		c += (Vec[i] << (7 - i));
	}

	return c;
}

/**
 * Converts a character into a binary vector with exactly 8 elements.
 * @param ch Character to be converted.
 * @result Converted binary vector.
 */
vector<bool> CharToBoolVec(char ch)
{
	vector<bool> vec;
	for (unsigned int i = 0; i < 8; i++)
	{
		vec.push_back((ch >> (7 - i)) & 0x01);
	}

	return vec;
}

/**
 * Converts a vector of unsigned chars into a binary vector.
 * @param data Unsigned char vector to be converted.
 * @result Converted binary vector.
 */
DataBits DataToBits(const Data& data)
{
	DataBits vec, tmp;
	for(Data::const_iterator it = data.begin(); it != data.end(); it++)
	{
		tmp = CharToBoolVec(*it);
		vec.insert(vec.end(), tmp.begin(), tmp.end());
	}

	return vec;
}

/**
 * Converts a binary vector into a vector of unsigned chars.
 * @param data Binary vector to be converted.
 * @result Converted unsigned char vector.
 */
Data BitsToData(const DataBits& data)
{
	assert(data.size() % 8 == 0);

	Data buf;
	DataBits slice;
	unsigned char tmp;

	for(unsigned int i = 0; i < data.size(); i+=8)
	{
		slice = DataBits(data.begin()+i, data.begin()+i+8);
		tmp = BoolVecToChar(slice);
		buf.push_back(tmp);
	}

	return buf;
}

/**
 * Converts a binary vector into an integer.
 * @param data Binary vector to be converted.
 * @result Converted integer.
 */
BigInt DataBitsToInt(const DataBits& data)
{
	BigInt num = 0;

	for(unsigned int i = 0; i < data.size()-1; i++)
	{
		num += data[i] & 0x01;
		num *= 2;
	}
	num += data[data.size()-1] & 0x01;

	return num;
}

/**
 * Converts an integer into a binary vector.
 * @param num Integer to be converted.
 * @result Converted binary vector.
 */
DataBits IntToDataBits(BigInt num)
{
	DataBits dataBits;

	while(num != 0)
	{
		dataBits.insert(dataBits.begin(), (bool)(num & 0x01));
		num /= 2;
	}

	return dataBits;
}
