/* 
Author: Kamruzzaman Tipu
Library: Firebase ESP8266 Library
Library Version: 3.6.4
*/

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESPping.h>
#include <FirebaseESP8266.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define WIFI_SSID "Tahsin"
#define WIFI_PASSWORD "Rasel@@@@1988"
#define WIFI_SEARCHING_COUNTER_MAX 150
#define INTERNET_CONNECTION_COUNTER_MAX 150
#define RTC_COUNTER_IN_SETUP_MAX 120
#define MAIN_LOOP_INTERVAL 10000
#define RTC_COUNTER_MAX 18  // 18*10 = 180s from main loop
#define PING_TIMEOUT 2
#define HOST "www.google.com"

#define API_KEY "AIzaSyCvQoSIMEirKLvtc-UoG4xi2hYjHsJYSAk "
#define DATABASE_URL "rasel-home-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "rasel1@email.com"
#define USER_PASSWORD "rasel1234"

#define RELAY_1 D0
#define RELAY_2 D1
#define RELAY_3 D2
#define RELAY_4 D3

#define RELAY_1_FB_KEY "l1"
#define RELAY_2_FB_KEY "l2"
#define RELAY_3_FB_KEY "fn"
#define RELAY_4_FB_KEY "tv"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
String value = "";
StaticJsonDocument<256> doc;
StaticJsonDocument<256> doc1;
unsigned int wifi_searching_counter = 0;
unsigned int internet_connection_counter = 0;
unsigned long epochTime;
unsigned long previousMillis = 0;
unsigned long rtc_counter = 0;


void streamCallback(StreamData data) {
  Serial.println("Received Data: ");
  value = data.stringData().c_str();
  deserializeJson(doc, value);
  if (doc[RELAY_1_FB_KEY] == true) {
    Serial.print(RELAY_1_FB_KEY);
    Serial.println(": HIGH");
    digitalWrite(RELAY_1, HIGH);
  } else if (doc[RELAY_1_FB_KEY] == false) {
    Serial.print(RELAY_1_FB_KEY);
    Serial.println(": LOW");
    digitalWrite(RELAY_1, LOW);
  } else if (doc[RELAY_2_FB_KEY] == true) {
    Serial.print(RELAY_2_FB_KEY);
    Serial.println(": HIGH");
    digitalWrite(RELAY_2, HIGH);
  } else if (doc[RELAY_2_FB_KEY] == false) {
    Serial.print(RELAY_2_FB_KEY);
    Serial.println(": LOW");
    digitalWrite(RELAY_2, LOW);
  } else if (doc[RELAY_3_FB_KEY] == true) {
    Serial.print(RELAY_3_FB_KEY);
    Serial.println(": HIGH");
    digitalWrite(RELAY_3, HIGH);
  } else if (doc[RELAY_3_FB_KEY] == false) {
    Serial.print(RELAY_3_FB_KEY);
    Serial.println(": LOW");
    digitalWrite(RELAY_3, LOW);
  } else if (doc[RELAY_4_FB_KEY] == true) {
    Serial.print(RELAY_4_FB_KEY);
    Serial.println(": HIGH");
    digitalWrite(RELAY_4, HIGH);
  } else if (doc[RELAY_4_FB_KEY] == false) {
    Serial.print(RELAY_4_FB_KEY);
    Serial.println(": LOW");
    digitalWrite(RELAY_4, LOW);
  }
  Serial.println();
}

// void streamTimeoutCallback(bool timeout) {
//   if (timeout)
//     Serial.println("stream timed out, resuming...\n");

//   if (!stream.httpConnected())
//     Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
// }


void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\nConnecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    wifi_searching_counter++;
    Serial.println(wifi_searching_counter);
    Serial.print(".");
    delay(300);
    if (wifi_searching_counter == WIFI_SEARCHING_COUNTER_MAX) {
      Serial.print("Wifi Searching Timeout, Going to Reset");
      wifi_searching_counter = 0;
      ESP.reset();
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  timeClient.begin();
  while (1) {
    if (Ping.ping(HOST, PING_TIMEOUT)) {
      Serial.println("Online");
      rtc_counter = 0;
      break;
    } else {
      Serial.println("Offline");
      Serial.print("RTC Connection Counter: ");
      Serial.println(rtc_counter);
      rtc_counter++;
      delay(500);
      if (rtc_counter == RTC_COUNTER_IN_SETUP_MAX) {
        Serial.println("RTC timout, Device will reset now");
        rtc_counter = 0;
        ESP.reset();
      }
    }
  }
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.beginStream(stream, "/db/power/")) {
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
  }
  if (Firebase.RTDB.get(&fbdo, "/db/power")) {
    String value = fbdo.stringData().c_str();
    Serial.println("Initial State from Firebase: ");
    Serial.println(value);
    deserializeJson(doc1, value);
    if (doc1[RELAY_1_FB_KEY] == true) {
      Serial.print(RELAY_1_FB_KEY);
      Serial.println(": HIGH");
      digitalWrite(RELAY_1, HIGH);
    }
    if (doc1[RELAY_1_FB_KEY] == false) {
      Serial.print(RELAY_1_FB_KEY);
      Serial.println(": LOW");
      digitalWrite(RELAY_1, LOW);
    }
    if (doc1[RELAY_2_FB_KEY] == true) {
      Serial.print(RELAY_2_FB_KEY);
      Serial.println(": HIGH");
      digitalWrite(RELAY_2, HIGH);
    }
    if (doc1[RELAY_2_FB_KEY] == false) {
      Serial.print(RELAY_2_FB_KEY);
      Serial.println(": LOW");
      digitalWrite(RELAY_2, LOW);
    }
    if (doc1[RELAY_3_FB_KEY] == true) {
      Serial.print(RELAY_3_FB_KEY);
      Serial.println(": HIGH");
      digitalWrite(RELAY_3, HIGH);
    }
    if (doc1[RELAY_3_FB_KEY] == false) {
      Serial.print(RELAY_3_FB_KEY);
      Serial.println(": LOW");
      digitalWrite(RELAY_3, LOW);
    }
    if (doc1[RELAY_4_FB_KEY] == true) {
      Serial.print(RELAY_4_FB_KEY);
      Serial.println(": HIGH");
      digitalWrite(RELAY_4, HIGH);
    }
    if (doc1[RELAY_4_FB_KEY] == false) {
      Serial.print(RELAY_4_FB_KEY);
      Serial.println(": LOW");
      digitalWrite(RELAY_4, LOW);
    }
  }
  Firebase.setStreamCallback(stream, streamCallback);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= MAIN_LOOP_INTERVAL) {
    previousMillis = currentMillis;
    if (Ping.ping(HOST, PING_TIMEOUT)) {
      Serial.println("Online");
      rtc_counter = 0;
    } else {
      Serial.println("Offline");
      Serial.print("RTC Connection Counter: ");
      Serial.println(rtc_counter);
      rtc_counter++;
      if (rtc_counter == RTC_COUNTER_MAX) {
        Serial.println("RTC timout, Device will reset now");
        rtc_counter = 0;
        ESP.reset();
      }
    }
  }
}


unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}