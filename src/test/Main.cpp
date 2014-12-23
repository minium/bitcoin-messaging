/**
 * Main.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_MODULE Main Tests
#define BOOST_TEST_DYN_LINK

#include "Utilities.h"

#include <stdexcept>
#include <boost/test/unit_test.hpp>

struct MyFixture
{
	MyFixture()
	{
        try
        {
    		Utilities::LoadConfigFile();
    		Utilities::LoadHuffmanCode();
    		Utilities::LoadKeypairMap();
    		Utilities::LoadKeystore();
    		Utilities::LoadWallet();

            if(!Utilities::IsBitcoinRunning()){
                throw std::runtime_error("Bitcoin is not running!");
            }
        }
        catch(std::runtime_error& e)
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

	~MyFixture()
	{
		Utilities::UnloadConfigFile();
		Utilities::UnloadHuffmanCode();
		Utilities::UnloadKeypairMap();
		Utilities::UnloadKeystore();
		Utilities::UnloadWallet();
	}
};

BOOST_GLOBAL_FIXTURE(MyFixture);
