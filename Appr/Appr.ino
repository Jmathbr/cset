#include <WiFi.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

const char*   SSID      = "Rede Desconectada";
const char*   PASSWORD  = "988641988coelho";
const char*   VCS_URL   = "https://cest.imd.ufrn.br/cest-api/firmware/last-update";
const char*   auth_url = "https://cest.imd.ufrn.br/cest-api/auth";
String leitura, httpRequestData;
const char* token ;

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
  HTTPClient http; //Declarado o HTTP, nao precisa redeclarar 
  http.begin(VCS_URL);
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println("External: " + httpCode);
  Serial.println("External: " + payload);
  deserializeJson(doc, payload);
  String Url = doc["swURL"];
  Serial.println("External: URL FIRMWARE: "+Url);
  http.end();

//==================================================
//==================================================
//HTTP POST + JSON
 http.begin(auth_url);
 http.addHeader("Content-Type", "application/json");
 
 StaticJsonDocument<64> auth;

 auth["username"] = "admin";
 auth["password"] = "admin";

serializeJson(auth, httpRequestData);

 int httpResponseCode = http.POST(httpRequestData);
 String return_post = http.getString();
 http.end();
 
 StaticJsonDocument<256> op;

 DeserializationError error = deserializeJson(op, return_post);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

 token = op["jwt"];
 Serial.print("External: Token: ");
 Serial.println(token);



//==================================================
//String md5 = ESP.getSketchMD5();
//Serial.println(md5);
}

void loop() {
  // put your main code here, to run repeatedly:

}
