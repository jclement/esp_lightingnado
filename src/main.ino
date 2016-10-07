#include "arduino.h"

// Needed for SPIFFS
#include <FS.h>

// Wireless / Networking
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266WiFi.h>

// PIN used to reset configuration.  Enables internal Pull Up.  Ground to reset.
#define PIN_RESET 13       // Labeled D7 on ESP12E DEVKIT V2
#define RESET_DURATION 30
#define PS_OUTPUT 12      // D6
#define PS_CHECK 14       // D5 (or tie to D6)

#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
uint led_count = -1;
uint led_inset_start = 0;
uint led_inset_length = 999;

#include "LightMode.hpp"
#include "Ants/Ants.hpp"
#include "Slide/Slide.hpp"
#include "Twinkle/Twinkle.hpp"
#include "Tracker/Tracker.hpp"
#include "Percent/Percent.hpp"
#include "Rainbow/Rainbow.hpp"

LightMode *currentMode = NULL;
char currentModeChar=' ';

// MQTT
#include <AsyncMqttClient.h>
AsyncMqttClient mqttClient;
char* mqtt_server;
uint mqtt_port;
char* mqtt_user;
char* mqtt_pass;

// Name for this ESP
char* node_name;
char* topic_status;
char* topic_status_mode;
char* topic_control;

// temporary variables for incoming messages from MQTT
#define MAX_MESSAGE_SIZE 1000
bool hasUnprocessedMessage = false;
char pendingMessage[MAX_MESSAGE_SIZE] = "";

/* ========================================================================================================
                                           __
                              ______ _____/  |_ __ ________
                             /  ___// __ \   __\  |  \____ \
                             \___ \\  ___/|  | |  |  /  |_> >
                            /____  >\___  >__| |____/|   __/
                                 \/     \/           |__|
   ======================================================================================================== */

bool shouldSaveConfig = false;
void saveConfigCallback()
{
  shouldSaveConfig = true;
}

void saveSetting(const char* key, WiFiManagerParameter setting) {
  char filename[80] = "/config_";
  strcat(filename, key);
  const char *value = setting.getValue();
  File f = SPIFFS.open(filename, "w");
  if (f) {
    f.print(value);
  }
  f.close();
}

char* readSetting(const char* key) {
  char filename[80] = "/config_";
  strcat(filename, key);
  File f = SPIFFS.open(filename, "r");
  char* output = (char *) malloc(f.size()+1);
  f.readBytes(output, f.size());
  output[f.size()] = '\0';
  f.close();
  return output;
}

