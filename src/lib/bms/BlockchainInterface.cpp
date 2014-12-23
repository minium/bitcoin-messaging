/**
 * BlockchainInterface.cpp
 *
 * Module providing functionality for embedding data into and
 * extracting data from transaction transactions.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include "BlockchainInterface.h"
#include "DataInterface.h"
#include "BitcoinException.h"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <string>

using std::string;
using std::vector;

namespace BlockchainInterface
{

/**
 * Selects unspent outputs that accumulate above the specified minimum.
 * @param nMin Minimum spendable amount.
 * @return Set of unspent outputs
 */
InitInputs SelectInputs(uint64_t nMin)
{
	InitInputs params = {};
	COutPoint point;

	/* Check if there is enough budget in the first place */
	if(Utilities::Wallet.getbalance() * pow(10,8) < nMin)
        throw std::runtime_error("[SelectInputs] Insufficient funds");

	/* Get list of unspent outputs and sort by amount in descending order */
	vector<unspenttxout_t> unspent = Utilities::Wallet.listunspent();
	sort(unspent.begin(), unspent.end(),
			[](const unspenttxout_t& lhs, const unspenttxout_t& rhs)
			{
				return lhs.amount > rhs.amount;
			}
	);

	/* Get last transaction output in chain and find it among the unspent outputs */
	point = COutPoint(uint256(Utilities::Config.at("State.LastTx")),0);
	for(unsigned int i = 0; i < unspent.size(); i++)
	{
		if(unspent[i].txid == point.hash.GetHex() && unspent[i].n == point.n){
			point = COutPoint(uint256(unspent[i].txid), unspent[i].n);

			params.budget += (uint64_t)(unspent[i].amount * pow(10,8));
			params.utxo.push_back(point);

			unspent.erase(unspent.begin()+i);
			break;
		}
	}

	/* Choose remaining unspent outputs until sufficient amount is accumulated */
	for(unsigned int i = 0; i < unspent.size(); i++)
	{
        if(params.budget >= nMin)
			break;

		point = COutPoint(uint256(unspent[i].txid), unspent[i].n);

		params.budget += (uint64_t)(unspent[i].amount * pow(10,8));
		params.utxo.push_back(point);

		
	}

	return params;
}

/**
 * Creates a new keypair and returns the corresponding address.
 * @return A new Bitcoin address.
 */
CBitcoinAddress SelectAddress()
{
	return CBitcoinAddress(Utilities::Wallet.getnewaddress());
}


/**
 * Embeds data into the sequence number of a transaction input.
 * @param bits To be embedded data.
 * @param txIn Transaction input in which the data is to be embedded.
 */
void PackDataIntoSeqNr(DataBits& bits, CTxIn& txIn)
{
	int nBits = std::min(32, (int)bits.size());
	PadBits(bits, 32 - nBits);
	DataBits slice = SliceBits(bits, 32);

	txIn.nSequence = DataInterface::EncodeDataInSequenceNr(slice);
}

/**
 * Extracts data from the sequence number of a transaction input.
 * @result Embedded data.
 */
DataBits UnpackDataFromSeqNr(const CTxIn& txIn)
{
	return DataInterface::DecodeDataInSequenceNr(txIn.nSequence);
}

/**
 * Embeds data into a script pair of P2SH standard transaction type.
 * The data is first embedded in the signature script of the transaction
 * input in the next transaction and then the corresponding public key
 * script in the transaction output in the current transaction is created.
 * @param bits To be embedded data.
 * @param txOut Transaction output of the current transaction.
 * @param tx Current transaction.
 * @param nInput Index of the current transaction input.
 */
