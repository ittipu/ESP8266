#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define RELAY_1 D0
#define RELAY_2 D2

const char* ssid = "BRACNet_Employee";
const char* pass = "a$UJ@n@pB@ngl@d3$h";


const char* mqtt_server = "202.168.255.20";
const char* mqtt_user = "tipu";
const char* mqtt_pass = "tipu1234";

String RELAY_1_SUB_TOPIC = "test/light";
String RELAY_2_SUB_TOPIC = "test/fan";


WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(LED_BUILTIN, HIGH);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (client.connect("espClient", mqtt_user, mqtt_pass)) {
    client.subscribe("#");
  }
  digitalWrite(LED_BUILTIN, HIGH);


}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  //  char buffer [256];
  //  serializeJson(doc,buffer);
  //  serializeJson(doc, Serial);
  //  Serial.println();

  client.loop();
  delay(1000);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  if (String(topic) == RELAY_1_SUB_TOPIC) {
    Serial.println(topic);
    if (messageTemp == "0") {
      Serial.println("Relay 1 low");
      digitalWrite(RELAY_1, LOW);
    }

    if (messageTemp == "1") {
      Serial.println("Relay 1 HIGH");
      digitalWrite(RELAY_1, HIGH);
    }
  }
  if (String(topic) == RELAY_2_SUB_TOPIC) {
    Serial.println(topic);
    if (messageTemp == "0") {
      Serial.println("Relay 2 trun off");
      digitalWrite(RELAY_2, LOW);
    }

    if (messageTemp == "1") {
      Serial.println("Relay 2 turn on");
      digitalWrite(RELAY_2, HIGH);
    }
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("#");

    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(2500);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(2500);
    }
  }
}
