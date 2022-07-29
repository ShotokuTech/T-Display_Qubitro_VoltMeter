#include <TFT_eSPI.h>
#include <SPI.h>

#include <QubitroMqttClient.h>
#include <WiFi.h>

// WiFi Client
WiFiClient wifiClient;

// Qubitro Client
QubitroMqttClient mqttClient(wifiClient);

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

#define analogPin32 32     
int adc4 = 0;
float voltage = 0;

// Device Parameters
char deviceID[] = "cb5beddb-c6c2-41b0-b8b5-f16500778c0e";
char deviceToken[] = "csesZcNk-ibmW6H9RBBsIxaBNChnN7oP-17gSCbH";

// WiFi Parameters
const char* ssid = "blacklabs";
const char* password = "2 Ixtlan";

void setup()
{
//    Serial.begin(115200);
    pinMode(analogPin32, INPUT);   
    tft.init();
    tft.setRotation(1);
    tft.setTextColor(TFT_GREEN);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(4);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);

    // Initialize the serial port
    serial_init();
  
    // Initialize wireless connectivity
    wifi_init();

    // Initialize Qubitro
    qubitro_init();    
}    

void loop()
{
    tft.fillScreen(TFT_BLACK);            
    tft.setCursor(0, 0);
    adc4 = analogRead(analogPin32);
    Serial.print(adc4);
    Serial.print("  ");    
    voltage=(adc4*19.8)/(4095);
    tft.print(voltage);
    tft.println(" V");

    // Send telemetry
    String payload = "{\"Solar Panel One\":" + String(voltage)
    + "}";
    mqttClient.poll();
    mqttClient.beginMessage(deviceID);
    mqttClient.print(payload);
    mqttClient.endMessage();
    
    delay(30*1000);
}

// Initialization code

void serial_init() {
  // Initiate serial port connection
  Serial.begin(115200);

  // Delay for stabilization
  delay(200);
}

void wifi_init() {
  // Set WiFi mode
  WiFi.mode(WIFI_STA);

  // Disconnect WiFi
  WiFi.disconnect();
  delay(100);

  // Initiate WiFi connection
  WiFi.begin(ssid, password);

  // Print connectivity status to the terminal
  Serial.print("Connecting to WiFi...");
  while(true)
  {
    delay(1000);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi Connected.");
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      Serial.print("RSSI: ");
      Serial.println(WiFi.RSSI());
      break;
    }
  }
}

void qubitro_init() {
  char host[] = "broker.qubitro.com";
  int port = 1883;
  mqttClient.setId(deviceID);
  mqttClient.setDeviceIdToken(deviceID, deviceToken);
  Serial.println("Connecting to Qubitro...");
  if (!mqttClient.connect(host, port))
  {
    Serial.print("Connection failed. Error code: ");
    Serial.println(mqttClient.connectError());
    Serial.println("Visit docs.qubitro.com or create a new issue on github.com/qubitro");
  }
  Serial.println("Connected to Qubitro.");
  mqttClient.subscribe(deviceID);
}