void PackDataIntoP2SH(DataBits& bits, CTxOut& txOut, CTransaction& tx, int nInput)
{
	DataBits slice;
	vector<CPubKey> pubkeys;
	CScript multisigAddress;

	/* Embed data into first public key */
	int nSuffixBits = std::stoi(Utilities::Config.at("Keymap.SuffixBits"));
	int nBits = std::min(nSuffixBits, (int)bits.size());

	PadBits(bits, nSuffixBits - nBits);
	slice = SliceBits(bits, nSuffixBits);

	pubkeys.push_back(Utilities::KeyMap.at(slice).GetPubKey());

	/* Embed data into remaining pubkeys */
	int nExtraKeys = std::min(11, (int)ceil(bits.size()/250.0));
	for(int i = 0; i < nExtraKeys; i++)
	{
		nBits = std::min(250, (int)bits.size());
		PadBits(bits, 250 - nBits);
		slice = SliceBits(bits, 250);

		pubkeys.push_back(DataInterface::EncodeDataInPubkey(slice,5));
	}

	multisigAddress.SetMultisig(1, pubkeys);
	Utilities::Store.AddCScript(multisigAddress);

	txOut.scriptPubKey.SetDestination(CBitcoinAddress(multisigAddress.GetID()).Get());

	/* Temporarily sign the corresponding input */
	if(!SignSignature(Utilities::Store, txOut.scriptPubKey, tx, nInput, SIGHASH_ALL))
	{
		throw std::runtime_error("[PackDataIntoP2SH] Temporary signature generation failed");
	}
}

/**
 * Extracts data from a signature script of P2SH transaction type.
 * @param txIn A transaction input.
 * @result Data embedded in the signature script.
 */
DataBits UnpackDataFromP2SH(const CTxIn& txIn)
{
    DataBits bits;
	DataBits slice;

	vector<CPubKey> pubkeys;

	CScript scriptSig = txIn.scriptSig;
	CScript::const_iterator start;
	CScript::const_iterator end;

	opcodetype opcode;
	vector<unsigned char> scriptData;

	start = scriptSig.begin();
	end = scriptSig.end();

	/* Extract the redemption script from P2SH signature script */
	while (start < end && scriptSig.GetOp(start, opcode, scriptData));

	/* Extract the embedded public keys */
    CScript scriptPubkey(scriptData.begin(), scriptData.end());
    start = scriptPubkey.begin();
    end = scriptPubkey.end();

    while (start < end && scriptPubkey.GetOp(start, opcode, scriptData))
    {
    	if(scriptData.size() == 33){
    		pubkeys.push_back(CPubKey(scriptData));
    	}
    }

    /* Extract the data from the public keys */
    int nSuffixBits = std::stoi(Utilities::Config.at("Keymap.SuffixBits"));
    DataBits buf = DataToBits(Data(pubkeys[0].begin(), pubkeys[0].end()));
    slice = DataBits(buf.end()-nSuffixBits, buf.end());

    pubkeys.erase(pubkeys.begin(),pubkeys.begin()+1);
    bits.insert(bits.end(), slice.begin(), slice.end());

    for(vector<CPubKey>::const_iterator it = pubkeys.begin(); it != pubkeys.end(); it++)
    {
    	slice = DataInterface::DecodeDataInPubkey(*it, 5);
    	bits.insert(bits.end(), slice.begin(), slice.end());
    }

    return bits;
}

/**
 * Embeds data into the public key script of a Nulldata transaction output.
 * @param bits To be embedded data.
 * @param txIn Transaction output in which the data is to be embedded.
 */
void PackDataIntoNulldata(DataBits& bits, CTxOut& txOut)
{
	int nBits = std::min(320, (int)bits.size());
	PadBits(bits, 320 - nBits);
	DataBits slice = SliceBits(bits, 320);

	txOut.scriptPubKey = (CScript() << OP_RETURN << BitsToData(slice));
	txOut.nValue = 0;
}

/**
 * Extracts data from a public key script of Nulldata transaction type.
 * @param txOut A transaction output of Nulldata type.
 * @result Data embedded in the public key script.
 */
DataBits UnpackDataFromNulldata(const CTxOut& txOut)
{
	Data data = txOut.scriptPubKey;
	data.erase(data.begin(), data.begin()+2);

	return DataToBits(data);
}

/**
 * Embeds data into the transaction output values of a set of transaction outputs.
 * @param bits To be embedded data.
 * @param txOuts Vector of transaction outputs.
 * @param budget Spendable budget.
 * @param lbound Lower bound of each transaction output value.
 */
