#define BLYNK_TEMPLATE_ID "TMPL6MI_EkAc3"  // Mendefinisikan ID template Blynk
#define BLYNK_TEMPLATE_NAME "smart bin monitoring"  // Mendefinisikan nama template Blynk
#define BLYNK_AUTH_TOKEN "oN2nvCpFjHNseQXyHvl74uy_lCmLXb4L"  // Mendefinisikan token otentikasi Blynk

#include <WiFi.h>  // Mengimpor library WiFi
#include <WiFiClient.h>  // Mengimpor library WiFiClient
#include <BlynkSimpleEsp32.h>  // Mengimpor library BlynkSimpleEsp32
#include <LiquidCrystal_I2C.h>  // Mengimpor library LiquidCrystal_I2C
#include <Wire.h>  // Mengimpor library Wire

char auth[] = "oN2nvCpFjHNseQXyHvl74uy_lCmLXb4L";  // Menyimpan token otentikasi Blynk
char ssid[] = "smartbin";  // Menyimpan nama jaringan WiFi
char pass[] = "11333356789";  // Menyimpan kata sandi jaringan WiFi

#define BLYNK_FIRMWARE_VERSION "0.1.1"  // Mendefinisikan versi firmware Blynk
#define BLYNK_PRINT Serial  // Mengaktifkan pencetakan melalui Serial

#define APP_DEBUG  // Mengaktifkan mode debug aplikasi

#define trig 32  // Pin output untuk trigger ultrasonik
#define echo 33  // Pin input untuk echo ultrasonik

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Menginisialisasi objek LCD

BlynkTimer timer;  // Membuat objek timer Blynk 
long tinggi = 19;  // Tinggi maksimum bin (dalam cm)

void capacity() {
  digitalWrite(trig, LOW);  // Mengatur pin trigger ultrasonik ke LOW
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);  // Mengatur pin trigger ultrasonik ke HIGH
  delayMicroseconds(10);
  digitalWrite(trig, LOW);  // Mengatur pin trigger ultrasonik ke LOW
  long t = pulseIn(echo, HIGH);  // Membaca waktu respon dari echo ultrasonik
  long cm = t / 29 / 2;  // Menghitung jarak dalam cm berdasarkan waktu respon
  Serial.println(cm);  // Mencetak jarak dalam cm melalui Serial Monitor
  double level = tinggi - cm;  // Menghitung tingkat pengisian bin

  lcd.init();  // Menginisialisasi LCD
  lcd.backlight();  // Menghidupkan backlight LCD
  lcd.clear();  // Menghapus konten pada LCD

  if (level > 0) {
    long percentage = ((level / tinggi)) * 100;  // Menghitung persentase pengisian bin
    Blynk.virtualWrite(V1, percentage);  // Mengirim data persentase ke pin virtual V1 pada aplikasi Blynk

    // Update tampilan pada LCD
    lcd.setCursor(0, 0);
    lcd.print("Kapasitas:");
    lcd.setCursor(0, 1);
    lcd.print(percentage);
    lcd.print("%");
  } else {
    Blynk.virtualWrite(V1, 0);  // Mengirim data persentase 0 ke pin virtual V1 pada aplikasi Blynk

    lcd.setCursor(0, 0);
    lcd.print("Smart Trash Bin");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring 2");
  }
}

void setup() {
  pinMode(trig, OUTPUT);  // Mengatur pin trig sebagai output
  pinMode(echo, INPUT);  // Mengatur pin echo sebagai input
  Serial.begin(115200);  // Memulai komunikasi serial dengan kecepatan 115200 bps
  delay(2000);  // Delay 2 detik
  Blynk.begin(auth, ssid, pass);  // Memulai koneksi WiFi dan Blynk
  timer.setInterval(10L, capacity);  // Menjadwalkan fungsi capacity() untuk dieksekusi setiap 10 ms
}

void loop() {
  Blynk.run();  // Menjalankan loop Blynk
  timer.run();  // Menjalankan timer Blynk
}
