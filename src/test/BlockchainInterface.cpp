/**
 * BlockchainInterface.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "BlockchainInterface.h"
#include "DataInterface.h"

using namespace BlockchainInterface;


BOOST_AUTO_TEST_SUITE(BlockchainInterfaceTests)

BOOST_AUTO_TEST_CASE(SelectInput_Test)
{
	uint64_t minBudget = 1;
	InitInputs inputs;

	inputs = SelectInputs(minBudget);

	BOOST_REQUIRE(inputs.utxo.size() >= 1);
	BOOST_REQUIRE(inputs.budget >= minBudget);
}

BOOST_AUTO_TEST_CASE(PackIntoSequenceNr_Test)
{
	for(int i = 0; i < 1000; i++)
	{
		DataBits bits = Utilities::GenerateRandomBits(i % 100);
		DataBits copy = DataBits(bits);

		DataBits slice = SliceBits(bits, std::min(32, i % 100));
		PadBits(slice, 32 - slice.size());

		CTxIn input = CTxIn();
		PackDataIntoSeqNr(copy, input);

		BOOST_REQUIRE(slice == UnpackDataFromSeqNr(input));
		BOOST_REQUIRE(bits == copy);
	}
}

BOOST_AUTO_TEST_CASE(PackIntoNulldata_Test)
{
	for(int i = 0; i < 1000; i++)
	{
		DataBits bits = Utilities::GenerateRandomBits(i % 500);
		DataBits copy = DataBits(bits);

		DataBits slice = SliceBits(bits, std::min(320, i % 500));
		PadBits(slice, 320 - slice.size());

		CTxOut output = CTxOut();
		PackDataIntoNulldata(copy, output);

		BOOST_REQUIRE(slice == UnpackDataFromNulldata(output));
		BOOST_REQUIRE(bits == copy);
	}
}

BOOST_AUTO_TEST_CASE(PackIntoBudgetSplit_Test)
{
	for(int i = 0; i < 1000; i++)
	{
		DataBits bits = Utilities::GenerateRandomBits(i % 500);
		DataBits copy = DataBits(bits);

		uint64_t budget = pow(10,7);
		uint64_t lbound = 546;
		int nOuts = 2 + (i%19);
		int nBits = DataInterface::EmbeddableBitsInValues(budget-(nOuts*lbound), nOuts);

		DataBits slice = SliceBits(bits, std::min(nBits, i % 500));
		PadBits(slice, nBits - slice.size());

		std::vector<CTxOut> txOuts(nOuts);
		PackDataIntoBudgetSplit(copy, txOuts, budget, lbound);

		BOOST_REQUIRE(slice == UnpackDataFromBudgetSplit(txOuts, lbound));
		BOOST_REQUIRE(bits == copy);
	}
}

BOOST_AUTO_TEST_CASE(PackIntoBudgetClaim_Test)
{
	for(int i = 0; i < 1000; i++)
	{
		DataBits bits = Utilities::GenerateRandomBits(i % 500);
		DataBits copy = DataBits(bits);

		uint16_t nIns = 2 + (i%19);
		std::vector<CTxIn> inputs(nIns);
		int nBits = DataInterface::EmbeddableBitsInPermutation(nIns);

		DataBits slice = SliceBits(bits, std::min(nBits, i % 500));
		PadBits(slice, nBits - slice.size());

		PackDataIntoBudgetClaim(copy, inputs);

		BOOST_REQUIRE(slice == UnpackDataFromBudgetClaim(inputs));
		BOOST_REQUIRE(bits == copy);
	}
}

BOOST_AUTO_TEST_CASE(PackIntoP2SH_Test)
{
	for(int i = 0; i < 1000; i++)
	{
		DataBits bits = Utilities::GenerateRandomBits(i % 500);
		DataBits copy = DataBits(bits);

		const int nKeys = std::min(11, (int)ceil((bits.size()-8)/250.0));
		const int MaxBits = 8 + nKeys * 250;

		CTransaction tx = CTransaction();
		CTxIn txIn = CTxIn();
		tx.vin.push_back(txIn);
		CTxOut txOut = CTxOut();

		DataBits slice = SliceBits(bits, std::min(MaxBits, (int)bits.size()));
		PadBits(slice, MaxBits - slice.size());

		PackDataIntoP2SH(copy, txOut, tx, 0);

		BOOST_REQUIRE(slice == UnpackDataFromP2SH(tx.vin[0]));
		BOOST_REQUIRE(bits == copy);
	}
}

//BOOST_AUTO_TEST_CASE(OptimalTxOutputCount_Test)
//{
//	for(int i = 0; i < 20; i++)
//	{
//		DataBits bits = Utilities::GenerateRandomBits(i*1000);
//
//		CTransaction tx = CTransaction();
//		tx.vin.push_back(CTxIn());
//		tx.vout.push_back(CTxOut());
//
//		BlockchainInterface::Parameters params;
//		BlockchainInterface::OptimizeParams(tx, bits, pow(10,7), params);
//	}
//}

BOOST_AUTO_TEST_CASE(EmbeddingInTransactions_Test)
{
	for(int i = 0; i < 40; i++)
	{
		DataBits originalData = Utilities::GenerateRandomBits(8*i*250);
		DataBits recoveredData;

		std::vector<COutPoint> prevOut;
		CBitcoinAddress addr;

		std::vector<CTransaction> txs = EmbedData(originalData, pow(10,7), prevOut, addr);
		recoveredData = BlockchainInterface::ExtractData(txs);

		int padding = recoveredData.size() - originalData.size();
		recoveredData.erase(recoveredData.end() - padding, recoveredData.end());

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_CASE(ExtractingFromTransactionsI_Test)
{
	uint256 TxBegin = uint256("ca0bc0e7d422a71e016d757ecdba8ee801791b5ece529cb5fbcb3cc04bcb102e");
	uint256 TxEnd = uint256("b12d1d539f84a16a98baf1e7f77ed30559785e9e51228a59d0af0dbe127d38d2");
	std::vector<TransactionChain> chains;

	chains = ReadTransactions(TxBegin, TxEnd);
	DataBits bits = ExtractData(chains.front());

	int padding = 8 - (bits.size() % 8);
	PadBits(bits, padding);

	Data data = BitsToData(bits);
	std::string str = std::string(data.begin(), data.end());

	BOOST_REQUIRE(str.size() == 5761);
}

BOOST_AUTO_TEST_CASE(ExtractingFromTransactionsII_Test)
{
	uint256 TxBegin = uint256("ca0bc0e7d422a71e016d757ecdba8ee801791b5ece529cb5fbcb3cc04bcb102e");
	std::vector<TransactionChain> chains;

	chains = ReadTransactions(TxBegin);
	DataBits bits = ExtractData(chains.front());

	int padding = 8 - (bits.size() % 8);
	PadBits(bits, padding);

	Data data = BitsToData(bits);
	std::string str = std::string(data.begin(), data.end());

	BOOST_REQUIRE(str.size() == 5761);
}

BOOST_AUTO_TEST_CASE(DryRun)
{
	for(int i = 0; i < 50; i++)
	{
		Data originalData = BitsToData(Utilities::GenerateRandomBits(8*i*250));
		std::string str(originalData.begin(), originalData.end());

		HuffmanCoding::TransformCharDomain(str);
		originalData = Data(str.begin(), str.end());

		DataBits compressedData = HuffmanCoding::Compress(originalData, Utilities::HuffCode);
		Data recoveredData;

		InitInputs params = SelectInputs(Utilities::MinimumBudget(originalData.size()));
		CBitcoinAddress addr = SelectAddress();
		TransactionChain chain = EmbedData(compressedData, params.budget, params.utxo, addr);

		Utilities::Wallet.walletpassphrase("123456", 10);
		BOOST_REQUIRE_NO_THROW(AuthorizeTransactions(chain));

		compressedData = ExtractData(chain);
		recoveredData = HuffmanCoding::Decompress(compressedData, Utilities::HuffCode);

		BOOST_REQUIRE(originalData == recoveredData);
	}
}

BOOST_AUTO_TEST_SUITE_END()
