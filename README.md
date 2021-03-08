# ESP32
ESP-WROOM-32というWifiとBluetooth搭載のマイコンを使用したプログラムのリポジトリ
# フォルダ構成
- example  :ESP32のプログラムの例
- hardware :対向二輪ロボットやアームの設計データ、ESP32を使用した基板のデータ
- image    :写真や動画データなど
- library  :projectsのプログラムで使用しているライブラリー
- projects :対向二輪ロボットのプログラムやアームのプログラムなど
  - ROBOCON:NHK高専ロボコン時に作成したproject
# 開発に使用したソフト
- ソフトウェア開発
  - Visual Studio Code
  - PlatformIO
- ハードウェア開発
  - 基板製作:Autodesk Eagle
  - 筐体制作:Autodesk Fusion360,Inventor
# Opposing two wheels
制御練習用の対向二輪の足回り。専用のコントローラーまたはスマホからコントロールできる。

![設計データ](/image/二輪足回り.png)
![コントローラー](/image/コントローラー.jpg)
- ライントレース
![ライントレース](/image/ライントレース.gif)
- 台形制御
![台形制御](/image/台形制御.gif)
# Mini arm robot
3Dプリンターとサーボモーター（sg90）で作成した。
![設計データ](/image/アーム設計.jpg)
![アーム](/image/アーム実物.jpg)
![アーム動作](/image/アーム動作.gif)
