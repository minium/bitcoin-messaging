/**
 * Utilities.cpp
 *
 * Utilities module for management of configuration
 * files and various auxiliary functions.
 *
 * @author Krzysztof Okupski
 * @version 1.0  9/10/2014
 */

#include "Utilities.h"
#include "BlockchainInterface.h"
#include "Serialization.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <random>

#include <boost/filesystem.hpp>

using std::string;
using std::vector;

namespace fs = boost::filesystem;


namespace Utilities
{

ConfigMap Config;
KeypairMap KeyMap;
KeyStore Store;
HuffCodeMap HuffCode;
BitcoinAPI Wallet;

/** Loads directory path to configuration files */
string GetConfigPath()
{
    string path;

    /* Check if config files exist in home directory */
    string home = getenv("HOME");
    if(fs::exists(home + "/.bms") && fs::is_directory(home + "/.bms"))
    {
        path = home + "/.bms/";
    }

    /* Check if config files exist in same directory as executable */
    if(fs::exists("config") && fs::is_directory("config"))
    {
        path = "config/";        
    }

    if(path.empty())
    {
        throw std::runtime_error("[ConfigPath] Could not find config directory.");
    }

    return path;
}

/** Loads the configuration file with system parameters. */
void LoadConfigFile()
{
	if(IsConfigFileLoaded())
		return;

	Config = Serialization::DeserializeConfigMap(GetConfigPath() + "bms.conf");
}

/** Loads the Huffman code file with code mappings. */
void LoadHuffmanCode()
{
	if(IsHuffmanCodeLoaded())
		return;

	HuffCode = Serialization::DeserializeHuffmanCode(GetConfigPath() + "huffcode.map");
}

/** Loads the keypair map file with private keys for signing transactions. */
void LoadKeypairMap()
{
	if(!IsConfigFileLoaded())
		return;

	if(IsKeypairMapLoaded())
		return;

	if(!fs::exists(GetConfigPath() + "keypair.map"))
	{
		KeyMap = Utilities::GenerateKeypairMap(std::stoi(Config.at("Keymap.SuffixBits")));
		Serialization::SerializeKeypairMap(KeyMap, GetConfigPath() + "keypair.map");
	}else{
		KeyMap = Serialization::DeserializeKeypairMap(GetConfigPath() + "keypair.map");
	}


}

/** Loads the keystore with the private keys from the keypair map. */
void LoadKeystore()
{
	if(!IsKeypairMapLoaded())
		return;

	if(IsKeystoreLoaded())
		return;

	for(KeypairMap::const_iterator it = KeyMap.begin(); it != KeyMap.end(); it++)
	{
		Store.AddKey(it->second);
	}
}

/** Loads the wallet credentials for the connection with the Bitcoin daemon. */
void LoadWallet()
{
	if(!IsConfigFileLoaded())
		return;

	if(IsWalletLoaded())
		return;

	const string Username = Config.at("Wallet.User");
	const string Password = Config.at("Wallet.Password");
	const string Address = Config.at("Wallet.IP");
	const int Port = std::stoi(Config.at("Wallet.Port"));

	Wallet = BitcoinAPI(Username, Password, Address, Port);
}

/**
 * Updates the first and last transaction state in the configuration file.
 * @param FirsTx Index of the first transaction.
 * @param LastTx Index of the last transaction
 */
void UpdateChainstate(const uint256 FirstTx, const uint256 LastTx)
{
	if(!IsConfigFileLoaded())
		return;

	if(Config.at("State.FirstTx") == string(64, '0'))
		Config.at("State.FirstTx") = FirstTx.GetHex();

	Config.at("State.LastTx") = LastTx.GetHex();

	Serialization::SerializeState(Config, GetConfigPath() + "bms.conf");
}



/**
 * Checks whether the configuration file is loaded or not.
 * @return True if configuration file is loaded. Otherwise not.
 */
bool IsConfigFileLoaded()
{
	return !Config.empty();
}

/**
 * Checks whether the keypair map is loaded or not.
 * @return True if keypair map is loaded. Otherwise not.
 */
bool IsKeypairMapLoaded()
{
	return !KeyMap.empty();
}

/**
 * Checks whether the Huffman code is loaded or not
 * @return True if the Huffman code is loaded. Otherwise not.
 */
bool IsHuffmanCodeLoaded()
{
	return !HuffCode.empty();
}

/**
 * Checks whether the Keystore is loaded or not
 * @return True if the Keystore is loaded. Otherwise not.
 */
bool IsKeystoreLoaded()
{
	return !Store.IsEmpty();
}

/**
 * Checks whether the Wallet configuration is loaded or not
 * @return True if the Wallet configuration is loaded. Otherwise not.
 */
bool IsWalletLoaded()
{
	return Wallet.IsInit();
}

/**
 * Checks whether the Bitcoin daemon is running or not
 * @return True if the Bitcoin daemon is running. Otherwise not.
 */
bool IsBitcoinRunning()
{
	if(!IsWalletLoaded())
		return false;

	try
	{
		Wallet.getinfo();
	}
	catch(BitcoinException& e)
	{
		return false;
	}

	return true;
}


/** Unloads the configuration file. */
void UnloadConfigFile()
{
	Config.clear();
}

/** Unloads the Huffman code. */
void UnloadHuffmanCode()
{
	HuffCode.clear();
}

/** Unloads the Keypair map. */
void UnloadKeypairMap()
{
	KeyMap.clear();
}

/** Unloads the Keystore. */
void UnloadKeystore()
{
	Store.Clear();
}

/** Unloads the Wallet configuration. */
void UnloadWallet()
{
	Wallet = BitcoinAPI();
}

/**
 * Generates a random hex string of a specified length.
 * @param nChars Length of the hex string in characters.
 * @return Random hex string.
 */
string GenerateRandomHexString(unsigned int nChars)
{
	const char Charset[] = "0123456789ABCDEF";
	const size_t MaxIndex = (sizeof(Charset) - 2);
	string str;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, MaxIndex);