void PackDataIntoBudgetSplit(DataBits& bits, vector<CTxOut>& txOuts, uint64_t budget, uint64_t lbound)
{
	const uint32_t nOutputs = txOuts.size();
	const uint32_t MaxBits = DataInterface::EmbeddableBitsInValues(budget - (nOutputs*lbound), nOutputs);
	const uint32_t nBits = std::min(MaxBits, (uint32_t)bits.size());

	assert(budget >= nOutputs * lbound);

	DataBits slice = SliceBits(bits, nBits);
	PadBits(slice, MaxBits - nBits);

	vector<uint64_t> values = DataInterface::EncodeDataInValues(slice, budget - (nOutputs * lbound), nOutputs);
	for(unsigned int i = 0; i < nOutputs; i++)
	{
		txOuts[i].nValue = values[i] + lbound;
	}
}

/**
 * Extracts data from transaction output values of a set of transaction outputs.
 * @param txOuts Vector of transaction outputs.
 * @param lbound Lower bound of each transaction output value.
 * @result Data embedded in transaction output values.
 */
DataBits UnpackDataFromBudgetSplit(const vector<CTxOut>& txOuts, uint64_t lbound)
{
	const uint32_t outputs = txOuts.size();
	vector<uint64_t> values(outputs);

	for(unsigned int i = 0; i < outputs; i++)
	{
		values[i] = txOuts[i].nValue - lbound;
	}

	return DataInterface::DecodeDataInValues(values);
}

/**
 * Embeds data into the order in which transaction outputs are claimed.
 * @param bits To be embedded data.
 * @param txInputs Vector of transaction inputs.
 */
void PackDataIntoBudgetClaim(DataBits& bits, vector<CTxIn>& txInputs)
{
	const uint32_t nInputs = txInputs.size();
	const uint32_t MaxBits = DataInterface::EmbeddableBitsInPermutation(nInputs);
	const uint32_t nBits = std::min(MaxBits, (uint32_t)bits.size());

	DataBits slice = SliceBits(bits, nBits);
	PadBits(slice, MaxBits - nBits);

	vector<uint16_t> perm = DataInterface::EncodeDataInPermutation(slice, nInputs);
	for(unsigned int i = 0; i < nInputs; i++)
	{
		txInputs[i].prevout.n = perm[i];
	}
}

/**
 * Extracts data from the order of claimed transaction outputs.
 * @param txInputs Vector of transaction inputs.
 * @result Data embedded in the order of claimed transaction outputs.
 */
DataBits UnpackDataFromBudgetClaim(const vector<CTxIn>& txInputs)
{
	const uint32_t nInputs = txInputs.size();
	vector<uint16_t> perm(nInputs);

	for(unsigned int i = 0; i < nInputs; i++)
	{
		perm[i] = txInputs[i].prevout.n;
	}

	return DataInterface::DecodeDataInPermutation(perm);
}

/*
 * Calculates the optimal parameters of a transaction for a given amount of data that is
 * to be embedded in it. It determines the minimum number of P2SH transaction input/output
 * pairs as well as Nulldata transaction outputs.
 * @param tx Transaction in which data is to be embedded.
 * @param bits Data to be embedded.
 * @param nBudget Available spendable budget.
 * @param Resulting parameters.
 */
