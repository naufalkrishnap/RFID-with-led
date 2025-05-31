#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN A4
#define SCL_PIN A5

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

const int lampuStatus = 3;
const int lampuStatus2 = 4;
const int pinRelay = 5;

String allowedUIDs[] = {
  "17 A9 FA 33", 
  "A0 EB 5D 56",
  "30 05 7F A2"
};

String normalizeUID(String uid) {
  uid.toUpperCase();
  uid.trim();
  return uid;
}

bool isAllowed(String uid) {
  uid = normalizeUID(uid);
  for (int i = 0; i < sizeof(allowedUIDs) / sizeof(allowedUIDs[0]); i++) {
    if (uid == normalizeUID(allowedUIDs[i])) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  Serial.println("System initialized");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("⛔ Tidak dapat menemukan PN532");
    while (1);
  }

  nfc.SAMConfig();

  pinMode(lampuStatus, OUTPUT);
  pinMode(lampuStatus2, OUTPUT);
  pinMode(pinRelay, OUTPUT);

  digitalWrite(lampuStatus, LOW);
  digitalWrite(lampuStatus2, LOW);
  digitalWrite(pinRelay, LOW);
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    // Konversi UID ke string
    String uidStr = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) uidStr += "0";
      uidStr += String(uid[i], HEX);
      if (i < uidLength - 1) uidStr += " ";
    }

    uidStr.toUpperCase();

    Serial.print("UID Terdeteksi: ");
    Serial.println(uidStr);

    if (isAllowed(uidStr)) {
      Serial.println("✅ Akses diberikan");
      digitalWrite(lampuStatus, HIGH);
      digitalWrite(pinRelay, HIGH);
      delay(10000);
      digitalWrite(lampuStatus, LOW);
      digitalWrite(pinRelay, LOW);
    } else {
      Serial.println("⛔ Akses ditolak");
      digitalWrite(lampuStatus2, HIGH);
      delay(1000);
      digitalWrite(lampuStatus2, LOW);
    }
  }

  delay(500); // polling interval
}
