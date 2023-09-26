#include "DHT.h"

#define DHTPIN D4
#define DHTTYPE DHT22

#define BLYNK_TEMPLATE_ID "TMPLZUFnnYwy"
#define BLYNK_DEVICE_NAME "Tipu Home"
#define BLYNK_AUTH_TOKEN "kL7MAY4pypYpb4URR0J1ZzIPbjqGILIX"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


DHT dht(DHTPIN, DHTTYPE);

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "IoT_Tipu";
char pass[] = "tipu1234";
int localHumidity = 0;
int localTemperature = 0;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V2)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Serial.print("Incoming data from V2: ");
  Serial.println(value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  localHumidity = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(localHumidity);
  Serial.println("%");
  
  localTemperature = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(localTemperature);
  Serial.println(" degree celcius");

  Blynk.virtualWrite(V0, localTemperature);
  Blynk.virtualWrite(V1, localHumidity);
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(3000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
