#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#define WIFI_SSID "CENSORED"
#define WIFI_PASSWORD "CENSORED"

#define OTA_HOSTNAME "strobeLight1"
#define OTA_PASSWORD "CENSORED"
#define OTA_PORT 3232

#define MQTT_USERNAME "CENSORED"
#define MQTT_PASSWORD "CENSORED"
#define MQTT_HOST "mqtt.internal.0x08.in"
#define MQTT_PORT 8883
#define MQTT_TOPIC_PREFIX "bus/devices/strobeLight1/"
uint16_t dutyCycle = 200;
int16_t strobe_frequency = 0;

static const char digicert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIGaDCCBFCgAwIBAgIDCKqsMA0GCSqGSIb3DQEBCwUAMIHHMQswCQYDVQQGEwJS
VTEeMBwGA1UECBMVUHJvdmluY2Ugb2YgTGVuaW5ncmFkMRcwFQYDVQQHEw5TdC4g
UGV0ZXJzYnVyZzESMBAGA1UEChMJQjRDS1NQNENFMR0wGwYDVQQLExRTeWNoJ3Mg
SnVpY2UgQnJld2VyeTEvMC0GA1UEAxMmQjRDS1NQNENFIFJvb3QgQ2VydGlmaWNh
dGlvbiBBdXRob3JpdHkxGzAZBgkqhkiG9w0BCQEWDGluZm9AMHgwOC5pbjAeFw0y
MDAxMDQxNDAzMDBaFw0zMDAxMDQxNDAzMDBaMIGyMQswCQYDVQQGEwJSVTEeMBwG
A1UECBMVUHJvdmluY2Ugb2YgTGVuaW5ncmFkMRcwFQYDVQQHEw5TdC4gUGV0ZXJz
YnVyZzESMBAGA1UEChMJQjRDS1NQNENFMRMwEQYDVQQLEwpBdXRvbWF0aW9uMSQw
IgYDVQQDExtCNENLU1A0Q0UgQXV0b21hdGlvbiBCdXMgQ0ExGzAZBgkqhkiG9w0B
CQEWDGluZm9AMHgwOC5pbjCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIB
AKjJJh0ECc2uBmAC5OJWiX90bgOnTj3aUeIBT2cQaHmS0zJuSRwF56f58qwD+rqI
OEz+EPHFpkETRTO2OrmdtW5sXs+YXwZ6Hd8VgrK/lozH0zLInEMtkEP1LbLDQr0T
rWFm01uaNLDQAClSSfEMfYO+qTK7LubL6bFAhOXUuie8N81iCgPhVJ2EpcGtZ3Vt
xIeEviW3GjwhjK2SfDfC17QjU1mpVwxiv3RxmTXRlBisnqKDas4PWeQfJoYW8+KR
xjFlVFmSz8R+BMfihNZzM2VMO2WrUC9QOiYhlVqUqzSthGvxFWrIh4vbjnYk7+ZF
ZKqdeWBQRl4pSz7nu3s3i7LXf5+rke9bhGJXtor1JX2ZKqZwKUFuu+d6WxpZl3Ej
V5LiagLkpGFS30/pkUpX4SS9uE2cEla6OsRw3Yw9KHQIr+Hib380pXpcmDg07FMI
ZEnfBbnsU1wIvhbTXygux7jaZ1VL4bPnD03zI4VuuaxTBXj1kc2blaiFxf8ciDn0
B83BAxjbvyuujJ4tgxYtvgY257mBL2AC0JWLkCMFlARQW0KoP/SIl1VU2PZzDsAw
FiUS5gKlvPjUXUvgySOs54FSJ973McxJZL/sk06abpxdBkbqtWwXgfiQBn7f/JBF
xKaj894uGnoT/URwI+XmghD84BktLKcGBGNTtw6lVvfFAgMBAAGjcDBuMA8GA1Ud
EwEB/wQFMAMBAf8wHQYDVR0OBBYEFJnSDz+ucNW6kLpw0BckdPIR58JZMAsGA1Ud
DwQEAwIBBjARBglghkgBhvhCAQEEBAMCAAcwHAYJYIZIAYb4QgENBA8WDVNhZmV0
eSBUaGlyZCEwDQYJKoZIhvcNAQELBQADggIBAE0MMgNTp37Z7ak6hSUtZr3rapbb
12ngNrwf4wkqOurt1BxYmHN+fKTWFdaWP7ft+Jv1PJN4QIY4I+yojSpdqlsqUlK1
2XwF2t/QmGi9pWlzspbzdNDwIn7Fqrkw+s8DNtRJCB1E04VCjhYKmfBVqF8JdcL3
gu61yR4xxU0TN0B/QPuMPMIQbkutU36JOIdlBmDqeM4TAd2YZjK+KSMMaLaBG2AQ
ljUOn3jfIh12Ql5wueSINMJbnCWBA64/q5fMu7JFZ1bGYbpx6t4dc7xLhYvf1nZF
S1EUZXNWhuQo9Tp1aaOsNuUwS735FCnzBody//9c4oI1LSyZR7yesJqSyIseLmTL
wR1iVFHGuK9qjAVMHmFUXnl7doe1duJyKYW5mn8+UcQArzWvM7qjKUq08KD0v1Aq
BLKJzZqeko5K3cyXdWw6EPV0vnFa9MDpXVlJS8BaRjqHsA/PldwviWh4SxtuB05W
QaZ72XMZ4J6JniehgM5LvTHGpIxxXnqjpVUPsnSlgSgpQaXHQK4+8WJWyUYO+L5Z
0Q12AOjIRMTD41WcbVTU6ifAa83SzV2ZMxMu2UlxhHDjDoM4GtA13wA5WHGjDTfA
iiwRCk5eadKXRGTMO4QiSCv1oI8cg4GHnlXY8kwvITqft1T/TcD0+TTy3qgORHtj
/oTl0xjZcMUsMesP
-----END CERTIFICATE-----
)EOF";

AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
TimerHandle_t arduinoUpdateTimer;
// TimerHandle_t strobeTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void getUpdates() {
  ArduinoOTA.handle();
  ArduinoOTA.onStart([]() {
    xTimerStop(arduinoUpdateTimer, 0);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    xTimerStart(arduinoUpdateTimer, 0);
});
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.setSecure(true);
  mqttClient.setRootCa(digicert, 2283);
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        Serial.println("OTA server has been started");
        ArduinoOTA.begin();
        xTimerStart(arduinoUpdateTimer, 0);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStop(arduinoUpdateTimer, 0);
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe(MQTT_TOPIC_PREFIX "#", 0);
  Serial.print("Subscribing at QoS 0, packetId: ");
  Serial.println(packetIdSub);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.print("Disconnected from MQTT. ");
  Serial.println((uint8_t)reason);

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  char* data = (char*)malloc(len + 1);
  data[len] = 0;
  strncpy(data, payload, len);
  if (strcmp(topic, MQTT_TOPIC_PREFIX "frequency") == 0){
    strobe_frequency = atoi(data);
    // if (strobe_frequency == 0) xTimerStop(strobeTimer, 0);
    // xTimerStart(strobeTimer, 0);
    // xTimerChangePeriod(strobeTimer, pdMS_TO_TICKS(strobe_frequency), 500);
    Serial.print("Freq ");
  } else if (topic, MQTT_TOPIC_PREFIX "dutyCycle"){
    dutyCycle = atoi(data);
    Serial.print("dutyCycle ");
  };
  Serial.println(data);
  free(data);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void strobo() {
  if (strobe_frequency == 0){
    digitalWrite(13, LOW);
    // Serial.println("Strobe off");
    } else {
    digitalWrite(13, HIGH);
    // Serial.println("Strobe on");
    delay(dutyCycle);
    digitalWrite(13, LOW);
  }
}



void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(13, OUTPUT);
  WiFi.mode(WIFI_STA);
  
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.setPort(OTA_PORT);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  arduinoUpdateTimer = xTimerCreate("arduinoOTATimer", pdMS_TO_TICKS(1500), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(getUpdates));
  // strobeTimer = xTimerCreate("strobeLightTimer", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(strobo));


  WiFi.onEvent(WiFiEvent);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);


  connectToWifi();
}

void loop() {
  // if (!WiFi.isConnected()) connectToWifi();
  // delay(2000);
  strobo();
  if (strobe_frequency == 0) delay(200);
  delay(strobe_frequency);

}
