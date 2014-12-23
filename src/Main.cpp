/**
 * Main executable.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include "BitcoinException.h"
#include "lib/bms/BlockchainInterface.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>
#include <iomanip>

using std::string;
using std::vector;

namespace fs = boost::filesystem;

using namespace Utilities;
using namespace BlockchainInterface;


int main()
{
    try
    {
		Utilities::LoadConfigFile();
		Utilities::LoadHuffmanCode();
		Utilities::LoadKeypairMap();
		Utilities::LoadKeystore();
		Utilities::LoadWallet();

        if(!Utilities::IsBitcoinRunning()){
            throw std::runtime_error("[Bitcoind] Bitcoin daemon is not running!");
        }


	    /* Select operation mode */
	    char mode = 'X';
	    std::cout << "Would you like to write to (W) or read from (R) the blockchain?" << std::endl;

	    do{
		    std::cout << "Choice: ";
		    std::cin >> mode;

		    if(!(mode == 'W' || mode == 'R'))
		    {
			    std::cout << "Your choice was incorrect, please try again." << std::endl;
		    }
	    }while(!(mode == 'W' || mode == 'R'));
	    std::cout << std::endl;

	    switch(mode)
	    {
		    case 'W':
			    {
				    /* Read the file */
				    string text;
				    string path;
				    std::cout << "Please enter the full path to the text file you wish to send to the blockchain:" << std::endl;
				    std::cin >> path;

				    fs::path filePath(path);
				    if(!fs::exists(filePath))
				    {
					    throw std::runtime_error("The entered file does not exist");
				    }

                    if(!fs::is_regular_file(filePath))
                    {
	                    throw std::runtime_error("The named path does not refer to a regular file");
                    }

				    std::stringstream sstr;
				    fs::ifstream fileStream(filePath, ios::in);
				    sstr << fileStream.rdbuf();
				    text = sstr.str();


				    /* Compress the data */
				    HuffmanCoding::TransformCharDomain(text);
				    std::cout << std::endl << "Your text has been converted into:" << std::endl;
				    std::cout << text << std::endl;
				    Data originalData = Data(text.begin(), text.end());
				    DataBits compressedData = HuffmanCoding::Compress(originalData, Utilities::HuffCode);

                    std::cout << "[INFO] Original data size: " << originalData.size() << " bytes" << std::endl;
                    std::cout << "[INFO] Compressed data size: " << compressedData.size() / 8.0 << " bytes" << std::endl; 

				    /* Embed the data into a transaction chain */
				    InitInputs params = SelectInputs(MinimumBudget(compressedData.size()));
				    CBitcoinAddress addr = SelectAddress();
				    TransactionChain chain = EmbedData(compressedData, params.budget, params.utxo, addr);

				    /* Enter wallet password to authorize the transaction */
				    uint64_t nFees = Utilities::TransactionFee(chain);
				    std::cout << "[INFO] The data will be embedded in " << chain.size() << " transactions" << std::endl;
				    std::cout << "[INFO] The embedding of the data will cost " << nFees << " Satoshi" << std::endl;

				    string password;
				    std::cout << "If you wish to proceed, please enter your wallet password: ";
				    std::cin >> password;
				    std::cout << std::endl;

                    try
                    {
    				    Utilities::Wallet.walletpassphrase(password, 60);
                    }
                    catch(BitcoinException& e)
                    {
                        string err;
                        err += "[Wallet] Error while unlocking wallet";
                        err += "\nReason: ";
                        err += e.getMessage();
                        throw std::runtime_error(err);
                    }
				    AuthorizeTransactions(chain);
				  
				    Utilities::Wallet.walletlock();
				    SendTransactions(chain);

				    std::cout << "[INFO] The transactions have been sent successfully!" << std::endl;
				    std::cout << "[INFO] First transaction ID: " << chain.front().GetHash().GetHex() << std::endl;
				    std::cout << "[INFO] Last transaction ID: " << chain.back().GetHash().GetHex() << std::endl;

				    UpdateChainstate(chain.front().GetHash(), chain.back().GetHash());
                    std::cout << "[INFO] The chain state configuration has been updated successfully" << std::endl;
				    break;
			    }
		    case 'R':
			    {
				    string input;
				    uint256 BeginTx, EndTx;

				    std::cout << "First transaction ID: ";
				    std::cin >> input;
				    BeginTx = uint256(input);

				    std::cout << "Last transaction ID: ";
				    std::cin >> input;
				    std::cout << std::endl;
				    EndTx = uint256(input);

				    vector<TransactionChain> chain = ReadTransactions(BeginTx, EndTx);
                    std::cout << "[INFO] Successfully extracted " << chain.size() << " message(s)!" << std::endl;

				    for(vector<TransactionChain>::const_iterator it = chain.begin(); it != chain.end(); it++)
				    {
					    DataBits compressedData = ExtractData(*it);
					    Data uncompressedData = HuffmanCoding::Decompress(compressedData, Utilities::HuffCode);

					    std::cout << "[INFO] Message (" << uncompressedData.size() << " characters)" << std::endl;
					    std::cout << string(uncompressedData.begin(), uncompressedData.end()) << std::endl;
				    }
				    break;
			    }
	    }

	    UnloadConfigFile();
	    UnloadHuffmanCode();
	    UnloadKeypairMap();
	    UnloadKeystore();
	    UnloadWallet();

    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;

        Utilities::UnloadConfigFile();
	    Utilities::UnloadHuffmanCode();
	    Utilities::UnloadKeypairMap();
	    Utilities::UnloadKeystore();
	    Utilities::UnloadWallet();

        exit(1);
    }

}