void OptimizeParams(const CTransaction& tx, const DataBits& bits, const uint64_t nBudget, Parameters& params)
{
	unsigned int nBitsA = std::stoi(Utilities::Config.at("Keymap.SuffixBits"));
	unsigned int nBitsB = 255 - std::stoi(Utilities::Config.at("Random.SuffixBits"));
	unsigned int nBitsSeqNr = 32;
	unsigned int nBitsNulldata = 320;

	CScript dummyA = (CScript() << OP_HASH160 << Data(20) << OP_EQUAL);
	CScript dummyB = (CScript() << OP_RETURN << Data(40));

	unsigned int nBitsBudgetSplit;
	unsigned int nBitsBudgetClaim;
	unsigned int nTotalEmbeddableBits;

	unsigned int nScriptHash = 14;
	unsigned int nNulldata = 1;

	uint64_t nFees;

	if(bits.size() == 0)
	{
		params.nScriptHash = 0;
		params.nNulldata = 0;
		params.nFees = Utilities::TransactionFee(tx);
		return;
	}

	do{
		CTransaction tmp = tx;
		tmp.vout.clear();

		/* Calculate embeddable data in script hashes */
		for(unsigned int i = 0; i < nScriptHash; i++)
		{
			tmp.vout.push_back(CTxOut());
			tmp.vout.back().scriptPubKey = dummyA;
		}

		nTotalEmbeddableBits = nScriptHash * (nBitsA + 11*nBitsB);
		nTotalEmbeddableBits += nScriptHash * nBitsSeqNr;

		/* Embed data in Nulldata whenever necessary */
		if((nScriptHash == 1 && bits.size() > nTotalEmbeddableBits) || nScriptHash >= 2)
		{
			nNulldata = 1;

			tmp.vout.push_back(CTxOut());
			tmp.vout.back().scriptPubKey = dummyB;

			nTotalEmbeddableBits += nBitsNulldata;
		}
		else
		{
			nNulldata = 0;
		}

		/* If the fees are more than the budget, then return */
		nFees = Utilities::TransactionFee(tmp);

		/* Embed data in budget split and claim */
		if(nScriptHash >= 2)
		{
			if(nBudget >= nFees)
				nBitsBudgetSplit = DataInterface::EmbeddableBitsInValues(nBudget-nFees, nScriptHash);
			else
				nBitsBudgetSplit = 0;

			nBitsBudgetClaim = DataInterface::EmbeddableBitsInPermutation(nScriptHash);

			nTotalEmbeddableBits += nBitsBudgetSplit + nBitsBudgetClaim;
		}

		if(nTotalEmbeddableBits >= bits.size() || nScriptHash == 14)
		{
			params = {nScriptHash, nNulldata, nFees};
			nScriptHash--;
		}

	}while(nTotalEmbeddableBits >= bits.size() && nScriptHash > 0);

}

/**
 * Embeds data in a chain of transactions.
 * @param data Data to be embedded.
 * @param nBudget Spendable budget.
 * @param prevOut Available spendable outputs.
 * @param addr Destination address of the remaining budget.
 * @result Chain of transactions embedding the data.
 */
TransactionChain EmbedData(const DataBits& data, uint64_t nBudget, const vector<COutPoint>& prevOut, const CBitcoinAddress& addr)
{
	TransactionChain txs(2);
	DataBits bits = data;
	DataBits slice;
	Parameters params;

	for(vector<COutPoint>::const_iterator it = prevOut.begin(); it != prevOut.end(); it++)
	{
		CTxIn in;
		in.prevout = *it;
		txs[0].vin.push_back(in);
	}

	int idx = 0;
	do{
		OptimizeParams(txs[idx], bits, nBudget, params);

		unsigned int nScriptHash = params.nScriptHash;
		unsigned int nNulldata = params.nNulldata;
		uint64_t nFees = params.nFees;

		/* Set number script hash outputs */
		for(unsigned int i = 0; i < nScriptHash; i++)
		{
			txs[idx].vout.insert(txs[idx].vout.begin(), CTxOut());
			txs[idx+1].vin.insert(txs[idx+1].vin.begin(), CTxIn());

			txs[idx+1].vin[i].prevout.n = i;
		}

		/* Embed data into budget split and claim */
		if(txs[idx].vout.size() == 1)
		{
			txs[idx].vout[0].nValue = nBudget - nFees;
			txs[idx+1].vin[0].prevout.n = 0;
		}
		else if(txs[idx].vout.size() >= 2)
		{
			if(nBudget >= nFees)
				PackDataIntoBudgetSplit(bits, txs[idx].vout, nBudget - nFees, 546);

			PackDataIntoBudgetClaim(bits, txs[idx+1].vin);
		}

		/* Set nulldata output and embed data in it */
		if(nNulldata)
		{
			txs[idx].vout.insert(txs[idx].vout.end(), CTxOut());
			PackDataIntoNulldata(bits, txs[idx].vout.back());
		}

		/* Embed data in script hash outputs */
		for(unsigned int i = 0; i < nScriptHash; i++)
		{
			int n = txs[idx+1].vin[i].prevout.n;
			PackDataIntoP2SH(bits, txs[idx].vout[n], txs[idx+1], i);
			PackDataIntoSeqNr(bits, txs[idx+1].vin[i]);
		}

		nBudget = nBudget - nFees;

		if(bits.size() > 0)
		{
			txs.push_back(CTransaction());
			idx++;
		}

	}while(bits.size() > 0);

	OptimizeParams(txs[idx+1], bits, nBudget, params);

	CTxOut out;
	out.nValue = nBudget - params.nFees;
	out.scriptPubKey.SetDestination(addr.Get());
	txs[idx+1].vout.push_back(out);

	return txs;
}


