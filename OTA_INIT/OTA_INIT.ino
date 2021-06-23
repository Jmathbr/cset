#include <WiFi.h>
#include <SPIFFS.h>

#include <HTTPUpdate.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"

#define LED 2

const char*   SSID      = "brisa-1386277";
const char*   PASSWORD  = "ledafyje";

const char*   VCS_URL   = "https://cest.imd.ufrn.br/cest-api/firmware/last-update";
const char*   auth_url = "https://cest.imd.ufrn.br/cest-api/auth";

const char* token;
const uint8_t* data_cloud;
float v_cloud_value;
float v_local_value = 13;

void setup() {
//==================================================
  // INIT
  Serial.begin(115200);
  Serial.println("\nINIT");
  pinMode(LED, OUTPUT);
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
 
//HTTP POST + JSON = AUTHENTIC
 HTTPClient http; //Declarado o HTTP, nao precisa redeclarar 
 http.begin(auth_url);
 http.addHeader("Content-Type", "application/json");
 
 DynamicJsonDocument doc(1024);
 StaticJsonDocument<64> auth;

 auth["username"] = "admin";
 auth["password"] = "admin";
 
 String httpRequestData;
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

//HTTP GET  + JSON
  http.begin(VCS_URL);
  http.addHeader("Authorization",mytoken);
  int httpCode = http.GET();
  String payload = http.getString();
  
  //Serial.println("External: " + httpCode);
  //Serial.println("External: " + payload);
  
  deserializeJson(doc, payload);
  JsonObject obj = doc.as<JsonObject>();
  
  String data_cloudd = payload;
  String Url_skecth = doc["swURL"];
  String v_cloud = doc["swVersion"];
  String Url_spiffs = doc["fsURL"];

  v_cloud_value = v_cloud.toFloat() *10;
  Serial.println(v_cloud_value);
    
  Serial.println("External: Versao do Firmware: Cloud: " + v_cloud);
  Serial.println("External: Url Firmware: Cloud: " + Url_skecth);
  http.end();
  
//=============================================================

//Version Control - compare(Local , Cloud)

  if(v_cloud_value == v_local_value){   // Versoes iguais
  
  Serial.println("Nao existe atualizaçao de Firmware");
  
  } else{   // Versao da nuvem superior ao firmware
  
    if (SPIFFS.begin(true)) {
      Serial.println("SPIFFS Ok");
    } else {
      Serial.println("SPIFFS Falha");
    }
  
    Serial.println("Existe atualizando Firmware:");

    Serial.print("sobrescrevendo: vcs.txt");
  
    httpUpdate.setLedPin(2, HIGH);

  // Callback - Progresso
    Update.onProgress([](size_t progresso, size_t total) {
    Serial.print(progresso * 100 / total);
    Serial.print(" ");
    });

  // Não reiniciar automaticamente
    httpUpdate.rebootOnUpdate(false);

  // Cria instância de Cliente seguro
    WiFiClientSecure client;

  // Instrui Cliente a ignorar assinatura do Servidor na conexao segura
    client.setInsecure();

  // Atualização SPIFFS ---------------------------------
    Serial.println("\n*** Atualização da SPIFFS ***");
    for (byte b = 5; b > 0; b--) {
      Serial.print(b);
      Serial.println("... ");
      delay(1000);
    }
  // Encerra SPIFFS
    SPIFFS.end();

  // Efetua atualização da SPIFFS
    t_httpUpdate_return resultado = httpUpdate.updateSpiffs(client, Url_spiffs);
  
  // Verifica resultado
    switch (resultado) {
      case HTTP_UPDATE_FAILED: {

        String s = httpUpdate.getLastErrorString();
      
        Serial.println("\nFalha: " + s);
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      } 
      break;
    
      case HTTP_UPDATE_NO_UPDATES: {
        Serial.println("\nNenhuma atualização disponível");
      } 
      break;
    
      case HTTP_UPDATE_OK: {
        Serial.println("\nSucesso");
      } 
      break;
      }


  // Atualização Sketch ---------------------------------
    Serial.println("\n*** Atualização do sketch ***");
    for (byte b = 5; b > 0; b--) {
      Serial.print(b);
      Serial.println("... ");
      delay(1000);
    }

  // Efetua atualização do Sketch

  // ESP32
    resultado = httpUpdate.update(client, Url_skecth);
  
    switch (resultado) {
      case HTTP_UPDATE_FAILED: {
      
      // ESP32
        String s = httpUpdate.getLastErrorString();
      
        Serial.println("\nFalha: " + s);
      } break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("\nNenhuma atualização disponível");
        break;
      case HTTP_UPDATE_OK: {
        Serial.println("\nSucesso, reiniciando...");
        //ESP.restart();
        break;
      } 
    }
  Serial.println("Resetando em 10s...");
  for(int i = 10;i>0;i--){
    Serial.print(i);
    delay(1000);
    ESP.restart();
  }
  
  };

}

void loop() {
  // put your main code here, to run repeatedly:

}
