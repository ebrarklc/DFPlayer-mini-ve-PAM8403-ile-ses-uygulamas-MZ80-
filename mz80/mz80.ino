#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// DFPlayer bağlantıları
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// MZ80 ve LED bağlantıları
const int sensorPin = 2;   // MZ80 sensör pini
const int ledPin = 13;     // Harici LED pini

// Sensör ve ses durumu takibi
int lastSensorState = -1;  // Önceki sensör durumu (başlangıçta bilinmiyor)
bool isPlaying = false;

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

  pinMode(sensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.println("MZ80 + DFPlayer Başlatılıyor...");

  delay(1000); // DFPlayer için bekleme

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer başlatılamadı!");
    while (true); // Dur
  }

  Serial.println("DFPlayer Mini bağlandı.");
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(25); // Ses seviyesi: 0-30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  // Başlangıçta bir ses çalma yok
}

void loop() {
  int sensorValue = digitalRead(sensorPin);

  // Engel var → LOW, Engel yok → HIGH
  if (sensorValue != lastSensorState) {
    lastSensorState = sensorValue;

    if (sensorValue == LOW) {
      // ENGEL VAR
      Serial.println("ENGEL VAR - LED YANDI - 2. SES ÇALIYOR");
      digitalWrite(ledPin, HIGH);
      myDFPlayer.play(2);
      isPlaying = true;
    } else {
      // ENGEL YOK
      Serial.println("ENGEL YOK - LED SÖNDÜ - 1. SES ÇALIYOR");
      digitalWrite(ledPin, LOW);
      myDFPlayer.play(1);
      isPlaying = true;
    }
  }

  // Oynatma tamamlandı mı kontrol et
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    if (type == DFPlayerPlayFinished && isPlaying) {
      isPlaying = false;
      Serial.println("Şarkı bitti.");
    }
  }

  delay(100); // Aşırı hızlı okuma olmasın
}
