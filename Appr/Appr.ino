#include <WiFi.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include"esp_ota_ops.h"
#include "esp_partition.h"
//#include "Hash.h"

const char*   SSID      = "brisa-1386277";
const char*   PASSWORD  = "ledafyje";
const char*   VCS_URL   = "https://cest.imd.ufrn.br/cest-api/firmware/last-update";
const char*   auth_url = "https://cest.imd.ufrn.br/cest-api/auth";
String leitura;

void setup() {
  // INIT
  Serial.begin(115200);
  Serial.println("\nINIT");

  
//==================================================  
// Conecta WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConectando WiFi " + String(SSID));
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  WiFiClientSecure client;
  client.setInsecure();
  
//==================================================
//Json + txt interno
  DynamicJsonDocument doc(1024);
  // Inicializa SPIFFS
  if (SPIFFS.begin()) {
    Serial.println("Inside: SPIFFS Ok");
  } else {
    Serial.println("Inside: SPIFFS Falha");
  }
  
  // Verifica / exibe arquivo
  if (SPIFFS.exists("/vcs.txt")) {
    File f = SPIFFS.open("/vcs.txt", "r");
    if (f) {
      Serial.println("Interno: Lendo arquivo:");
      //Processo Json
      leitura = f.readString();
      deserializeJson(doc, leitura);
      JsonObject obj = doc.as<JsonObject>();

      String vers = doc["swVersion"];
      
      Serial.println(leitura);

      Serial.println("Inside: Versao do Firmware:"+vers);
      
      f.close();
    }
  } else {
    Serial.println("Inside: Arquivo não encontrado");
  }
  
//==================================================
//HTTP GET + JSON
 // HTTPClient http;
 // http.begin(VCS_URL);
 // int httpCode = http.GET();
 // String payload = http.getString();
  //Serial.println("External: " + httpCode);
 // Serial.println("External: " + payload);
 // deserializeJson(doc, payload);
 // String Url = doc["swURL"];
 // Serial.println("External: URL FIRMWARE: "+Url);
 // http.end();

  
//==================================================
//==================================================
//HTTP POST + JSON
 HTTPClient http;
 http.begin(auth_url);
 http.addHeader("Content-Type", "text/plain");
String httpRequestData = "{\"password\":\"admin\" , \"username\":\"admin\"}";
  deserializeJson(doc, httpRequestData);
  JsonObject obj = doc.as<JsonObject>();

 int httpResponseCode = http.POST(httpRequestData);
 String payload = http.getString();
  
 Serial.println("External: " + payload);
 Serial.println(httpResponseCode);

 http.end();

//==================================================
//String md5 = ESP.getSketchMD5();
//Serial.println(md5);
}

void loop() {
  // put your main code here, to run repeatedly:

}
