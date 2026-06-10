#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

// URL de tu base de datos de Firebase (asegúrate de incluir el .json al final)
const char* firebaseUrl = "https://esp32-weatherstation-50cbc-default-rtdb.firebaseio.com/sensor.json";

#define DHTPIN 4
#define DHTTYPE DHT22 // Cambiar a DHT11 si usas ese modelo
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado al Wi-Fi con éxito!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("---------------------------------------");
}

void loop() {
  // 1. Leer los datos del sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verificar si la lectura falló
  if (isnan(h) || isnan(t)) {
    Serial.println("[-] Error crítico: No se pudo leer el sensor DHT.");
    delay(2000);
    return;
  }

  // 2. Mostrar los datos en el Monitor Serial
  Serial.println("[Sensor Local]");
  Serial.print("-> Temperatura: ");
  Serial.print(t, 1); // Imprime con 1 decimal
  Serial.println(" °C");
  Serial.print("-> Humedad:     ");
  Serial.print(h, 1);
  Serial.println(" %");

  // 3. Enviar los datos a Firebase si hay conexión Wi-Fi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(firebaseUrl);
    http.addHeader("Content-Type", "application/json");

    // Creamos el JSON con las lecturas reales
    String httpRequestData = "{\"temperatura\":" + String(t, 2) + ",\"humedad\":" + String(h, 2) + "}";
    
    Serial.println("[Firebase] Enviando datos...");
    int httpResponseCode = http.PUT(httpRequestData); 
    
    if (httpResponseCode > 0) {
      Serial.print("[+] Éxito. Respuesta del servidor: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("[-] Error al enviar. Código: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("[-] Error: Conexión Wi-Fi perdida.");
  }

  Serial.println("---------------------------------------");
  
  // Esperar 10 segundos antes de la próxima lectura
  delay(10000); 
}
