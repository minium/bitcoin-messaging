/**
 * Serialization.cpp
 *
 * Serialization module for saving and loading of the
 * Huffman code, the keypair map and the configuration file.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include "Serialization.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>

using std::string;
using std::vector;
using std::map;
using std::set;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace Serialization
{

/**
 * Serializes a given Huffman code to a specified location.
 * @param Codes To be serialized Huffman code mapping.
 * @param Path Path to where the mapping is to be serialized.
 * */
void SerializeHuffmanCode(const HuffCodeMap& Codes, const string& Path)
{
	fs::ofstream ofs(Path);
    if (!ofs.good())
    {
        string err;
        err += "[SerializeHuffmanCode] Failed to open output stream";
        err += "\nPath: ";
        err += Path;    	
        throw std::runtime_error(err);
    }
	
	boost::archive::text_oarchive oa(ofs);

	oa << const_cast<const HuffCodeMap&>(Codes);

	for (HuffCodeMap::left_const_iterator it = Codes.left.begin();
			it != Codes.left.end(); it++)
	{
		oa << it;
	}

	for (HuffCodeMap::right_const_iterator it = Codes.right.begin();
			it != Codes.right.end(); it++)
	{
		oa << it;
	}

	ofs.close();
}

/**
 * Deserializes a Huffman code map from a specified location.
 * @param Path Path to where the Huffman code mapping is stored.
 * @result The stored Huffman code mapping.
 */
HuffCodeMap DeserializeHuffmanCode(const string& Path)
{
    fs::ifstream ifs(Path);
	if(!ifs.good())
	{
        string err;
        err += "[DeserializeHuffmanCode] Failed to open input stream";
        err += "\nPath: ";
        err += Path;
    	throw std::runtime_error(err);
	}

	HuffCodeMap codes;
	boost::archive::text_iarchive ia(ifs);

	ia >> codes;
	ifs.close();

	return codes;
}

/**
 * Serializes a given keypair map to a specified location.
 * @param Keymap To be serialized keypair map.
 * @param Path Path to where the mapping is to be serialized.
 * */
void SerializeKeypairMap(const KeypairMap& Keymap, const string& Path)
{
	fs::ofstream ofs(Path);
	if(!ofs.good())
	{
        string err;
        err += "[SerializeKeypairMap] Failed to open output stream";
        err += "\nPath: ";
        err += Path;
    	throw std::runtime_error(err);
	}

	map<DataBits, vector<unsigned char>> tmpMap;
	boost::archive::text_oarchive oa(ofs);

	for (KeypairMap::const_iterator it = Keymap.begin(); it != Keymap.end(); it++)
	{
		bool compression = it->second.IsCompressed();
		vector<unsigned char> key = vector<unsigned char>(it->second.begin(), it->second.end());
		vector<unsigned char> element;

		element.insert(element.begin(), (unsigned char)compression);
		element.insert(element.end(), key.begin(), key.end());

		tmpMap[it->first] = element;
	}

	oa << tmpMap;
	ofs.close();
}

/**
 * Deserializes a keypair map from a specified location.
 * @param Path Path to where the mapping is to be serialized.
 * @result The stored keypair mapping.
 */
KeypairMap DeserializeKeypairMap(const string& Path)
{
	fs::ifstream ifs(Path);
	if(!ifs.good())
	{
        string err;
        err += "[DeserializeKeypairMap] Failed to open input stream";
        err += "\nPath: ";
        err += Path;
    	throw std::runtime_error(err);
	}

	KeypairMap keymap;
	boost::archive::text_iarchive ia(ifs);

	map<DataBits, vector<unsigned char>> tmpMap;
	ia >> tmpMap;
	ifs.close();

	for (map<DataBits, vector<unsigned char>>::const_iterator it = tmpMap.begin(); it != tmpMap.end(); it++)
	{
		vector<unsigned char> element = it->second;

		bool compression = (bool)element[0];
		vector<unsigned char> key = vector<unsigned char>(element.begin()+1, element.end());

		keymap[it->first] = CKey();
		keymap[it->first].Set(key.begin(), key.end(), compression);
	}

	return keymap;
}

/**
 * Serializes the state parameters of a configuration to a specified location.
 * @param Config The configuration file comprising the state parameters.
 * @param Path Path to where the configuration file is stored.
 */
void SerializeState(const ConfigMap& Config, const string& Path)
{
	const string FirstTx = Config.at("State.FirstTx");
	const string LastTx = Config.at("State.LastTx");

	fs::ifstream ifs(Path);
	stringstream sstr;
	string str;

	if (!ifs.good())
    {
        string err;
        err += "[SerializeState] Failed to open input stream";
        err += "\nPath: ";
        err += Path;
		throw std::runtime_error(err);
    }

	sstr << ifs.rdbuf();
	ifs.close();

	str = string(sstr.str());

	size_t pos = str.find("State.FirstTx") + 14;
	str.replace(pos, 64, FirstTx);

	pos = str.find("State.LastTx") + 13;
	str.replace(pos, 64, LastTx);


	fs::ofstream ofs(Path);
	if (!ofs.good())
    {
        string err;
        err += "[SerializeState] Failed to open output stream";
        err += "\nPath: ";
        err += Path;
        throw std::runtime_error(err);
    }

	ofs << str;
	ofs.close();
}

/**
 * Deserializes the configuration file from a specified location.
 * @param Path Path to where the configuration file is stored.
 * @result The stored configuration map.
 */
ConfigMap DeserializeConfigMap(const string& Path)
{
	fs::ifstream ifs(Path);
	if (!ifs.good())
    {
        string err;
        err += "[DeserializeConfigMap] Failed to open input stream";
        err += "\nPath: ";
        err += Path;
        throw std::runtime_error(err);
    }

	ConfigMap config; 
    set<string> setOptions;
    setOptions.insert("*");

    for (po::detail::config_file_iterator it(ifs, setOptions), end; it != end; ++it)
    {
    	std::string key = it->string_key;
    	if (config.count(key) == 0)
        {
            config[key] = it->value[0];
        }
    }

    ifs.close();
    return config;
}

}