	for (unsigned int i = 0; i < nChars; i++)
	{
		str += Charset[dist(gen)];
	}

	return str;
}

/**
 * Generates a random binary vector of a specified length.
 * @param nBits Length of vector in bits.
 * @return Random binary vector.
 */
DataBits GenerateRandomBits(unsigned int nBits)
{
	DataBits data;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1);

	for (unsigned int i = 0; i < nBits; i++)
	{
		data.insert(data.end(), (bool)dist(gen));
	}

	return data;
}

/**
 * Generates a keypair map where the \p nBits public key suffix is controlled.
 * The map comprises keypairs with all combinations of the trailing \p nBits
 * of the public key.
 * @param nBits Number of controlled public key suffix bits.
 * @return Keypair map with controlled public key suffix.
 */
KeypairMap GenerateKeypairMap(unsigned int nBits)
{
	KeypairMap keyMap;
	CPubKey pubkey;

	DataBits key, buf;
	CKey val;

	while (keyMap.size() < pow(2, nBits))
	{
		val.MakeNewKey(true);
		pubkey = val.GetPubKey();

		buf = DataToBits(Data(pubkey.begin(), pubkey.end()));
		key = DataBits(buf.end() - nBits, buf.end());

		keyMap[key] = val;
	}

	return keyMap;
}

/**
 * Calculates the transaction fees in Satoshi for a given transaction.
 * @param Tx Transaction of which the fee is calculated.
 * @return Transaction fee in Satoshi.
 */
uint64_t TransactionFee(const CTransaction& Tx)
{
	const uint64_t FeeRate = std::stoul(Utilities::Config.at("TxFeeRate"));
	const unsigned int Size = Tx.GetSerializeSize(SER_DISK,1);

	return FeeRate * ceil(Size/1000.0);
}

/**
 * Calculates the transaction fee in Satoshi for a given transaction chain.
 * @param Chain Transaction chain of which the fee is calculated.
 * @return Transaction fee in Satoshi.
 */
uint64_t TransactionFee(const TransactionChain& Chain)
{
	uint64_t nFee = 0;
	for(TransactionChain::const_iterator it = Chain.begin(); it != Chain.end(); it++)
	{
		nFee += TransactionFee(*it);
	}

	return nFee;
}

/**
 * Calculates the minimum budget for a given number of bits of to be embedded data.
 * @param nBits Number of bits to be embedded.
 * @return Minimum budget in Satoshi.
 */
uint64_t MinimumBudget(unsigned int nBits)
{
	const uint64_t FeeRate = std::stoul(Utilities::Config.at("TxFeeRate"));
	uint64_t minBudget = 0;

	TransactionChain chain = BlockchainInterface::EmbedData(DataBits(nBits), (uint64_t)0, vector<COutPoint>(), CBitcoinAddress());
	minBudget += TransactionFee(chain);
	minBudget += 2*FeeRate;

	return minBudget;
}

/**
 * Converts a given regular transaction into a hex string.
 * @param Tx Transaction to be converted into a hex string.
 * @return Hex string of transaction.
 */
string TransactionToHexString(const CTransaction& Tx)
{
	CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
	string ret;

	ss << Tx;
	ret = HexStr(ss.begin(), ss.end());

	return ret;
}

/**
 * Converts a given hex string into a regular transaction.
 * @param HexString Hex string of transaction.
 * @return Converted transaction.
 */
CTransaction HexStringToTransaction(const string& HexString)
{
	vector<unsigned char> txData(ParseHex(HexString));
	CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION);

	CTransaction tx;
	ssData >> tx;

	return tx;
}

}
