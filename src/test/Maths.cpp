/**
 * Maths.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "Maths.h"

using namespace Math;


BOOST_AUTO_TEST_SUITE(MathTests)

BOOST_AUTO_TEST_CASE(Factorial_Test)
{
	BigInt bin1("1307674368000");
	BigInt bin2("2432902008176640000");

	BOOST_REQUIRE(Factorial(15) == bin1);
	BOOST_REQUIRE(Factorial(20) == bin2);
}

BOOST_AUTO_TEST_CASE(BinomialCoefficient_Test)
{
	BigInt bin1("2743355077591282538231819720749000");
	BigInt bin2("4110309813715592060664607462520467"
			"3661181482104821558334324522322245"
			"3961686492150576818256239849895780"
			"8721334755964733592404450443749995"
			"000000");

	BOOST_REQUIRE(BinomialCoefficient(10000, 10) == bin1);
	BOOST_REQUIRE(BinomialCoefficient(100000000, 20) == bin2);
}

BOOST_AUTO_TEST_CASE(NumberCompositions_Test)
{
	BigInt bin("82221973055587378044140706739446352"
			"15122022458298030777261095087856400"
			"805804158672921856713387501");

	BOOST_REQUIRE(NumberCompositions(4, 3) == 15);
	BOOST_REQUIRE(NumberCompositions(1000000, 20) == bin);
}


BOOST_AUTO_TEST_CASE(PermutationToInt)
{
	std::vector<uint16_t> perm1 = {3,2,1,0};
	std::vector<uint16_t> perm2 = {2,7,8,3,9,1,5,6,0,4};

	BOOST_REQUIRE(PermutationToInteger(perm1) == 23);
	BOOST_REQUIRE(PermutationToInteger(perm2) == 1000000);
}

BOOST_AUTO_TEST_CASE(IntToPermutation)
{
	std::vector<uint16_t> perm1 = {3,2,1,0};
	std::vector<uint16_t> perm2 = {2,7,8,3,9,1,5,6,0,4};

	BOOST_REQUIRE(IntegerToPermutation(23,4) == perm1);
	BOOST_REQUIRE(IntegerToPermutation(1000000,10) == perm2);
}

BOOST_AUTO_TEST_SUITE_END()