uint readIntSetting(const char* key) {
  char filename[80] = "/config_";
  strcat(filename, key);
  File f = SPIFFS.open(filename, "r");
  char* output = (char *) malloc(f.size()+1);
  f.readBytes(output, f.size());
  output[f.size()] = '\0';
  f.close();
  uint result = atoi(output);
  free(output);
  return result;
}

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);

  SPIFFS.begin();

  WiFiManager wifiManager;

  // short pause on startup to look for settings RESET
  Serial.println("Waiting for reset");
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PS_CHECK, INPUT);
  pinMode(PS_OUTPUT, OUTPUT);
  digitalWrite(PS_OUTPUT, LOW);
  bool reset = false;
  int resetTimeRemaining = RESET_DURATION;
  while (!reset && resetTimeRemaining-- > 0) {
    if (digitalRead(PIN_RESET) == 0) {
      reset = true;
    }
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  if (reset) {
    Serial.println("Resetting");
    SPIFFS.format();
    wifiManager.resetSettings();
  }

  // add bonus parameters to WifiManager
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", "", 40);
  wifiManager.addParameter(&custom_mqtt_server);

  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", "1883", 6);
  wifiManager.addParameter(&custom_mqtt_port);

  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", "", 20);
  wifiManager.addParameter(&custom_mqtt_user);

  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", "", 20);
  wifiManager.addParameter(&custom_mqtt_pass);

  WiFiManagerParameter custom_node_name("nodename", "node name", "", 50);
  wifiManager.addParameter(&custom_node_name);

  WiFiManagerParameter custom_led_count("led_count", "led count", "1", 6);
  wifiManager.addParameter(&custom_led_count);

  WiFiManagerParameter custom_led_inset_start("led_inset_start", "led inset start", "", 6);
  wifiManager.addParameter(&custom_led_inset_start);

  WiFiManagerParameter custom_led_inset_length("led_inset_length", "led inset length", "", 6);
  wifiManager.addParameter(&custom_led_inset_length);

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point
  wifiManager.autoConnect();

  //print out obtained IP address
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  if (shouldSaveConfig) {
    Serial.println("Saving configuration...");
    saveSetting("mqtt_server", custom_mqtt_server);
    saveSetting("mqtt_port", custom_mqtt_port);
    saveSetting("mqtt_user", custom_mqtt_user);
    saveSetting("mqtt_pass", custom_mqtt_pass);
    saveSetting("node_name", custom_node_name);
    saveSetting("led_count", custom_led_count);
    saveSetting("led_inset_start", custom_led_inset_start);
    saveSetting("led_inset_length", custom_led_inset_length);
  }

  // read settings from configuration
  mqtt_server = readSetting("mqtt_server");
  mqtt_port = readIntSetting("mqtt_port");
  mqtt_user = readSetting("mqtt_user");
  mqtt_pass = readSetting("mqtt_pass");
  node_name = readSetting("node_name");
  led_count = readIntSetting("led_count");
  led_inset_start = readIntSetting("led_inset_start");
  led_inset_length = readIntSetting("led_inset_length");

  if (led_inset_length > (led_count - led_inset_start)) {
    led_inset_length = led_count - led_inset_start;
  };

  Serial.print("Node Name: ");
  Serial.println(node_name);

  Serial.print("LED Count: ");
  Serial.println(led_count);

  topic_status = (char*) malloc(strlen(node_name) + 12);
  strcpy(topic_status, "esp/");
  strcat(topic_status, node_name);
  strcat(topic_status, "/status");

  topic_control = (char*) malloc(strlen(node_name) + 13);
  strcpy(topic_control, "esp/");
  strcat(topic_control, node_name);
  strcat(topic_control, "/control");

  topic_status_mode = (char*) malloc(strlen(node_name) + 17);
  strcpy(topic_status_mode, "esp/");
  strcat(topic_status_mode, node_name);
  strcat(topic_status_mode, "/status/mode");

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onMessage(onMqttMessage);

  mqttClient.setServer(mqtt_server, mqtt_port);

  mqttClient.setKeepAlive(5);

  mqttClient.setWill(topic_status, 2, true, "offline");

  Serial.print("MQTT: ");
  Serial.print(mqtt_user);
  Serial.print("@");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);

  if (strlen(mqtt_user) > 0) {
    mqttClient.setCredentials(mqtt_user, mqtt_pass);
  }

  //mqttClient.setClientId(node_name);

  Serial.println("Connecting to MQTT...");
  mqttClient.connect();

  strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(led_count, 4);
  strip->Begin();
  strip->ClearTo(RgbColor(0,0,0));
  strip->Show();
}

/* ========================================================================================================
               _____   ______________________________ ___________                    __
              /     \  \_____  \__    ___/\__    ___/ \_   _____/__  __ ____   _____/  |_  ______
             /  \ /  \  /  / \  \|    |     |    |     |    __)_\  \/ // __ \ /    \   __\/  ___/
            /    Y    \/   \_/.  \    |     |    |     |        \\   /\  ___/|   |  \  |  \___ \
            \____|__  /\_____\ \_/____|     |____|    /_______  / \_/  \___  >___|  /__| /____  >
                    \/        \__>                            \/           \/     \/          \/
   ======================================================================================================== */

uint16_t controlSubscribePacketId;

void onMqttConnect() {
  Serial.println("** Connected to the broker **");
  // subscribe to the control topic
  controlSubscribePacketId = mqttClient.subscribe(topic_control, 2);
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("** Subscribe acknowledged **");
  // once successfully subscribed to control, public online status
  if (packetId == controlSubscribePacketId) {
    mqttClient.publish(topic_status, 2, true, "online");
  }
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("** Disconnected from the broker.  Reconnecting... **");
  mqttClient.connect();
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // if we haven't finished processing the current payload, skip this one
  if (hasUnprocessedMessage) return;
  // if the message is too large, skip it
  if (len >= MAX_MESSAGE_SIZE) return;
  // copy payload and flag that we have one!
  strcpy(pendingMessage, payload);
  hasUnprocessedMessage = true;
}

