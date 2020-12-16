#ifndef WEB_H
#define WEB_H

#include <ESP8266WebServer.h>

namespace webserver {

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

String fmtWeight(float val, int decPlc) {
  return fmtFloatValue(val, decPlc, " g");
}

void handleRoot() {
  if (!server.authenticate(WWWUSER, WWWPWD)) {
    return server.requestAuthentication();
  }
  
  const char html[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>Duremar 4</title>
<link rel="stylesheet" href="main.css">
<script src="main.js"></script>
</head>
<body>
<table>
<tr><td>T1</td><td id="t0">--</td></tr>
<tr><td>T2</td><td id="t1">--</td></tr>
<tr><td>T3</td><td id="t2">--</td></tr>
<tr><td>T4</td><td id="t3">--</td></tr>
<tr><td>Wt</td><td id="w0">--</td></tr>
</table>
<div>
  <div class="Rbtn" id="r0" onclick="bclk(0)">R1<div id="m0">--</div></div>
  <div class="Rbtn" id="r1" onclick="bclk(1)">R2<div id="m1">--</div></div>
  <div class="Rbtn" id="r2" onclick="bclk(2)">R3<div id="m2">--</div></div>
  <div class="Rbtn" id="r3" onclick="bclk(3)">R4<div id="m3">--</div></div>
</div>
<div>
© trad00@yandex.ru
</div>
</body>
<script type="text/javascript">
  update();
</script>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleCSS() {
  const char text[] = R"rawliteral(
table {
  background: white;
  border: 0px solid black;
  -moz-user-select: none;
  -khtml-user-select: none;
  -webkit-user-select: none;
  user-select: none;
}
td {
  font-size: 50pt;
  font-family: Verdana;
  padding: 10px 30px;
  text-align: center;
}
.Rbtn {
  display: block;
  font-size: 50pt;
  font-family: Verdana;
  text-align: center;
  vertical-align: middle;
  width: 350px;
  padding: 20px 20px;
  margin: 5px 5px;
  color: white;
  background-color: gray;
  cursor: pointer;
  -moz-user-select: none;
  -khtml-user-select: none;
  -webkit-user-select: none;
  user-select: none;
}
.RbtnOn {
  background-color: #43a209;
}
  )rawliteral";
  server.send(200, "text/css", text);
}

void handleJS() {
  const char text[] = R"rawliteral(
function bclk(i) {
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "/switch", true);
  xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {
      update();
    }
  }
  xhttp.send("i="+i);
}
function update() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == XMLHttpRequest.DONE) {
      if (this.status == 200) {
        var data = JSON.parse(this.responseText);
        var elem;
        for (let i = 0; i < 4; i++) {
          
          elem = document.getElementById("r"+i);
          var relay_on = data[i*2+4];
          if (relay_on == 0)
            elem.classList.remove("RbtnOn");
          else
            elem.classList.add("RbtnOn");
          
          elem = document.getElementById("m"+i);
          var relay_mode = data[i*2+5];
          if (relay_mode == 0)
              elem.innerHTML = "AUTO";
          else if (relay_mode == 1)
              elem.innerHTML = "ON";
          else
              elem.innerHTML = "OFF";
        }
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = data[i];
        }
        elem = document.getElementById("w0");
        elem.innerHTML = data[12];
      } else {
        var elem;
        for (let i = 0; i < 4; i++) {
          elem = document.getElementById("t"+i);
          elem.innerHTML = "--";
          elem = document.getElementById("m"+i);
          elem.innerHTML = "--";
        }
        elem = document.getElementById("w0");
        elem.innerHTML = "--";
      }
    }
  };
  xhttp.open("GET", "/data", true);
  xhttp.send();
  setTimeout(update, 2500);
};
  )rawliteral";
  server.send(200, "text/javascript", text);
}

void handleGetData() {
  server.sendHeader("Cache-Control", "no-cache");
  String json = "[";
  for (uint8_t i=0; i<4; i++) {
    if (i>0)
      json += ",";
    json += "\"" + fmtTemperature(main::getTempSensor(i), 1) + "\"";
  }
  for (uint8_t i=0; i<4; i++) {
    main::RelayActuator &ra = main::ra[i];
    json += "," + String(ra.STATUS());
    json += "," + String(ra.getMode());
  }
  json += ",\"" + fmtWeight(main::readWeight(), 0) + "\"";
  json += "]";
  server.send(200, "text/plain", json);
}

void handleSwitch() {
  uint8_t i = server.arg("i").toInt();
  main::RelayActuator &ra = main::ra[i];
  ra.shiftMode();
  server.send(200);
}

void setup() {
  server.on("/", handleRoot);
  server.on("/main.css", handleCSS);
  server.on("/main.js", handleJS);
  server.on("/data", handleGetData);
  server.on("/switch", handleSwitch);
}

void begin() {
  server.begin();
}

void loop() {
  server.handleClient();
}

} //namespace

#endif
