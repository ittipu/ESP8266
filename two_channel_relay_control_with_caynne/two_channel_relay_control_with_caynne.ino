#include <ESP8266WiFi.h>
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>

#define RELAY_1 D0
#define RELAY_2 D1
#define RELAY_3 D2


const char* ssid = "TASIN";
const char* wifiPassword = "rasel101010";

char username[] = "b9190820-790b-11ed-8d53-d7cd1025126a";
char password[] = "b1811d8f4713efd09c5f8fc899e2f770064fc773";
char clientID[] = "4a8a0b50-790d-11ed-b193-d9789b2af62b";
#define RELAY1_CHANNEL 1
#define RELAY2_CHANNEL 2
#define RELAY3_CHANNEL 3

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);

  setup_wifi();
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);

}

void loop() {
  Cayenne.loop();
}

CAYENNE_IN(RELAY1_CHANNEL)
{
  int currentValue = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", RELAY1_CHANNEL, RELAY_1, currentValue);
  if (currentValue == 1) {
    digitalWrite(RELAY_1, HIGH);
  }
  if (currentValue == 0) {
    digitalWrite(RELAY_1, LOW);
  }
}

CAYENNE_IN(RELAY2_CHANNEL)
{
  int currentValue = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", RELAY2_CHANNEL, RELAY_2, currentValue);
  if (currentValue == 1) {
    digitalWrite(RELAY_2, HIGH);
  }
  if (currentValue == 0) {
    digitalWrite(RELAY_2, LOW);
  }
}

CAYENNE_IN(RELAY3_CHANNEL)
{
  int currentValue = getValue.asInt();
  CAYENNE_LOG("Channel %d, pin %d, value %d", RELAY3_CHANNEL, RELAY_3, currentValue);
  if (currentValue == 1) {
    digitalWrite(RELAY_3, HIGH);
  }
  if (currentValue == 0) {
    digitalWrite(RELAY_3, LOW);
  }
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    Serial.print(".");
  }
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


}