/* ========================================================================================================
             __  __                                  _   _                 _ _ _
            |  \/  | ___  ___ ___  __ _  __ _  ___  | | | | __ _ _ __   __| | (_)_ __   __ _
            | |\/| |/ _ \/ __/ __|/ _` |/ _` |/ _ \ | |_| |/ _` | '_ \ / _` | | | '_ \ / _` |
            | |  | |  __/\__ \__ \ (_| | (_| |  __/ |  _  | (_| | | | | (_| | | | | | | (_| |
            |_|  |_|\___||___/___/\__,_|\__, |\___| |_| |_|\__,_|_| |_|\__,_|_|_|_| |_|\__, |
                                        |___/                                          |___/
   ======================================================================================================== */

void terminateCurrentMode()
{
  delete(currentMode);
  currentMode = NULL;
  currentModeChar = '\0';
}

void updateMode(char* payload)
{
  currentMode->update(payload);

  // push status message to MQTT
  char msg[100] = "Update:";
  strcat(msg, currentMode->description());
  mqttClient.publish(topic_status_mode, 2, true, msg);
}

void switchMode(char mode, char* payload)
{
  LightMode* newMode;

  // create mode, if we can find it
  switch (mode) {
    case 'S':
      newMode = new Slide(strip, payload);
      break;
    case 'T':
      newMode = new Twinkle(strip, payload);
      break;
    case 'P':
      newMode = new Percent(strip, payload, led_inset_start, led_inset_length);
      break;
    case 'A':
      newMode = new Ants(strip, payload);
      break;
    case 'R':
      newMode = new Rainbow(strip, payload);
      break;
    case 'K':
      newMode = new Tracker(strip, payload);
      break;
    case '0': // OFF
      digitalWrite(PS_OUTPUT, LOW);
      delete(currentMode);
      currentMode = NULL;
      currentModeChar = '\0';
      mqttClient.publish(topic_status_mode, 2, true, "Mode Cleared");
      strip->ClearTo(RgbColor(0,0,0));
      strip->Show();
      return;
    default:
      return;
  }

  // if we had a previous mode,
  if (currentMode != NULL) {
    delete(currentMode);
  }

  // switch active mode
  currentModeChar = mode;
  currentMode = newMode;

  // turn on the power supply
  digitalWrite(PS_OUTPUT, HIGH);

  // tell the world
  char msg[100] = "Switch:";
  strcat(msg, currentMode->description());
  mqttClient.publish(topic_status_mode, 2, true, msg);
}

void processPendingMessage() {

  char targetModeChar = pendingMessage[0];
  char* modePayload = pendingMessage + 1;

  if (targetModeChar != currentModeChar) {
    switchMode(targetModeChar, modePayload);
  } else {
    if (currentMode != NULL) {
      updateMode(modePayload);
    }
  }
}

/* ========================================================================================================
                         _____         .__         .____
                        /     \ _____  |__| ____   |    |    ____   ____ ______
                       /  \ /  \\__  \ |  |/    \  |    |   /  _ \ /  _ \\____ \
                      /    Y    \/ __ \|  |   |  \ |    |__(  <_> |  <_> )  |_> >
                      \____|__  (____  /__|___|  / |_______ \____/ \____/|   __/
                              \/     \/        \/          \/            |__|
   ======================================================================================================== */

unsigned long lastWifiCheck=0;
void checkAndResetWifi()
{
  if (abs(millis() - lastWifiCheck) > 5000) {
    // check for Wifi every 5 seconds and bounce if it's disconnected
    lastWifiCheck = millis();
    if (WiFi.status() == WL_DISCONNECTED)
    {
      Serial.println("WiFi Disconnection... Resetting.");
      ESP.reset();
    }
  }
}

void loop() {

  if (currentMode != NULL) {
    // don't try to send data unless the power supply confirms it's good
    if (digitalRead(PS_CHECK) > 0) {
      currentMode->tick();
    }
  }

  Serial.println(digitalRead(PS_CHECK));
  delay(20);

  checkAndResetWifi();

  if (hasUnprocessedMessage) {
    processPendingMessage();
    hasUnprocessedMessage = false;
  }
}
