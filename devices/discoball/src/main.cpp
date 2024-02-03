#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include "FastAccelStepper.h"
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#define WIFI_SSID "CENSORED"
#define WIFI_PASSWORD "CENSORED"

#define OTA_HOSTNAME "discoBall"
#define OTA_PASSWORD "CENSORED"
#define OTA_PORT 3232

#define MQTT_USERNAME "CENSORED"
#define MQTT_PASSWORD "CENSORED"
#define MQTT_HOST "mqtt.internal.0x08.in"
#define MQTT_TOPIC_PREFIX "bus/devices/discoball/"
#define MQTT_PORT 8883

#define dirPinStepper 25
#define enablePinStepper 32
#define stepPinStepper 33

uint16_t accelerationSpeed = 512; // 512 steps/s²
int16_t speed = 20000; // this means in us/step

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
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

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
        ArduinoOTA.begin();
        Serial.println("OTA server has been started");
        xTimerStart(arduinoUpdateTimer, 0);
        xTimerStart(mqttReconnectTimer, 0);
        xTimerStop(wifiReconnectTimer, 0)
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
  if (strcmp(topic, MQTT_TOPIC_PREFIX "speed") == 0){
    speed = atoi(data);
    if (speed == 0)
      stepper->stopMove(); 
    else if (speed > 0) {
      stepper->setSpeedInUs(speed);
      stepper->runForward();
    } else {
      stepper->setSpeedInUs(speed);
      stepper->runBackward();
    }
    Serial.print("Speed set ");
  } else if (topic, MQTT_TOPIC_PREFIX "accelerationSpeed"){
    accelerationSpeed = atoi(data);
    stepper->setAcceleration(accelerationSpeed);
  };
  stepper->applySpeedAcceleration();
  Serial.println(data);
  free(data);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}



void setup() {
  Serial.begin(115200);
  Serial.println();

  engine.init();
  stepper = engine.stepperConnectToPin(stepPinStepper);
  if (stepper) {
    stepper->setDirectionPin(dirPinStepper);
    stepper->setEnablePin(enablePinStepper, true);
    stepper->setAutoEnable(true);
    stepper->setSpeedInUs(speed);
    stepper->setAcceleration(accelerationSpeed);
    stepper->runForward();
  }

  WiFi.mode(WIFI_STA);
  
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.setPort(OTA_PORT);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  arduinoUpdateTimer = xTimerCreate("arduinoOTATimer", pdMS_TO_TICKS(3000), pdMS_TO_TICKS(500), (void*)0, reinterpret_cast<TimerCallbackFunction_t>(getUpdates));

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

}
