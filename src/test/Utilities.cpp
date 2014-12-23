/**
 * Utilities.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "Utilities.h"
#include "Serialization.h"
#include "BlockchainInterface.h"

using namespace Utilities;


BOOST_AUTO_TEST_SUITE(UtilitiesTests)

BOOST_AUTO_TEST_CASE(RandomBitStringGenerator)
{
	for(unsigned int i = 0; i < 1000; i++)
	{
		DataBits bits = GenerateRandomBits(i);
		BOOST_REQUIRE(bits.size() == i);
	}
}

BOOST_AUTO_TEST_CASE(RandomStringGenerator)
{
	for(unsigned int i = 0; i < 1000; i++)
	{
		std::string str = GenerateRandomHexString(i);
		BOOST_REQUIRE(str.length() == i);
	}
}

BOOST_AUTO_TEST_CASE(KeypairMapGeneration)
{
	int nBits = 8;
	KeypairMap keymap;

	keymap = GenerateKeypairMap(nBits);

	BOOST_REQUIRE(keymap.size() == pow(2,nBits));
}

BOOST_AUTO_TEST_CASE(ConfigLoading)
{
	BOOST_REQUIRE(IsConfigFileLoaded());
}

BOOST_AUTO_TEST_CASE(HuffmanCodeLoading)
{
	BOOST_REQUIRE(IsHuffmanCodeLoaded());
}

BOOST_AUTO_TEST_CASE(KeystoreLoading)
{
	BOOST_REQUIRE(IsKeystoreLoaded());
}

BOOST_AUTO_TEST_CASE(WalletLoading)
{
	BOOST_REQUIRE(IsWalletLoaded());
}

//BOOST_AUTO_TEST_CASE(UpdateChainstate_Test)
//{
//	ReadConfigFile();
//	ConfigMap configA = Serialization::DeserializeConfigMap("bms.conf");
//	configA.at("State.FirstTx") = Utilities::GenerateRandomHexString(64);
//	configA.at("State.LastTx") = Utilities::GenerateRandomHexString(64);
//
//	UpdateChainstate(uint256(configA.at("State.FirstTx")), uint256(configA.at("State.LastTx")));
//
//	ConfigMap configB = Serialization::DeserializeConfigMap("bms.conf");
//
//	BOOST_REQUIRE(configA.at("State.FirstTx") == configB.at("State.FirstTx"));
//	BOOST_REQUIRE(configA.at("State.LastTx") == configB.at("State.LastTx"));
//}

BOOST_AUTO_TEST_CASE(TransactionFees_Test)
{
	TransactionChain chain(2);
	uint64_t nFee = TransactionFee(chain);
	uint64_t nFeeRate = std::stoul(Utilities::Config.at("TxFeeRate"));

	BOOST_REQUIRE(2*nFeeRate == nFee);
}

BOOST_AUTO_TEST_CASE(MinimumBudget_Test)
{
	uint64_t nBudget = MinimumBudget(10000);
	uint64_t nFeeRate = std::stoul(Utilities::Config.at("TxFeeRate"));
	BOOST_REQUIRE(5*nFeeRate == nBudget);


	nBudget = MinimumBudget(40000);
	BOOST_REQUIRE(12*nFeeRate == nBudget);
}

BOOST_AUTO_TEST_SUITE_END()
