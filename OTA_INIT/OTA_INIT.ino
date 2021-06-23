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

String leitura, httpRequestData;
const char* token;
const uint8_t* data_cloud;
float v_cloud_value;
float v_local_value = 12;

void setup() {
  
  // INIT
  Serial.begin(115200);
  Serial.println("\nINIT");
  pinMode(LED, OUTPUT);
  
//==================================================
 DynamicJsonDocument doc(1024);
//Json + txt interno
/*  
  delay(1000);
 
  // Inicializa SPIFFS
  Serial.println(SPIFFS.open("/vcs .txt"));
  
  if (SPIFFS.open("/vcs .txt")) {
    Serial.println("Existe: ");
    File f = SPIFFS.open("/vcs.txt", "r");
    Serial.println("Inside: Lendo arquivo: ");
      //Processo Json
    leitura = f.readString();
    deserializeJson(doc, leitura);
    JsonObject obj = doc.as<JsonObject>();
    Serial.println(leitura);
    String v_local = doc["swVersion"];
    v_local_value = v_local.toFloat();
    //Serial.println(leitura); //EXIBE TODOS OS DADOS DO ARQUIVO LOCAL
    
    Serial.println("Inside: Versao do Firmware: "+v_local);
    f.close();
    SPIFFS.end();
    
  } else {
    //caso o arquivo nao exista, o arquivo é criado.
    Serial.println("Inside: Arquivo não encontrado");
    Serial.println("Inside: Criando arquivo vcs.txt");
    File fw = SPIFFS.open("/vcs.txt", FILE_WRITE);
    fw.write((const uint8_t*)"{\"swVersion\":\"0.0\",\"swRelease\":\"0001-01-00T00:00:00Z\"}",50);
    fw.close();
    delay(1000);
    
    Serial.println(SPIFFS.open("/vcs .txt"));
    File fr = SPIFFS.open("/vcs.txt", "r");
    Serial.println("Inside: Lendo arquivo: ");
      //Processo Json
    leitura = fr.readString();
    deserializeJson(doc, leitura);
    JsonObject obj = doc.as<JsonObject>();
    Serial.println(leitura);
    String v_local = doc["swVersion"];
    v_local_value = v_local.toFloat();
      //Serial.println(leitura); //EXIBE TODOS OS DADOS DO ARQUIVO LOCAL
    
    Serial.println("Inside: Versao do Firmware: "+v_local);
    fr.close();
    SPIFFS.end();
    }
  /*
  /*
  while (true){
    Serial.print("Verificando versao interna:");
    Serial.println(EEPROM.read(0));
    if(EEPROM.read(0)){
      float v_local = EEPROM.read(0);
      Serial.print("Inside: Versao do Firmware: ");
      Serial.println(v_loca);
      }else{
        float value = 1;
        Serial.println("write");
        EEPROM.write(0, value);
        EEPROM.commit();
      }
    
    }
  */

  
  // Verifica / exibe arquivo
  

//==================================================
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
//=============================================================
// Comparador de Versoes 
  Serial.println("dasdasdasd");
  Serial.println(v_cloud_value);
  Serial.println(v_local_value);
  Serial.println(v_cloud_value == v_local_value);
  if(v_cloud_value == v_local_value){   // Versoes iguais
  
  Serial.println("Nao existe atualizaçao de Firmware");
  
  } else{   // Versao da nuvem superior ao firmware
  
    if (SPIFFS.begin()) {
      Serial.println("SPIFFS Ok");
    } else {
      Serial.println("SPIFFS Falha");
    }
  
    Serial.println("Existe atualizando Firmware:");

    Serial.print("sobrescrevendo: vcs.txt");
  /*
  Serial.print(obj);
  File fww = SPIFFS.open("/vcs.txt", "w");
  fww.write(obj);
  fww.close();
  Serial.print("SUB TXT feita:");
  */
  
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
  }
 
 /*String md5 = ESP.getSketchMD5();
 Serial.println(md5);
 Serial.println("Reset");
 ESP.restart();*/
  
  };


 
//String md5 = ESP.getSketchMD5();
//Serial.println(md5);
}

void loop() {
  // put your main code here, to run repeatedly:

}
