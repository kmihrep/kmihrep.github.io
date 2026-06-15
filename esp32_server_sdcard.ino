#include "WiFi.h"
#include "WebServer.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

const char *ssid = "ESP32_server";
const char *password = "12345678";

WebServer server(80);

bool loadFromSDCard(String path, String mimeType) {
  if (path == "/") {
    path = "/index.html";
  }

  if (SD.exists(path)) {
    File dataFile = SD.open(path, "r");
    server.streamFile(dataFile, mimeType);
    dataFile.close();
    return true;
  }
  
  return false;
}

void handleRoot() {
  if (!loadFromSDCard("/index.html", "text/html")) {
    server.send(404, "text/plain", "404: index.html missing from SD Card!");
  }
}

void handleFileRequest() {
  String path = server.uri();
  String dataType = "text/plain";
  
  if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".css"))  dataType = "text/css";
  else if (path.endsWith(".png"))  dataType = "image/png";
  else if (path.endsWith(".jpg"))  dataType = "image/jpeg";

  if (!loadFromSDCard(path, dataType)) {
    server.send(404, "text/plain", "404: File Not Found");
  }
}

void handleSave() {
  if (server.hasArg("userInput")) {
    String textToSave = server.arg("userInput") + "\n";
    File file = SD.open("/user_input.txt", FILE_APPEND);
    if (file) {
      file.print(textToSave);
      file.close();
      
      String successPage = "<html><head><link rel='stylesheet' type='text/css' href='/style.css'></head>";
      successPage += "<body><div class='container'><h3>Data Logged!</h3><br><a href='/'>Go Back</a></div></body></html>";
      server.send(200, "text/html", successPage);
    } else {
      server.send(500, "text/plain", "SD Write Error");
    }
  }
}

void setup() {
  Serial.begin(115200);

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Mount Failed!");
    while (true);
  }
  Serial.println("SD Card Loaded.");

  WiFi.softAP(ssid, password);
  Serial.print("Server Live at: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/save", handleSave);
  
  server.onNotFound(handleFileRequest); 

  server.begin();
}

void loop() {
  server.handleClient();
}