/**
 * Serializatin.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "Main.cpp"

#include "Serialization.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;
using namespace Serialization;


BOOST_AUTO_TEST_SUITE(SerializationTests)

BOOST_AUTO_TEST_CASE(HuffmanMapSerialization)
{
	HuffCodeMap codesA;
	HuffCodeMap codesB;

	codesA.insert(HuffCodeMap::value_type('a', std::vector<bool>{0,1,0,0}));
	codesA.insert(HuffCodeMap::value_type('b', std::vector<bool>{1,0,1,1}));

	SerializeHuffmanCode(codesA, "HuffCode.tmp");

	fs::path mapPath("HuffCode.tmp");
	BOOST_REQUIRE(fs::exists(mapPath));

	codesB = DeserializeHuffmanCode("HuffCode.tmp");
	BOOST_REQUIRE(fs::remove(mapPath));

	BOOST_REQUIRE(codesA == codesB);
}

BOOST_AUTO_TEST_CASE(KeypairMapSerialization)
{
	KeypairMap keymapA;
	KeypairMap keymapB;

	keymapA = Utilities::GenerateKeypairMap(8);
	SerializeKeypairMap(keymapA, "KeyMap.tmp");

	fs::path mapPath("KeyMap.tmp");
	BOOST_REQUIRE(fs::exists(mapPath));

	keymapB = DeserializeKeypairMap("KeyMap.tmp");
	BOOST_REQUIRE(fs::remove(mapPath));

	BOOST_REQUIRE(keymapA == keymapB);
}

BOOST_AUTO_TEST_CASE(ConfigDeserialization)
{
	ConfigMap config;

	fs::path configPath("config/bms.conf");
	BOOST_REQUIRE(fs::exists(configPath));

	config = DeserializeConfigMap("config/bms.conf");
	BOOST_REQUIRE(config.size() > 0);
}

BOOST_AUTO_TEST_SUITE_END()
