// Import library untuk ESP32
#include <WiFi.h>
// Import library untuk DF Player
#include <DFRobotDFPlayerMini.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Konfigurasi WiFi
const char* ssid = "OPPO A77s";   
const char* password = "11111111";
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

#define FPSerial Serial1
DFRobotDFPlayerMini myDFPlayer;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

const char *downloadURL = "https://admin.voiceconvert.id/assets/voices/solovoice1";

// Pin sensor PIR
int pirPin = 18; // Sesuaikan dengan pin yang Anda gunakan
bool motionDetected = false;
int btnGPIO = 0;
int btnState = false;

void setup() {
  // Mulai Serial Monitor
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/26, /*tx =*/27);
  Serial.begin(115200); 
  
  // Menghubungkan ke WiFi
  wifi();
  delay(2000);
  timeClient.begin();
  delay(2000);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30

  pinMode(pirPin, INPUT);
}

void loop() {
  timeClient.update();

  if (timeClient.getHours() % 1 == 0 && timeClient.getMinutes() == 0) {
    downloadFile();
  }
  delay(1000);
  // Baca nilai sensor PIR
  int pirValue = digitalRead(pirPin);

  // Cek jika gerakan terdeteksi
  if (pirValue == HIGH) {
    Serial.println("Gerakan terdeteksi!");
    playSound();
  }else {
    Serial.println("Tidak ada gerakan.");
  }
  delay(100000); // Delay sebelum membaca lagi sensor
}

void wifi(){
    // Set GPIO0 Boot button as input
    pinMode(btnGPIO, INPUT);

    // We start by connecting to a WiFi network
    // To debug, please enable Core Debug Level to Verbose

    Serial.println();
    Serial.print("[WiFi] Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    
    int tryDelay = 500;
    int numberOfTries = 20;
    
    while (true) {
        
        switch(WiFi.status()) {
          case WL_NO_SSID_AVAIL:
            Serial.println("[WiFi] SSID not found");
            break;
          case WL_CONNECT_FAILED:
            Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
            return;
            break;
          case WL_CONNECTION_LOST:
            Serial.println("[WiFi] Connection was lost");
            break;
          case WL_SCAN_COMPLETED:
            Serial.println("[WiFi] Scan is completed");
            break;
          case WL_DISCONNECTED:
            Serial.println("[WiFi] WiFi is disconnected");
            break;
          case WL_CONNECTED:
            Serial.println("[WiFi] WiFi is connected!");
            Serial.print("[WiFi] IP address: ");
            Serial.println(WiFi.localIP());
            return;
            break;
          default:
            Serial.print("[WiFi] WiFi Status: ");
            Serial.println(WiFi.status());
            break;
        }
        delay(tryDelay);
        
        if(numberOfTries <= 0){
          Serial.print("[WiFi] Failed to connect to WiFi!");
          // Use disconnect function to force stop trying to connect
          WiFi.disconnect();
          return;
        } else {
          numberOfTries--;
        }
    }
}

void playSound() {
  myDFPlayer.play(1);  //Play the first mp3
  delay(5000);
  myDFPlayer.play(2);
  delay(5000);
}

void downloadFile() {
  HTTPClient http;

  Serial.print("Downloading file from: ");
  Serial.println(downloadURL);

  http.begin(downloadURL);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("File downloaded successfully!");
      // Lakukan sesuatu dengan payload (misalnya, simpan ke SD card atau tampilkan di Serial Monitor)
    } else {
      Serial.printf("HTTP error code: %d\n", httpCode);
    }
  } else {
    Serial.println("Failed to connect to server.");
  }
  http.end();
}
