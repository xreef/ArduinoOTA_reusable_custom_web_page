/*
 *	Custom OTA page from LittleFS file system
 *
 *	Renzo Mischianti <www.mischianti.org>
 *
 *	https://www.mischianti.org/category/tutorial/esp8266-tutorial/esp8266-firmware-management-tutorial/
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <LittleFS.h>

#ifndef STASSID
#define STASSID "<YOUR-SSID>"
#define STAPSK  "<YOUR-PASSWD>"
#endif

const char* host = "esp8266-webupdate";
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += httpServer.uri();
  message += "\nMethod: ";
  message += (httpServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += httpServer.args();
  message += "\n";

  for (uint8_t i = 0; i < httpServer.args(); i++) {
    message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\n";
  }

  httpServer.send(404, "text/plain", message);
}

bool loadFromFS(String path, String dataType) {
  Serial.print("Requested page -> ");
  Serial.println(path);
  if (LittleFS.exists(path)){
      File dataFile = LittleFS.open(path, "r");
      if (!dataFile) {
          handleNotFound();
          return false;
      }

      if (httpServer.streamFile(dataFile, dataType) != dataFile.size()) {
        Serial.println("Sent less data than expected!");
      }else{
          Serial.println("Page served!");
      }

      dataFile.close();
  }else{
      handleNotFound();
      return false;
  }
  return true;
}

void handleRoot() {
//	loadFromFS("/index_color.html.gz", "text/html");
	loadFromFS("/index_black_white.html.gz", "text/html");
}

void setup(void) {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  Serial.print(F("Inizializing FS..."));
  if (LittleFS.begin()){
    Serial.println(F("done."));
  }else{
    Serial.println(F("fail."));
  }

  MDNS.begin(host);

  // handler for the /update form page
  httpServer.on("/update", HTTP_GET, handleRoot);

  httpUpdater.setup(&httpServer);

  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer modified ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void) {
  httpServer.handleClient();
  MDNS.update();
}
