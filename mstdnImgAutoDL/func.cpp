#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/cryptlib.h>
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

#include <cryptopp/rijndael.h>
#include <cryptopp/modes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>

#include <string>
#include <iostream>

#include "func.h"

using namespace CryptoPP;
using namespace std;

string md5Hash(vector<unsigned char>& file) {
	string hashStr;
	Weak::MD5 hash;

	hash.Update((const CryptoPP::byte*)&file[0], file.size());
	hashStr.resize(hash.DigestSize());
	hash.Final((CryptoPP::byte*)&hashStr[0]);

	string hashOut;

	HexEncoder encoder(new StringSink(hashOut));
	StringSource ss(hashStr, true, new Redirector(encoder));

	return hashOut;
}

string enc(string str) {
	string keystr = "B4D09DDB478726709F86F059D0A95109";
	string ivstr = "6E3294F424509BE6178D16E8D9890DBE";
	string keybin, ivbin;

	StringSource sk(keystr, true, new HexDecoder(new StringSink(keybin)));
	StringSource sv(ivstr, true, new HexDecoder(new StringSink(ivbin)));

	string cipher;
	string cipherstr;

	CBC_Mode<AES>::Encryption enc;
	enc.SetKeyWithIV((CryptoPP::byte*)&keybin[0], keybin.size(), (CryptoPP::byte*)&ivbin[0]);

	StringSource sc(str, true, new StreamTransformationFilter(enc, new StringSink(cipher)));
	StringSource scs(cipher, true, new HexEncoder(new StringSink(cipherstr)));

	return cipherstr;
}

string dec(string str) {
	string keystr = "B4D09DDB478726709F86F059D0A95109";
	string ivstr = "6E3294F424509BE6178D16E8D9890DBE";
	string keybin, ivbin;

	StringSource sk(keystr, true, new HexDecoder(new StringSink(keybin)));
	StringSource sv(ivstr, true, new HexDecoder(new StringSink(ivbin)));

	CBC_Mode<AES>::Decryption dec;
	dec.SetKeyWithIV((CryptoPP::byte*)&keybin[0], keybin.size(), (CryptoPP::byte*)&ivbin[0]);

	string cipher;
	StringSource scse(str, true, new HexDecoder(new StringSink(cipher)));

	string orig;
	StringSource ss(cipher, true, new StreamTransformationFilter(dec, new StringSink(orig)));

	return orig;
}

fileData::fileData(string hash, string tag, string url, string name, bool nsfw) {
	this->hash = hash;
	this->tag = tag;
	this->url = url;
	this->name = name;
	this->nsfw = nsfw;
}