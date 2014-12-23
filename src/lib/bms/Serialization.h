/**
 * Serialization.h
 *
 * Serialization module for saving and loading of the
 * Huffman code, the keypair map and the configuration file.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#ifndef BMS_SERIALIZATION_H
#define BMS_SERIALIZATION_H

#include "Utilities.h"

namespace Serialization
{
	void SerializeHuffmanCode(const HuffCodeMap& Code, const std::string& Path);
	HuffCodeMap DeserializeHuffmanCode(const std::string& Path);

	void SerializeKeypairMap(const KeypairMap& Keymap, const std::string& Path);
	KeypairMap DeserializeKeypairMap(const std::string& Path);

	void SerializeState(const ConfigMap& Config, const std::string& Path);
	ConfigMap DeserializeConfigMap(const std::string& Path);
};

#endif
