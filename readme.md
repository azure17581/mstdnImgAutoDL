# mstdnImgAutoDL
マストドンの [pawoo.net](https://pawoo.net/about) のアカウントで「お気に入り」したトゥートに添付された画像や動画をダウンロードする。  
同時に画像管理ソフト [at_picture](http://artisticimitation.web.fc2.com/adbtest/) に対応した CSV を出力する。これにはダウンロードしたファイルのパス，MD5ハッシュ値，ファイルの添付されていた投稿のリンク，ユーザーID が含まれる。これを at_picture で読ませることで，このソフトウェアが保存した画像を，ユーザーIDをタグとして管理できるようになる。  
さらに，コメントとしてその画像が添付されていた投稿の URL も DB に追加されているため，それを踏んでブラウザで該当の投稿を開くことができる。

## 開発環境
- Windows 10 Pro 21H2
- Visual Studio 2019

## ビルド方法
- まず [vcpkg](https://vcpkg.io/en/getting-started.html) を導入する。
- Visual Studio と紐づける。
```
vcpkg integrate install
```
- [cpprestsdk](https://github.com/Microsoft/cpprestsdk)，[Crypto++](https://cryptopp.com/) を vcpkg で導入する。

64bit バイナリを生成する場合
```
vcpkg install cpprestsdk:x64-windows
vcpkg install cryptopp:x64-windows
```
32bit バイナリを生成する場合
```
vcpkg install cpprestsdk
vcpkg install cryptopp
```
- プロジェクトファイルを開くとこれらのライブラリを `#include` が参照できるようになっているはずなので，`ビルド` する。

## 使い方
`mstdnImgAutoDL.exe` を実行。
### 初回起動時
アクセストークンの入力を求められる。
- pawoo の `ユーザー設定` -> `開発` を開く。
- `新規アプリ` を選択し，適当な名前を付けて `アクセス権` で `read:favourites` のみにチェックを入れて `送信`
- 作成したアプリ名を選択し，`アクセストークン` の欄の文字列を `mstdnImgAutoDL.exe` に戻って貼り付け。

トークンの情報は `token.txt` に一応は暗号化されて保存される。  
以下，`2回目以降の起動時` と同様。

### 2回目以降の起動時
- 前回の起動後に新たに「お気に入り」した投稿の添付ファイルが `DL` フォルダに保存される。
- 40件投稿をさかのぼるごとに，続行するか訊かれる。続行しない場合は `n` ，続行する場合はそれ以外の何らかの文字列を入力。
- 保存処理が終了すると，[at_picture](http://artisticimitation.web.fc2.com/adbtest/) に対応した CSV `tags.csv` が出力される。既に存在する場合は，ファイルに情報が追記される。
- 添付ファイルを保存した投稿の直近10件の URL が `urlData.txt` に保存される。

### トークンを変更する場合
単に `token.txt` を削除してください。その後は `初回起動時` と同様。