#ifndef WEB_H
#define WEB_H

#include <ESP8266WebServer.h>

namespace webserver {

const char *ssid = APSSID;
const char *password = APPSK;

const char* www_username = WWWUSER;
const char* www_password = WWWPWD;

ESP8266WebServer server(80);

String fmtFloatValue(float val, int decPlc, String unit) {
  if (val == emptySignal){
    return "n/a";
  }
  uint8_t num = decPlc == 0 ? 3 : 4 + decPlc;
  return String(val, decPlc) + unit;
}

String fmtTemperature(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, "&#176;C");
}

String fmtPressure(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, " mmHg");
}

String fmtAlt(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, " m");
}

void handleRoot() {
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  
  const char html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>Duremar 4</title>

<style type="text/css">
  table {
    background: white;
    border: 0px solid black;
  }
  td {
    font-size: 30pt;
    font-family: Verdana, Arial, Helvetica, sans-serif;
    padding: 10px 40px;
    text-align: center;
  }
  .Rbtn {
    font-size: 30pt;
    font-family: Verdana, Arial, Helvetica, sans-serif;
    text-align: center;
    color: white;
    float: left;
    width: 70px;
    padding: 20px 20px;
    margin: 5px 5px;
    background-color: gray;
  }
  .RbtnOn {
    background-color: #43a209;
  }
</style>
</head>

<body>
<table>
<tr><td>T1</td><td id="t0">--</td></tr>
<tr><td>T2</td><td id="t1">--</td></tr>
<tr><td>T3</td><td id="t2">--</td></tr>
<tr><td>T4</td><td id="t3">--</td></tr>
</table>
<hr/>
<div class="Rbtn" id="r0">R1</div>
<div class="Rbtn" id="r1">R2</div>
<div class="Rbtn" id="r2">R3</div>
<div class="Rbtn" id="r3">R4</div>
</body>

<script type="text/javascript">
function update() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        var elem;
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = data[i];
        }
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("r"+i);
          if (data[i+4] == 0)
            elem.classList.remove("RbtnOn");
          else
            elem.classList.add("RbtnOn");
        }
      } else {
        var elem;
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = "--";
        }
      }
    }
  };
  xhttp.open("GET", "/data", true);
  xhttp.send();
  setTimeout(update, 2500);
};
update();
</script>

</html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void handleGetData() {
  Serial.println("handleGetData");
  server.sendHeader("Cache-Control", "no-cache");
  String json = "[";
  for (uint8_t i=0; i<4; i++) {
    if (i>0)
      json += ",";
    json += "\"" + fmtTemperature(main::temps[i], 1) + "\"";
  }
  for (uint8_t i=0; i<4; i++) {
    bool relOn = digitalRead(main::ra[i].getPin()) == REL_ON;
    json += "," + String(relOn);
  }
  json += "]";
  server.send(200, "text/plain", json);
}

void setup() {
  WiFi.softAP(ssid, password);
  webServerIP = WiFi.softAPIP();
  server.on("/", handleRoot);
  server.on("/data", handleGetData);
}

void begin() {
  server.begin();
}

void loop() {
  server.handleClient();
}

} //namespace

#endif
