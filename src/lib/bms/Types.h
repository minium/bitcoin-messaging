/**
 * Types.h
 *
 * Types module with definitions of various types and
 * a set of auxiliary type conversion and manipulation operations.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_TYPES_H
#define BMS_TYPES_H

#include <string>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include "base58.h"
#include "core.h"
#include "key.h"
#include "keystore.h"

typedef std::vector<CTransaction> TransactionChain;
typedef std::vector<unsigned char> Data;
typedef std::vector<bool> DataBits;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::multiprecision::cpp_dec_float_100 BigFloat;


void PadBits(DataBits& data, uint32_t nBits);
DataBits SliceBits(DataBits& data, uint32_t nBits);

char BoolVecToChar(const std::vector<bool>& Vec);
std::vector<bool> CharToBoolVec(char ch);

DataBits DataToBits(const Data& Data);
Data BitsToData(const DataBits& Data);

BigInt DataBitsToInt(const DataBits& Data);
DataBits IntToDataBits(BigInt num);

#endif
