#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <regex>

#include "func.h"

using namespace utility;
using namespace utility::conversions;
using namespace web;
using namespace web::http;
using namespace web::http::client;

using namespace std;
using namespace std::filesystem;

typedef unsigned char uchar;

int main() {
	//カレントパス
	string curPath = current_path().string();
	filesystem::create_directories("DL\\NSFW");

	//DLしたURL
	vector<string> arrURL;

	//画像情報CSV
	vector<fileData> fileList;
	
	//アクセストークン
	string_t token;

	//token.txt からトークン読込
	if (exists("token.txt")) {
		ifstream iftoken("token.txt");
		string encToken;
		iftoken >> encToken;

		token = to_string_t(dec(encToken));
	} else {
		cout << "アクセストークンを入力してください" << endl;
		string tok;
		cin >> tok;
		tok = "Bearer " + tok;
		string encToken = enc(tok);
		ofstream oftoken("token.txt");
		oftoken << encToken << endl;
		oftoken.close();

		token = to_string_t(tok);
	}

	string_t serverURI = L"https://pawoo.net/api/v1/favourites?limit=40";

	//既にDLしたリスト
	vector<string> DLlist;
	ifstream listread("urlData.txt");

	while (listread) {
		string tmpURL;
		listread >> tmpURL;
		DLlist.push_back(tmpURL);
	}
	listread.close();

	int dupCount = 0;

	while (true) {
		http_client reader(serverURI);
		http_request request(methods::GET);
		request.headers().add(L"Authorization", token);

		http_response response = reader.request(request).get();
		json::value resJson = response.extract_json().get();

		if (resJson.has_field(L"error")) {
			cout << "アクセストークンが無効です。token.txt を削除し，再起動してください" << endl;

			string t;
			getline(std::cin, t);

			return 0;
		}

		json::array arrData = resJson.as_array();
		//JSON解釈
		for (json::value valTmp : arrData) {
			bool dup = false;
			string postURL = to_utf8string(valTmp.at(L"url").as_string());

			for (string tmpList : DLlist) {
				if (tmpList == postURL) {
					dupCount++;
					dup = true;
				}
			}

			if (!dup) {
				arrURL.push_back(postURL);

				string userID = to_utf8string(valTmp.at(L"account").at(L"acct").as_string());

				bool nsfw = valTmp.at(L"sensitive").as_bool();

				//添付ファイル保存
				if (valTmp.has_array_field(L"media_attachments")) {
					json::array arrMedia = valTmp.at(L"media_attachments").as_array();
					for (json::value mediaTmp : arrMedia) {
						string_t mediaURL = mediaTmp.at(L"url").as_string();

						string mediaName;
						if (nsfw) {
							mediaName = "DL\\NSFW\\" + path(mediaURL).filename().string();
						} else {
							mediaName = "DL\\" + path(mediaURL).filename().string();
						}

						cout << mediaName << endl;

						http_client mediaDL(mediaURL);
						http_response resMedia = mediaDL.request(methods::GET).get();
						vector<uchar> mediaBin = resMedia.extract_vector().get();

						//保存
						std::ofstream fst(mediaName, ios_base::binary);
						for (uchar bin : mediaBin) {
							fst << bin;
						}
						fst.close();

						//ハッシュチェック
						string fileHash = md5Hash(mediaBin);

						//画像情報CSVデータ
						fileList.push_back(fileData(fileHash, userID, postURL, mediaName, nsfw));
					}
				}
			} else {
				if (dupCount > 2) {
					break;
				}
			}
		}

		if (dupCount > 2) {
			break;
		}

		cout << "次の40件をDLしますか? ( /n)" << endl;
		string ans;
		cin >> ans;

		if (ans == "n") {
			break;
		}

		string nextURI = to_utf8string(response.headers()[L"link"]);
		regex rex(R"(<.*>; rel="next")");
		smatch result;

		if (regex_search(nextURI, result, rex)) {
			nextURI = result[0];
			rex = R"(https.*=\d+)";
			if (regex_search(nextURI, result, rex)) {
				nextURI = result[0];
				serverURI = to_string_t(nextURI);
			} else {
				break;
			}
		} else {
			break;
		}
	}

	//終了処理
	if (arrURL.size() > 0) {
		//画像情報CSVを保存
		string csvName = "tags.csv";
		bool csvExist = exists(csvName);

		ofstream csvSt(csvName, ios_base::app);
		//存在しなかったら初期化
		if (!csvExist) {
			csvSt << R"("画像情報CSV")" << endl;
		}

		for (size_t i = 0; i < fileList.size(); i++) {
			csvSt << R"(")" << curPath << "\\" << fileList[i].name << R"(",")" << fileList[i].hash << R"(","source : )" << fileList[i].url << R"(",")" << fileList[i].tag << R"(")";
			if (fileList[i].nsfw) {
				csvSt << R"(,"NSFW")";
			}
			csvSt << endl;
		}
		csvSt.close();

		//最新のURL10件の保存
		ofstream urlDataSt("urlData.txt");
		for (size_t i = 0; (i < 10) && (i < arrURL.size()); i++) {
			urlDataSt << arrURL[i] << endl;
		}
		if (arrURL.size() < 10) {
			for (size_t i = 0; (i < 10 - arrURL.size()) && (i < DLlist.size()); i++) {
				urlDataSt << DLlist[i] << endl;
			}
		}
		urlDataSt.close();
	}

	cout << "\n完了" << endl;
	string t;
	getline(std::cin, t);
	
	return 0;
}