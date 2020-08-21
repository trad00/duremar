#ifndef WEB_H
#define WEB_H

#include <ESP8266WebServer.h>

namespace webserver {

const char *ssid = APSSID;
const char *password = APPSK;

const char* www_username = WWWUSER;
const char* www_password = WWWPWD;

ESP8266WebServer server(80);

void handleRoot() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  String html;
  for (uint8_t i=0; i<4; i++) {
    html += "<h1>T" + String(i+1) + " ";
    html += main::fmtTemperature(main::temps[i], 1) + "</h1><br>\n\r";
  }
  html += "<hl><br>\n\r";
  for (uint8_t i=0; i<4; i++) {
    bool relOn = digitalRead(main::ra[i].getPin()) == REL_ON;
    html += "<h1>R" + String(i+1) + " ";
    html += relOn ? String("ON"): String("OFF") + "</h1><br>\n\r";
  }
  
  server.send(200, "text/html", html);
}

void setup() {
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
}

void begin() {
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

} //namespace

#endif
