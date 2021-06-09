/********************************************************
 * CANAL INTERNET E COISAS
 * OTA - HTTP Update
 * Multiplataforma ESP32 e ESP8266
 * 03/2020 - Andre Michelon
 * andremichelon@internetecoisas.com.br
 * https://internetecoisas.com.br
 * Historico
 * 02/04/2021 Revisao para compatibilidade de WiFiClientSecure
 */

// Bibliotecas ------------------------------------------

  // Bibliotecas para ESP32
#include <WiFi.h>
#include <HTTPUpdate.h>
#include <SPIFFS.h>
#define LED 2

int addr = 0;

// Constantes -------------------------------------------
// Wi-Fi
//SO PRECISA MUDAR O SSID E A SENHA DO WIFI
/*
const char*   SSID      = "brisa-1386277";
const char*   PASSWORD  = "ledafyje";
*/
const char*   SSID      = "Rede Desconectada";
const char*   PASSWORD  = "988641988coelho";

// Setup ------------------------------------------------
void setup() {
  
  pinMode(LED, OUTPUT);

    // Inicialização para ESP32
  Serial.begin(115200);
  
  Serial.println("\nIniciando");

  // Inicializa SPIFFS
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Ok");
  } else {
    Serial.println("SPIFFS Falha");
  }

//===================== Conecta WiFi ===================================== 
  WiFi.begin(SSID, PASSWORD);
  Serial.println("\nConectando WiFi " + String(SSID));
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
//======================================================================== 


//===================== Downlaod da Imagem na Nuvem ===================================== 
  
  // LED indicador de progresso
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
  t_httpUpdate_return resultado = httpUpdate.updateSpiffs(client, "https://raw.githubusercontent.com/Jmathbr/cset/main/OTA-HTTPUpdate.spiffs.bin");
 
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
  resultado = httpUpdate.update(client, "https://raw.githubusercontent.com/Jmathbr/cset/main/Appr/Corrupt.bin");
  // Verifica resultado https://github.com/Jmathbr/cset/raw/main/Appr/Corrupt.bin
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
 
 String md5 = ESP.getSketchMD5();
 Serial.println(md5);
 Serial.println("Reset");
 ESP.restart();
  
}

// Loop --------------------------------------------
void loop() {
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(5000);   

}