/**
 * Extracts data from a chain of transactions.
 * @param txs Chain of transactions
 * @return Embedded data.
 */
DataBits ExtractData(const TransactionChain& txs)
{
	const unsigned int nTransactions = txs.size();
	vector<CTransaction> tmp = txs;

	DataBits bits, slice;
	CTxOut buf;

	unsigned int nScriptHash;
	unsigned int nNulldata;

	for(unsigned int idx = 0; idx < nTransactions-1; idx++)
	{
		nNulldata = (tmp[idx].vout.size() >= 2) ? 1 : 0;
		nScriptHash = tmp[idx].vout.size() - nNulldata;

		if(nNulldata)
		{
			buf = tmp[idx].vout.back();
			tmp[idx].vout.pop_back();
		}

		/* Extract data from budget split and claim */
		if(nScriptHash >= 2)
		{
			slice = UnpackDataFromBudgetSplit(tmp[idx].vout, 546);
			bits.insert(bits.end(), slice.begin(), slice.end());

			slice = UnpackDataFromBudgetClaim(tmp[idx+1].vin);
			bits.insert(bits.end(), slice.begin(), slice.end());
		}

		/* Extract data from Nulldata transaction output */
		if(nNulldata)
		{
			slice = UnpackDataFromNulldata(buf);
			bits.insert(bits.end(), slice.begin(), slice.end());
		}


		/* Extract data from P2SH transaction pairs */
		for(unsigned int j = 0; j < nScriptHash; j++)
		{
			slice = UnpackDataFromP2SH(tmp[idx+1].vin[j]);
			bits.insert(bits.end(), slice.begin(), slice.end());

			slice = UnpackDataFromSeqNr(tmp[idx+1].vin[j]);
			bits.insert(bits.end(), slice.begin(), slice.end());

		}
	}

	return bits;
}

/**
 * Signs all the transaction inputs of all transactions in the chain.
 * @param txs Chain of transactions.
 * @result True if signing was successful. Otherwise not.
 */
void AuthorizeTransactions(TransactionChain& txs)
{
	/* Sign first transaction */
	string HexTx = Utilities::TransactionToHexString(txs[0]);
	signrawtransaction_t SignedHexTx;

	try{
		SignedHexTx = Utilities::Wallet.signrawtransaction(HexTx);
	}catch(BitcoinException& e){
        string err;
        err += "[Authorize transaction] Failed to sign transaction";
        err += "\nReason: ";
        err += e.getMessage();
        throw std::runtime_error(err);
    }

	if(!SignedHexTx.complete){
	    throw std::runtime_error("[AuthorizeTransactions] The signature is not complete");	
    }

	txs[0] = Utilities::HexStringToTransaction(SignedHexTx.hex);

	/* Set referenced transaction and sign it */
	for(unsigned int i = 1; i < txs.size(); i++)
	{
		for(unsigned int j = 0; j < txs[i].vin.size(); j++)
		{
			txs[i].vin[j].prevout.hash = txs[i-1].GetHash();
		}

		for(unsigned int j = 0; j < txs[i].vin.size(); j++)
		{
			int n = txs[i].vin[j].prevout.n;
			if(!SignSignature(Utilities::Store, txs[i-1].vout[n].scriptPubKey, txs[i], j, SIGHASH_ALL)){
                throw std::runtime_error("[AuthorizeTransactions] Signature generation failed");	
			}
		}
	}
}

/**
 * Sends chain of transactions into the network.
 * @param txs Chain of transactions.
 * @result True if sending was successful. Otherwise not.
 */
