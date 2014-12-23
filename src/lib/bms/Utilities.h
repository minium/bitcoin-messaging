/**
 * Utilities.h
 *
 * Utilities module for management of configuration
 * files and various auxiliary functions.
 *
 *  @author Krzysztof Okupski
 *  @version 1.0
 */

#ifndef BMS_UTILITIES_H
#define BMS_UTILITIES_H

#include <string>

#include "BitcoinWallet.h"
#include "DataCompression.h"
#include "Types.h"

typedef std::map<DataBits, CKey> KeypairMap;
typedef CBasicKeyStore KeyStore;
typedef std::map<std::string, std::string> ConfigMap;

namespace Utilities
{
	extern ConfigMap Config;
	extern KeypairMap KeyMap;
	extern KeyStore Store;
	extern HuffCodeMap HuffCode;
	extern BitcoinAPI Wallet;
    
	void LoadConfigFile();
	void LoadHuffmanCode();
	void LoadKeypairMap();
	void LoadKeystore();
	void LoadWallet();
	void UpdateChainstate(const uint256 FirstTx, const uint256 LastTx);

	bool IsBitcoinRunning();
	bool IsConfigFileLoaded();
	bool IsHuffmanCodeLoaded();
	bool IsKeypairMapLoaded();
	bool IsKeystoreLoaded();
	bool IsWalletLoaded();

	void UnloadConfigFile();
	void UnloadHuffmanCode();
	void UnloadKeypairMap();
	void UnloadKeystore();
	void UnloadWallet();

	std::string GenerateRandomHexString(unsigned int nChars);
	DataBits GenerateRandomBits(unsigned int nBits);
	KeypairMap GenerateKeypairMap(unsigned int nBits);

	uint64_t TransactionFee(const CTransaction& Tx);
	uint64_t TransactionFee(const TransactionChain& Chain);
	uint64_t MinimumBudget(unsigned int nBits);

	std::string TransactionToHexString(const CTransaction& Tx);
	CTransaction HexStringToTransaction(const std::string& HexString);
};

#endif
