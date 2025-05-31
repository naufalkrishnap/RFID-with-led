#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

const int ledAksesDiterima = 3; 
const int ledAksesDitolak = 4;  

// Daftar UID yang diizinkan
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

  pinMode(ledAksesDiterima, OUTPUT);
  pinMode(ledAksesDitolak, OUTPUT);
  digitalWrite(ledAksesDiterima, LOW);
  digitalWrite(ledAksesDitolak, LOW);
}

void loop() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    String tagId = tag.getUidString();
    String normalizedUID = normalizeUID(tagId);

    Serial.print("UID Terdeteksi: ");
    Serial.println(normalizedUID);

    // Matikan kedua LED dulu sebelum cek akses
    digitalWrite(ledAksesDiterima, LOW);
    digitalWrite(ledAksesDitolak, LOW);

    if (isAllowed(normalizedUID)) {
      Serial.println("✅ Akses diberikan");
      digitalWrite(ledAksesDiterima, HIGH); 
    } else {
      Serial.println("⛔ Akses ditolak");
      digitalWrite(ledAksesDitolak, HIGH); 
    }

    delay(3000); 
    digitalWrite(ledAksesDiterima, LOW);
    digitalWrite(ledAksesDitolak, LOW);
  }

  delay(500); 
}
