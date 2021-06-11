#include <WiFi.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

const char*   SSID      = "brisa-1386277";
const char*   PASSWORD  = "ledafyje";
const char*   VCS_URL   = "https://cest.imd.ufrn.br/cest-api/firmware/last-update";
const char*   auth_url = "https://cest.imd.ufrn.br/cest-api/auth";
String leitura, httpRequestData;
const char* token ;
float v_cloud_value, v_local_value;

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
  if (not(SPIFFS.exists("/vcs .txt"))) {
    File f = SPIFFS.open("/vcs.txt", "r");
    if (f) {
      Serial.println("Inside: Lendo arquivo: ");
      //Processo Json
      leitura = f.readString();
      deserializeJson(doc, leitura);
      JsonObject obj = doc.as<JsonObject>();
      //Serial.println(leitura);
      
      String v_local = doc["swVersion"];
      v_local_value = v_local.toFloat();
      //Serial.println(leitura); //EXIBE TODOS OS DADOS DO ARQUIVO LOCAL

      Serial.println("Inside: Versao do Firmware: "+v_local);
      
      f.close();
    }
  } else {
    //caso o arquivo nao exista, o arquivo é criado.
    Serial.println("Inside: Arquivo não encontrado");
    Serial.println("Inside: Criando arquivo vcs.txt");
    
    File fw = SPIFFS.open("/vcs.txt", "w");
    fw.write((const uint8_t*)"{\"swVersion\":\"0.0\",\"swRelease\":\"0001-01-00T00:00:00Z\"}",50);
    fw.close();

  }

//==================================================
//==================================================

//HTTP POST + JSON = AUTHENTIC
 HTTPClient http; //Declarado o HTTP, nao precisa redeclarar 
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
 String mytoken = "Bearer " + String(token);
 Serial.println("----------------------------------------");
 Serial.print("External: Token: ");
 Serial.println(token);
 Serial.println("----------------------------------------");
//==================================================
//==================================================

//HTTP GET  + JSON
  http.begin(VCS_URL);
  http.addHeader("Authorization",mytoken);
  int httpCode = http.GET();
  String payload = http.getString();
  
  //Serial.println("External: " + httpCode);
  //Serial.println("External: " + payload);
  
  deserializeJson(doc, payload);
  
  String Url = doc["swURL"];
  String v_cloud = doc["swVersion"];
  v_cloud_value = v_cloud.toFloat();
  
  Serial.println("External: Versao do Firmware: Cloud: "+v_cloud);
  Serial.println("External: Url Firmware: Cloud: "+Url);
  http.end();

//=============================================================
//=============================================================
// Comparador de Versoes 

if(v_cloud_value == v_local_value){   // Versoes iguais
  
  Serial.println("Nao existe atualizaçao de Firmware");
  
  };

if(v_cloud_value > v_local_value){   // Versao da nuvem superior ao firmware

  Serial.println("Atualizando: ...");
  
  };


 
//String md5 = ESP.getSketchMD5();
//Serial.println(md5);
}

void loop() {
  // put your main code here, to run repeatedly:

}
