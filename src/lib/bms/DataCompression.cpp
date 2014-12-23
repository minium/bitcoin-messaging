/**
 * DataCompression.cpp
 *
 * Module with auxiliary functions for the Huffman coding scheme.
 * It comprises a function to compute the Huffman coding itself as
 * well as a compression and decompression function.
 *
 * @author Krzysztof Okupski
 * @version 1.0
 */

#include <algorithm>
#include <queue>
#include <iterator>
#include <iostream>

#include "DataCompression.h"
#include "Utilities.h"

using std::map;
using std::vector;
using std::string;
using std::transform;
using std::remove_if;

namespace HuffmanCoding
{

class BaseNode
{
public:
	const int frequency;

	BaseNode(int freq) : frequency(freq) { }
	virtual ~BaseNode() { }
};

class InternalNode: public BaseNode
{
public:
	BaseNode *left;
	BaseNode *right;

	InternalNode(BaseNode *node1, BaseNode *node2) :
			BaseNode(node1->frequency + node2->frequency)
	{
		left = node1;
		right = node2;
	};

	~InternalNode()
	{
		delete left;
		delete right;
	}
};

class LeafNode: public BaseNode
{
public:
	const char character;

	LeafNode(char sym, int freq) :
			BaseNode(freq), character(sym)
	{
	}
};

struct NodeCmp
{
	bool operator()(const BaseNode* left, const BaseNode* right) const
	{
		return left->frequency > right->frequency;
	}
};

bool IsCharInvalid(const char symbol)
{
	// Newline
	if (0x0A == symbol)
	{
		return false;
	}
	// Various signs
	else if (0x20 <= symbol && symbol <= 0x2F)
	{
		return false;
	}
	// Digits
	else if (0x30 <= symbol && symbol <= 0x39)
	{
		return false;
	}
	// More various signs
	else if (0x3A <= symbol && symbol <= 0x40)
	{
		return false;
	}
	// Lower case latin alphabet
	else if (0x61 <= symbol && symbol <= 0x7A)
	{
		return false;
	}
	// Upper case latin alphabet
	else if (0x41 <= symbol && symbol <= 0x5A)
	{
		return false;
	}
	// Anything else
	else
	{
		return true;
	}

}

/**
 * Removes characters that do not adhere to a restricted alphabet.
 * @param text String to be transformed.
 */
void TransformCharDomain(string& text)
{
	text.erase(remove_if(text.begin(), text.end(), &IsCharInvalid), text.end());
}

/**
 * Computes the frequency distribution of characters in a given string.
 * @param Text Source text from which the frequency distribution is to be computed.
 * @result Frequency map of the characters in the text.
 */
FreqMap ComputeFrequencies(const string& Text)
{
	FreqMap frequencies;

	for (string::const_iterator it = Text.begin(); it != Text.end(); it++)
	{
		frequencies[*it]++;
	}

	return frequencies;
}

BaseNode* BuildTree(const FreqMap& Frequencies)
{
	std::priority_queue<BaseNode*, vector<BaseNode*>, NodeCmp> tree;

	for (FreqMap::const_iterator it = Frequencies.begin();
			it != Frequencies.end(); it++)
	{
		tree.push(new LeafNode(it->first, it->second));
	}

	while (tree.size() > 1)
	{
		BaseNode *childLeft = tree.top();
		tree.pop();

		BaseNode *childRight = tree.top();
		tree.pop();

		BaseNode *parent = new InternalNode(childLeft, childRight);

		tree.push(parent);
	}

	return tree.top();
}

void GenerateCodes(const BaseNode *Node, const HuffCode& Prefix, HuffCodeMap& codes)
{
	if (const LeafNode* lf = dynamic_cast<const LeafNode*>(Node))
	{
		codes.insert(HuffCodeMap::value_type(lf->character, Prefix));
	}
	else if (const InternalNode* in = dynamic_cast<const InternalNode*>(Node))
	{

		HuffCode leftPrefix = Prefix;
		leftPrefix.push_back(false);
		GenerateCodes(in->left, leftPrefix, codes);

		HuffCode rightPrefix = Prefix;
		rightPrefix.push_back(true);
		GenerateCodes(in->right, rightPrefix, codes);
	}
}

/**
 * Generates the Huffman coding from a given frequency distribution.
 * @param Frequencies Frequency distribution of characters.
 * @result Corresponding Huffman coding of characters.
 */
HuffCodeMap GenerateCodes(const FreqMap& Frequencies)
{
	BaseNode* root = BuildTree(Frequencies);
	HuffCodeMap codes;

	GenerateCodes(root, HuffCode(), codes);
	delete root;

	return codes;
}

/**
 * Compresses a vector of characters using Huffman coding.
 * @param Data To be compressed data.
 * @param Codes Huffman coding of characters.
 * @result Compressed vector of characters.
 */
DataBits Compress(const Data& Data, const HuffCodeMap& Codes)
{
	DataBits compData;

	for (Data::const_iterator it = Data.begin(); it != Data.end(); it++)
	{
		compData.insert(compData.end(), Codes.left.at(*it).begin(), Codes.left.at(*it).end());
	}

	compData.insert(compData.end(), Codes.left.at((char)EoF).begin(), Codes.left.at((char)EoF).end());

	return compData;
}

/**
 * Decompresses a binary vector using Huffman coding.
 * @param Bits To be decompressed data.
 * @param Codes Huffman coding of characters.
 * @result Decompressed vector of characters.
 */
Data Decompress(const DataBits& Bits, const HuffCodeMap& Codes)
{
	Data decompData;

	DataBits ch;
	for (DataBits::const_iterator it = Bits.begin(); it != Bits.end(); it++)
	{
		ch.push_back(*it);

		HuffCodeMap::right_const_iterator it2 = Codes.right.find(ch);
		if (it2 != Codes.right.end())
		{
			unsigned char tmp = it2->second;
			if (tmp == EoF)
				break;

			decompData.push_back(tmp);
			ch.clear();
		}
	}

	return decompData;
}

}
