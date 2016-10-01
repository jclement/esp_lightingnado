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
#define PIN_RESET 13 // Labeled D7 on ESP12E DEVKIT V2
#define RESET_DURATION 30

#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
int led_count = -1;

#include "LightMode.hpp"
#include "Ants/Ants.hpp"
#include "Slide/Slide.hpp"
#include "Twinkle/Twinkle.hpp"
#include "Percent/Percent.hpp"
LightMode *currentMode = NULL;
char currentModeChar=' ';

// MQTT
#include <AsyncMqttClient.h>
AsyncMqttClient mqttClient;
char mqtt_server[40];
uint mqtt_port;
char mqtt_user[20];
char mqtt_pass[20];

// Name for this ESP
char node_name[20];
char topic_status[50];
char topic_control[50];

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

void saveSetting(const char* key, char* value) {
  char filename[80] = "/config_";
  strcat(filename, key);

  File f = SPIFFS.open(filename, "w");
  if (f) {
    f.print(value);
  }
  f.close();
}

String readSetting(const char* key) {
  char filename[80] = "/config_";
  strcat(filename, key);

  String output;

  File f = SPIFFS.open(filename, "r");
  if (f) {
    output = f.readString();
  }
  f.close();
  return output;
}

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);

  SPIFFS.begin();

  WiFiManager wifiManager;

  // short pause on startup to look for settings RESET
  Serial.println("Waiting for reset");
  pinMode(PIN_RESET, INPUT_PULLUP);
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

  WiFiManagerParameter custom_node_name("nodename", "node name", "rename_me", sizeof(node_name));
  wifiManager.addParameter(&custom_node_name);

  WiFiManagerParameter custom_led_count("led_count", "led count", "1", 6);
  wifiManager.addParameter(&custom_led_count);

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point
  wifiManager.autoConnect();

  //print out obtained IP address
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  if (shouldSaveConfig) {
    Serial.println("Saving configuration...");
    saveSetting("mqtt_server", (char *) custom_mqtt_server.getValue());
    saveSetting("mqtt_port", (char *) custom_mqtt_port.getValue());
    saveSetting("mqtt_user", (char *) custom_mqtt_user.getValue());
    saveSetting("mqtt_pass", (char *) custom_mqtt_pass.getValue());
    saveSetting("node_name", (char *) custom_node_name.getValue());
    saveSetting("led_count", (char *) custom_led_count.getValue());
  }

  // read settings from configuration
  readSetting("mqtt_server").toCharArray(mqtt_server, sizeof(mqtt_server));
  mqtt_port = readSetting("mqtt_port").toInt();
  readSetting("mqtt_user").toCharArray(mqtt_user, sizeof(mqtt_user));
  readSetting("mqtt_pass").toCharArray(mqtt_pass, sizeof(mqtt_pass));
  readSetting("node_name").toCharArray(node_name, sizeof(node_name));
  led_count = readSetting("led_count").toInt();

  Serial.print("Node Name: ");
  Serial.println(node_name);

  Serial.print("LED Count: ");
  Serial.println(led_count);

  strcat(topic_status, "esp/");
  strcat(topic_status, node_name);
  strcat(topic_status, "/status");

  strcat(topic_control, "esp/");
  strcat(topic_control, node_name);
  strcat(topic_control, "/control");

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

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

  mqttClient.setClientId(node_name);

  Serial.println("Connecting to MQTT...");
  mqttClient.connect();

  strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(led_count, 4);
  strip->Begin();
  strip->ClearTo(RgbColor(0,0,0));
  strip->Show();

  // default mode?
  //currentMode = new Slide(strip, "{\"delay\": 100, \"length\": 5, \"right\": false, \"color\":[200,100,0]}");
  //currentModeChar = 'S';
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

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("** Unsubscribe acknowledged **");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("** Disconnected from the broker **");
  Serial.println("Reconnecting to MQTT...");
  mqttClient.connect();
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("** Publish received **");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  payload: ");
  Serial.println(payload);
  if (len > 0) {
    char targetModeChar = payload[0];
    char* modePayload = payload;
    modePayload++; // strip the first char
    if (targetModeChar != currentModeChar) {
      // new mode!
      delete currentMode;
      currentMode = NULL;
      currentModeChar = ' ';

      // create mode, if we can find it
      switch (targetModeChar) {
        case 'S':
          currentMode = new Slide(strip, modePayload);
          break;
        case 'T':
            currentMode = new Twinkle(strip, modePayload);
            break;
        case 'P':
            currentMode = new Percent(strip, modePayload);
            break;
        case 'A':
            currentMode = new Ants(strip, modePayload);
            break;
      }

      if (currentMode != NULL) {
        currentModeChar = targetModeChar;

        // push status message to MQTT
        char* description = currentMode->description();
        char* msg = (char*) malloc(strlen(description) + 10);
        strcpy(msg, "Switch: ");
        strcat(msg, description);
        mqttClient.publish(topic_status, 2, true, msg);
        delete description;
        delete msg;

      } else {
        mqttClient.publish(topic_status, 2, true, "Mode Cleared");
      }
    } else {
      if (currentMode != NULL) {
        currentMode->update(modePayload);

        // push status message to MQTT
        char* description = currentMode->description();
        char* msg = (char*) malloc(strlen(description) + 10);
        strcpy(msg, "Update: ");
        strcat(msg, description);
        mqttClient.publish(topic_status, 2, true, msg);
        delete description;
        delete msg;
      }
    }
  }
}

void onMqttPublish(uint16_t packetId) {
}

/* ========================================================================================================
                         _____         .__         .____
                        /     \ _____  |__| ____   |    |    ____   ____ ______
                       /  \ /  \\__  \ |  |/    \  |    |   /  _ \ /  _ \\____ \
                      /    Y    \/ __ \|  |   |  \ |    |__(  <_> |  <_> )  |_> >
                      \____|__  (____  /__|___|  / |_______ \____/ \____/|   __/
                              \/     \/        \/          \/            |__|
   ======================================================================================================== */

void loop() {
  if (currentMode != NULL) {
    currentMode->tick();
  } else {
    delay(10);
  }
}
