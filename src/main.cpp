#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>


int x = 0;
WebSocketsServer webSocket = WebSocketsServer(81);
WebServer server(80);

void test_function() {
  x++;
  Serial.println(x);
  digitalWrite(2, !digitalRead(2));
}

void webSocketEvent(
  uint8_t client_num,
  WStype_t type,
  uint8_t* payload,
  size_t length
) {
  if (type == WStype_DISCONNECTED) {
    Serial.println("Browser disconnected");
  }
}


void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);

  WiFi.softAP("EGOR", "l0new0lf");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (!LittleFS.begin(true)) {
    Serial.println("File system faled");
    return;
  }

  server.on("/", []() {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
      server.send(500, "text/plain", "Can't open index.html");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();}
  );

  server.on("/test_function", []() {
    test_function();
    server.send(200, "text/plain", String(x));}
  );

  server.begin();
  Serial.println("Web up");

}
void loop() {
  webSocket.loop();
  server.handleClient();
}