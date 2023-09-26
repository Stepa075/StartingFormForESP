#define INIT_ADDRES_CELL 1023 // номер резервной ячейки в EEPROM
#define INIT_EEPROM_KEY 50  // ключ первого запуска. 0-254, на выбор
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "11111111"
#endif

int resetKey = 0;   //  Переменная сброса настроек и восстановления первого запуска, пишется туда же, куда и INIT_EEPROM_KEY
uint8_t readResetKey = 0;
struct FirstStruct{
  String ssidClient = "";   //  Переменная имени точки доступа, которая запишется в EEPROM
  String passClient = "";   //  Переменная пароля к точке доступа, которая запишется в EEPROM
};
// глобальный экземпляр для личного использования
FirstStruct firstStruct;

String plaseholder_ssid = "ssid";
String plaseholder_pass = "password";
/* Set these to your desired credentials. */
/* Создаем точку доступа и запускаем  web-сервер на ней*/
const char *ssid = APSSID;
const char *password = APPSK;
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  EEPROM.begin(4096);  // для esp8266/esp32
  if (EEPROM.read(INIT_ADDRES_CELL) != INIT_EEPROM_KEY) { // первый запуск
    EEPROM.write(INIT_ADDRES_CELL, INIT_EEPROM_KEY); // записали ключ
    Serial.println("First start EEPROM!!");
    EEPROM.commit();    // для esp8266/esp32
  }
  Serial.println();
  EEPROM.get(1023, readResetKey);
  Serial.println("Read reset key = " + String(readResetKey));
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/ok", handleRootOk);
  server.on("/1", handleRoot1);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
   server.handleClient();
   delay(1000);
}

void handleRoot() {
  server.send(200, "text/html", Send(plaseholder_ssid, plaseholder_pass));  //"<h1>You are connected</h1>" + String(stringOne)
}
void handleRootOk() {
  firstStruct.ssidClient = server.arg("wifi_ssid");
  firstStruct.passClient = server.arg("wifi_pass");
  server.send(200, "text/html", "<h1>" + String(firstStruct.ssidClient) + "</h1><br><h1>" + String(firstStruct.passClient) + "</h1>");  //"<h1>You are connected</h1>" + String(stringOne)
  write_to_EEPROM();
}

void handleRoot1() {
  server.send(200, "text/html", "<h1>Light: " + String(APSSID) + "</h1>");  //"<h1>You are connected</h1>" + String(stringOne)
}

void handle_NotFound() {
  server.send(404, "text/plain", "Page not found");
}

String Send(String plaseholder_ssid, String plaseholder_pass) {
    
    String webPage = "<!DOCTYPE html><html><body>";
    webPage += "<h2>Settings Form</h2>";
    webPage += "<form action=\"/ok\"";
    webPage += "<label for=\"wifi_ssid\">You WIFI ssid:</label><br>";
    webPage += "<input type=\"text\" id=\"wifi_ssid\" name=\"wifi_ssid\" placeholder=" + String(plaseholder_ssid) + "><br>";

    webPage += "<label for=\"wifi_pass\">You WIFI password:</label><br>";
    webPage += "<input type=\"text\" id=\"wifi_pass\" name=\"wifi_pass\" placeholder=" + String(plaseholder_pass) + "><br><br>";

    webPage += "<input type = \"submit\" value = \"Accept parametrs\">";
    webPage += "</form></body></html>";
    
    return webPage;
}



String SendHTML(uint16_t lux, uint16_t luxOn, uint16_t luxOff, String onOff) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  // ptr += <meta http-equiv="refresh" content="2">\n;
  ptr += "<title>Light control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #1abc9c;}\n";
  ptr += ".button-on:active {background-color: #16a085;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 20px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>Light sensor Web Server</h1>\n";
  ptr += "<h3>Using Access Point(AP) Mode</h3>\n";

  ptr += "<h3> The light is: " + String(onOff) + "</h3>\n";

  
  ptr += "<p> On light sensor data: " + String(luxOn) + " lux</p>\n";
  ptr += "<p> Current sensor data: " + String(lux) + " lux</p>\n";

  ptr += "<p> Off light sensor data: " + String(luxOff) + " lux</p>\n";
  

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void write_to_EEPROM(){
  EEPROM.put(0, firstStruct);
  EEPROM.commit();

}









