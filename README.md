# Multi-WiFiS4A

### Install
* visualstudio 伺服器編輯器
https://www.visualstudio.com/zh-hant/downloads/?rr=https%3A%2F%2Fwww.google.com.tw%2F


* Arduino編輯器
https://www.arduino.cc/en/Main/Donate

* 燒錄wifi模組D1 mini需要的lib:

https://drive.google.com/file/d/0B3rnGrzvGiDkcUFLVFZub25rNVk/view?usp=sharing
(解壓縮後放進Arduino目錄下)


* S4A軟體
http://s4a.cat/

---

### Update code:'src': 

* Server程式碼
TCP_ECHO_SERVER.cpp

* S4A韌體，用Arduino編輯器燒錄到板子
Uno_firmware.ino

* 小朋友的wifi程式碼
wifi_button.ino

* 連接Arduino板子的wifi程式碼
wifi_on_UNO.ino

---


### 使用步驟:

1.打開熱點

2.裝置上電(wifi module),熱點軟體會顯示目前連接上的ip

3.確認連上後,Arduino uno接電腦,開S4A

4.打開server.exe,看裝置是否都有連到server

5.點S4A的綠旗,按下按鈕後會正常運作
