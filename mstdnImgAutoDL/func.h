#pragma once

using namespace std;

string md5Hash(vector<unsigned char>& file);

string enc(string str);
string dec(string str);

struct fileData {
	fileData(string hash, string tag, string url, string name, bool nsfw);
	string hash;
	string tag;
	string url;
	string name;
	bool nsfw;
};