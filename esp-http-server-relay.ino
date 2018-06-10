#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "betalab";
const char* password = "deltalab";

//const char* ssid = "gogomil";
//const char* password = "gogomil123";

const char* module_name = "Module 1";

ESP8266WebServer server(80);

const int led = 1;
const int relay = 5;

int relay_state = 0;
int led_state = 0;

void handleRoot() {
  digitalWrite(led, 1);
  String message = "";
  message += "<html>\n";
  message += "  <head>\n";
  message += "    <title>";
  message += module_name;
  message += "</title>\n";
  message += "    <meta http-equiv='refresh' content=\"10; URL='/'\" />\n";
  message += "  </head>\n";
  message += "  <body>\n";
  message += "    <form action='/switch' method='get'>\n";
  message += "      <input type='submit' value='Relay is now ";
  if (relay_state) {
    message += "OFF";
  }
  else {
    message += "ON";
  }
  message += "'>\n";
  message += "    </form>\n";
  message += "    <form action='/reset' method='get'>\n";
  message += "      <input type='submit' value='Reset'>\n";
  message += "    </form>\n";
  message += "  </body>\n";
  message += "</html>";
  server.send(200, "text/html",  message);
  digitalWrite(led, 0);
}

void handleSwitch() {
  digitalWrite(led, 1);
  relay_state = 1 - relay_state;
  digitalWrite(relay, relay_state);
  String message = "";
  message += "<html>\n";
  message += "  <head>\n";
  message += "    <meta http-equiv='refresh' content=\"0; URL='/'\" />\n";
  message += "  </head>\n";
  message += "</html>";
  server.send(200, "text/html",  message);
  digitalWrite(led, 0);
}

void handleReset() {
  digitalWrite(led, 1);
  digitalWrite(relay, 1);
  delay(5000);
  digitalWrite(relay, 0);
  String message = "";
  message += "<html>\n";
  message += "  <head>\n";
  message += "    <meta http-equiv='refresh' content=\"0; URL='/'\" />\n";
  message += "  </head>\n";
  message += "</html>";
  server.send(200, "text/html",  message);
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)? "GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(led, 0);
  digitalWrite(relay, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println();

  connectToWifiNetwork();

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/switch", handleSwitch);

  server.on("/reset", handleReset);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  connectToWifiNetwork();
  server.handleClient();

  digitalWrite(led, led_state);
  led_state = 1 - led_state;
  delay(100);
}

void connectToWifiNetwork() {
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

