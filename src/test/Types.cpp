/**
 * Types.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */


#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "Types.h"
#include "Utilities.h"


BOOST_AUTO_TEST_SUITE(TypesTests)

BOOST_AUTO_TEST_CASE(CharToBoolConversion)
{
	char c = 'a';
	DataBits vec = {false, true, true, false, false, false, false, true};

	BOOST_REQUIRE(CharToBoolVec(c) == vec);

}

BOOST_AUTO_TEST_CASE(BoolToCharConversion)
{
	char c = 'a';
	DataBits vec = {false, true, true, false, false, false, false, true};

	BOOST_REQUIRE(c == BoolVecToChar(vec));
}

BOOST_AUTO_TEST_CASE(DataToBitsConversion)
{
	for(int i = 0; i < 1000; i++)
	{
		Data origData = ParseHex(Utilities::GenerateRandomHexString(50));
		DataBits convData = DataToBits(origData);

		BOOST_REQUIRE(BitsToData(convData) == origData);
	}
}

BOOST_AUTO_TEST_CASE(BitsToIntConversion)
{
	Data data = {0x41,0x41,0x41,0x41,
				 0x42,0x42,0x42,0x42,
				 0x43,0x43,0x43,0x43,
				 0x44,0x44,0x44,0x44};

	BigInt num("86738642548785208971184551234260714564");

	BOOST_REQUIRE(DataBitsToInt(DataToBits(data)) == num);
}

BOOST_AUTO_TEST_CASE(IntToBitsConversion)
{
	Data data = {0x41,0x41,0x41,0x41,
				 0x42,0x42,0x42,0x42,
				 0x43,0x43,0x43,0x43,
				 0x44,0x44,0x44,0x44};

	BigInt num("86738642548785208971184551234260714564");

	DataBits tmp = IntToDataBits(num);

	if((tmp.size() % 8) != 0)
	{
		tmp.insert(tmp.begin(), 8 - (tmp.size() % 8), false);
	}

	BOOST_REQUIRE(BitsToData(tmp) == data);
}

BOOST_AUTO_TEST_SUITE_END()

