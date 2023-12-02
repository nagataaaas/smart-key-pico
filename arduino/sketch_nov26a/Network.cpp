#include "Network.hpp"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <SerialBT.h>
#include <WiFi.h>

#include <string>

const char* ssid = "TP-Link_4DB9";
const char* password = "80018827";

const uint8_t fingerprint[20] = { 0x67, 0xC4, 0x38, 0xCD, 0x32, 0x59, 0x0C, 0xA1, 0xA6, 0x2F, 0x8C, 0xE7, 0x30, 0xD1, 0x5B, 0xD0, 0x5E, 0x52, 0x99, 0x84 };
String base_url = "http://api.nagata.pro";

void connect_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    sleep_ms(300);
    SerialBT.print(".");
  }
  SerialBT.println("connected!!");
}

int get_timestamp() {
  StaticJsonDocument<1024> doc;
  HTTPClient http;
  http.begin("http://worldtimeapi.org/api/timezone/Asia/Tokyo");
  int httpCode = http.GET();

  String body = http.getString();
  if (httpCode != HTTP_CODE_OK) return 0;

  DeserializationError error = deserializeJson(doc, body);
  switch (error.code()) {
    case DeserializationError::Ok:
      break;
    case DeserializationError::InvalidInput:
      SerialBT.println(F("Invalid input!\n"));
      break;
    case DeserializationError::NoMemory:
      SerialBT.println(F("Not enough memory\n"));
      break;
    default:
      SerialBT.println(F("Deserialization failed\n"));
      break;
  }
  if (error)
    return 0;
  else {
    return doc["unixtime"];
  }
}

void unlockAPI(char* code) {
  HTTPClient http;

  String url = base_url + "/smart-key/open/";
  url += code;

  http.begin(url);
  int httpCode = http.GET();
};

void lockAPI(char* code) {
  HTTPClient http;

  String url = base_url + "/smart-key/close/";
  url += code;

  http.begin(url);
  int httpCode = http.GET();
};

APILockState getState(char* code) {
  HTTPClient http;
  StaticJsonDocument<256> doc;

  String url = base_url + "/smart-key/get_state/";
  url += code;

  http.begin(url);
  int httpCode = http.GET();

  String body = http.getString();
  if (httpCode != HTTP_CODE_OK) {
    SerialBT.printf("[getState] error API. error code: %d\n", httpCode);
    SerialBT.printf("timestamp: %d\n", get_timestamp());
    return APILockState::UNKNOWN;
  }

  DeserializationError error = deserializeJson(doc, body);
  switch (error.code()) {
    case DeserializationError::Ok:
      break;
    case DeserializationError::InvalidInput:
      SerialBT.println("Invalid input!");
      SerialBT.printf("BODY: %s\n", body.c_str());
      break;
    case DeserializationError::NoMemory:
      SerialBT.println("Not enough memory");
      SerialBT.printf("BODY: %s\n", body.c_str());
      break;
    default:
      SerialBT.println("Deserialization failed");
      SerialBT.printf("BODY: %s\n", body.c_str());
      break;
  }
  if (error) return APILockState::LOCKED;
  return doc["is_open"] ? APILockState::UNLOCKED : APILockState::LOCKED;
};