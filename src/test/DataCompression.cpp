/**
 * DataCompression.cpp
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "Main.cpp"

#include "DataCompression.h"
#include "Types.h"

using namespace HuffmanCoding;


BOOST_AUTO_TEST_SUITE(DataCompressionTests)

BOOST_AUTO_TEST_CASE(CodeGeneration)
{
	std::string str = "go go gophers";
	FreqMap frequencies;
	HuffCodeMap codes;

	frequencies = ComputeFrequencies(str);
	codes = GenerateCodes(frequencies);

	BOOST_REQUIRE(codes.size() == 8);
	BOOST_REQUIRE(codes.left.at('g').size() == 2);
	BOOST_REQUIRE(codes.left.at('e').size() == 4);
}

BOOST_AUTO_TEST_CASE(DataCompression)
{
	std::string str = "This is some arbitrary TestdataX";
	Data originalData;
	DataBits compData;
	Data recoveredData;

	TransformCharDomain(str);
	originalData = Data(str.begin(), str.end());

	compData = Compress(originalData, Utilities::HuffCode);
	recoveredData = Decompress(compData, Utilities::HuffCode);

	BOOST_REQUIRE(originalData == recoveredData);
}

//BOOST_AUTO_TEST_CASE(GenerateCodemap)
//{
//	std::vector<boost::filesystem::path> files;
//	boost::filesystem::path root("/home/minium/Desktop/OANC-GrAF/");
//
//	boost::filesystem::directory_iterator it(root);
//	boost::filesystem::directory_iterator end;
//
//
//	while(it != end)
//	{
//		files.push_back((*it));
//		it++;
//	}
//
//	std::cout << "Files: " << files.size() << std::endl;
//	std::stringstream sstr;
//	for(std::vector<boost::filesystem::path>::const_iterator it = files.begin(); it != files.end(); it++)
//	{
//		boost::filesystem::ifstream stream(*it, ios::in);
//		sstr << stream.rdbuf();
//		stream.close();
//	}
//
//
//	std::string str = sstr.str();
//	HuffmanCoding::TransformCharDomain(str);
//	str += std::string(files.size(), EoF);
//
//	std::cout << "Text size: " << str.size() << std::endl;
//
//	FreqMap freqs = HuffmanCoding::ComputeFrequencies(str);
//	HuffCodeMap huffcode = HuffmanCoding::GenerateCodes(freqs);
//
//	Serialization::SerializeHuffmanCode(huffcode, "huffcode3.map");
//
//
//}

//BOOST_AUTO_TEST_CASE(CompressionRatio)
//{
//	std::vector<boost::filesystem::path> files;
//	boost::filesystem::path root("/home/minium/Desktop/OANC-GrAF/");
//
//	boost::filesystem::directory_iterator it(root);
//	boost::filesystem::directory_iterator end;
//
//
//	while(it != end)
//	{
//		files.push_back((*it));
//		it++;
//	}
//
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::shuffle(files.begin(), files.end(), gen);
//
//	std::vector<boost::filesystem::path> firstVec(files.begin(), files.begin()+8000);
//	std::vector<boost::filesystem::path> secondVec(files.begin()+8000,files.begin()+8806);
//
//	std::cout << "Files: " << firstVec.size() << std::endl;
//	std::stringstream sstr;
//	for(std::vector<boost::filesystem::path>::const_iterator it = firstVec.begin(); it != firstVec.end(); it++)
//	{
//		boost::filesystem::ifstream stream(*it, ios::in);
//		sstr << stream.rdbuf();
//		stream.close();
//	}
//
//	std::string str = sstr.str();
//	sstr.str(std::string());
//	TransformCharDomain(str);
//	str += std::string(firstVec.size(), EoF);
//
//	std::cout << "Text size: " << str.size() << std::endl;
//
//	FreqMap freqs = ComputeFrequencies(str);
//	HuffCodeMap huffcode = GenerateCodes(freqs);
//
//	std::cout << "=== Starting compression ===" << std::endl;
//	double compressionRatio = 0;
//	for(std::vector<boost::filesystem::path>::const_iterator it = secondVec.begin(); it != secondVec.end(); it++)
//	{
//		boost::filesystem::ifstream stream(*it, ios::in);
//		sstr << stream.rdbuf();
//		stream.close();
//
//		std::string text = sstr.str();
//		TransformCharDomain(text);
//		DataBits bits = Compress(Data(text.begin(), text.end()), huffcode);
//
//		BOOST_REQUIRE(Data(text.begin(), text.end()) == Decompress(bits, huffcode));
//		compressionRatio += (double)text.size() / ((double)bits.size() / 8.0);
//
//		sstr.clear();
//		sstr.str(std::string());
//	}
//
//	std::cout << "Compression ratio: " << compressionRatio / secondVec.size() << std::endl;
//
//}

BOOST_AUTO_TEST_SUITE_END()