void SendTransactions(const TransactionChain& txs)
{
	try
	{
		for(vector<CTransaction>::const_iterator it = txs.begin(); it != txs.end(); it++)
		{
			Utilities::Wallet.sendrawtransaction(Utilities::TransactionToHexString(*it));
		}
	}
	catch(BitcoinException& e)
	{
        string err;
        err += "[SendTransaction]: Failed to send transaction";
        err += "\nReason: ";
        err += e.getMessage();
        throw std::runtime_error(err);
	}
}

/**
 * Extracts transaction chains from the blockchain that begin at a
 * specified transaction and end at another specified transaction.
 * @param TxBegin First transaction ID.
 * @param TxEnd Last transaction ID.
 * @result Vector of transaction chains.
 */
vector<TransactionChain> ReadTransactions(uint256 TxBegin, uint256 TxEnd)
{
	vector<TransactionChain> chains;
	TransactionChain chain;

	/* Build up transaction chain backwards */
	try
	{
		while(TxBegin != TxEnd){
			string buf = Utilities::Wallet.getrawtransaction(TxEnd.GetHex()).hex;
			chain.push_back(Utilities::HexStringToTransaction(buf));
			TxEnd = chain.back().vin[0].prevout.hash;
		}

		string buf = Utilities::Wallet.getrawtransaction(TxEnd.GetHex()).hex;
		chain.push_back(Utilities::HexStringToTransaction(buf));
		std::reverse(chain.begin(), chain.end());
	}
	catch(BitcoinException& e)
	{
        string err;
        err += "[ReadTransactions]: Failed to read transaction";
        err += "\nReason: ";
        err += e.getMessage();
        throw std::runtime_error(err);
	}

	/* Split transaction chain into subchains */
	TransactionChain::const_iterator start = chain.begin();
	for(TransactionChain::const_iterator it = chain.begin(); it != chain.end(); it++)
	{
		if(it->vout.size() == 1 && !it->vout.front().scriptPubKey.IsPayToScriptHash())
		{
			TransactionChain subchain = TransactionChain(start, it+1);
			chains.push_back(subchain);

			start = it+1;
		}
	}

	return chains;
}

/**
 * Extracts transaction chains from the blockchain that begin at a
 * specified transaction and end within 10 blocks of the beginning transaction.
 * @param TxBegin First transaction ID.
 * @param nBlocks Distance in blocks from the first transaction.
 * @result Vector of transaction chains.
 */
vector<TransactionChain> ReadTransactions(uint256 TxBegin, unsigned int nBlocks)
{
	vector<TransactionChain> chains;
	TransactionChain chain;

	/* Build up transaction forwards */
	try
	{
		vector<CTransaction> txs;
		vector<string> refs;

		blockinfo_t block;
		getrawtransaction_t rawtx = Utilities::Wallet.getrawtransaction(TxBegin.GetHex(), 1);

		chain.push_back(Utilities::HexStringToTransaction(rawtx.hex));
		string BeginBlockHash = rawtx.blockhash;

		for(unsigned int i = 0; i < nBlocks; i++)
		{
			block = Utilities::Wallet.getblock(BeginBlockHash);
			refs = block.tx;

			for(unsigned int j = 0; j < refs.size(); j++)
			{
				txs.push_back(Utilities::HexStringToTransaction(Utilities::Wallet.getrawtransaction(refs[j]).hex));
			}

			for(unsigned int j = 0; j < txs.size(); j++)
			{
				if(txs[j].vin.front().prevout.hash == TxBegin)
				{
					chain.push_back(txs[j]);
					TxBegin = txs[j].GetHash();
					txs.erase(txs.begin()+j);
					j = 0;
				}
			}
			BeginBlockHash = block.nextblockhash;
		}
	}
	catch(BitcoinException& e)
	{
        string err;
        err += "[SendTransaction]: Failed to send transaction";
        err += "\nReason: ";
        err += e.getMessage();
        throw std::runtime_error(err);
	}

	TransactionChain::const_iterator start = chain.begin();
	for(TransactionChain::const_iterator it = chain.begin(); it != chain.end(); it++)
	{
		if(it->vout.size() == 1)
		{
			TransactionChain subchain = TransactionChain(start, it+1);
			chains.push_back(subchain);

			start = it+1;
		}
	}

	return chains;
}


}
