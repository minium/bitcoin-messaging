/**
 * BlockchainInterface.h
 *
 * Module providing functionality for embedding data into and
 * extracting data from transaction transactions.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_BLOCKCHAININTERFACE_H
#define BMS_BLOCKCHAININTERFACE_H

#include "BitcoinWallet.h"
#include "Utilities.h"
#include "Types.h"


namespace BlockchainInterface
{
	struct InitInputs{
		std::vector<COutPoint> utxo;
		uint64_t budget;
	};

	struct Parameters{
		unsigned int nScriptHash;
		unsigned int nNulldata;
		uint64_t nFees;
	};

	InitInputs SelectInputs(uint64_t nMin);
	CBitcoinAddress SelectAddress();

	void PackDataIntoP2SH(DataBits& data, CTxOut& txOut, CTransaction& tx, int nInput);
	DataBits UnpackDataFromP2SH(const CTxIn& TxIn);

	void PackDataIntoSeqNr(DataBits& data, CTxIn& txIn);
	DataBits UnpackDataFromSeqNr(const CTxIn& TxIn);

	void PackDataIntoNulldata(DataBits& data, CTxOut& txOut);
	DataBits UnpackDataFromNulldata(const CTxOut& TxOut);

	void PackDataIntoBudgetSplit(DataBits& bits, std::vector<CTxOut>& txOuts, uint64_t budget, uint64_t lbound);
	DataBits UnpackDataFromBudgetSplit(const std::vector<CTxOut>& TxOuts, uint64_t lbound);

	void PackDataIntoBudgetClaim(DataBits& bits, std::vector<CTxIn>& txIns);
	DataBits UnpackDataFromBudgetClaim(const std::vector<CTxIn>& TxIns);

	void OptimizeParams(const CTransaction& Tx, const DataBits& Data, const uint64_t Budget, Parameters& params);
	TransactionChain EmbedData(const DataBits& Data, uint64_t budget, const std::vector<COutPoint>& PrevOut, const CBitcoinAddress& Addr);
	DataBits ExtractData(const TransactionChain& Txs);

	void AuthorizeTransactions(TransactionChain& txs);
	void SendTransactions(const TransactionChain& Txs);
	std::vector<TransactionChain> ReadTransactions(uint256 TxBegin, uint256 TxEnd);
	std::vector<TransactionChain> ReadTransactions(uint256 TxBegin, unsigned int nBlocks = 10);
}

#endif
