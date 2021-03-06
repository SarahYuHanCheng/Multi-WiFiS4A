#include <ESP8266WiFi.h>

#include <WiFiClient.h>

/* A TCP client.*/
WiFiClient wifiClient;

// You have to modify the following parameters.
#define SSID "wifi name"
#define PASSWD "wifi password"
#define TCP_IP "192.168.X.X"
#define TCP_PORT 8888

// You have to modify the following parameters. About hardware pin.
#define deviceID 10       // recognized by server
int button_pressed = D4;  
int in_msg = D0;
int wifi_connect = D5;
int out_msg = D2;


char _buffer[3];

void setup()
{
  pinMode(in_msg, OUTPUT);
  pinMode(wifi_connect, OUTPUT);


  pinMode(out_msg, INPUT_PULLUP);
  int idflag = 1;
  char joinedSSID[32];

  // Initialize the UART
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWD);
    Serial.println("Retrying...connect to AP");
  }
  Serial.println("Connected to AP");
  wifiClient.connect(TCP_IP, TCP_PORT);
  _buffer[0] = 'R';         // recognized by server. R means remote button client.
  String(deviceID, HEX).toCharArray((_buffer + 1), 2);
  wifiClient.write(_buffer, 2);
  for (int i = 1; i < sizeof(_buffer); i++) {
    _buffer[i] = {0};
  }

}

static char buffer[32];
static unsigned long timerCheckUpdate = millis();
void loop()
{
//have to press button for 0.2 sec or more, the msg would be send
  int charAvail;
  int temp = digitalRead(out_msg);
  delay(200); 
  int temp2 = digitalRead(out_msg);
  if (!temp && (temp != temp2)) {
    digitalWrite(button_pressed, HIGH);
    String(deviceID, HEX).toCharArray((_buffer), 2);
    _buffer[1] = '1';
    wifiClient.write(_buffer, 2);
    for (int i = 1; i < sizeof(_buffer); i++) {
      _buffer[i] = {0};
    }

  }
  // gets() would return the ID of the sender.
  if ((charAvail = wifiClient.available()) > 0) {
    timerCheckUpdate = millis();
    wifiClient.read((unsigned char *)buffer, 32);
    if (buffer[0] == '1') {
      digitalWrite(in_msg, HIGH); //BUILTIN_LED off
      Serial.println(buffer[0]);
    }
    else if (buffer[0] == '0') {
      digitalWrite(in_msg, LOW); //BUILTIN_LED on
    }

    for (int i = 1; i < sizeof(buffer); i++) {
      buffer[i] = {0};
    }
    digitalWrite(button_pressed, LOW);
  }

  if (millis() - timerCheckUpdate >= 10000)
  {

    if (!wifiClient.connected()) {
      // Cannot join to the TCP server, stop.
      if (!wifiClient.connect(TCP_IP, TCP_PORT)) {
        Serial.println("connection failed");
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
          WiFi.begin(SSID, PASSWD);
          Serial.println("Retrying...connect to AP");
        }
        Serial.println("Connected to AP");
                int i = 0;
                while (i < 2) {
                  digitalWrite(wifi_connect, HIGH);
                  delay(500);
                  digitalWrite(wifi_connect, LOW);
                  delay(200);
                  i++;
                }
      } else {
        _buffer[0] = 'R';
        String(deviceID, HEX).toCharArray((_buffer + 1), 2);
        wifiClient.write(_buffer, 2);
        for (int i = 1; i < sizeof(_buffer); i++) {
          _buffer[i] = {0};
        }
        Serial.println("sended id again");
      }
    }
    else {
      timerCheckUpdate = millis();
    }
  }
}
