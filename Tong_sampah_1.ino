#define BLYNK_TEMPLATE_ID "TMPL6MI_EkAc3"  // Mendefinisikan ID template Blynk
#define BLYNK_TEMPLATE_NAME "smart bin monitoring"  // Mendefinisikan nama template Blynk
#define BLYNK_AUTH_TOKEN "oN2nvCpFjHNseQXyHvl74uy_lCmLXb4L"  // Mendefinisikan token otentikasi Blynk

#include <Wire.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "oN2nvCpFjHNseQXyHvl74uy_lCmLXb4L"; // Token otentikasi Blynk
char ssid[] = "smartbin"; // Nama SSID WiFi
char pass[] = "11333356789"; // Password WiFi   
  
#define BLYNK_FIRMWARE_VERSION "0.1.1" // Versi firmware Blynk yang digunakan
#define BLYNK_PRINT Serial // Output debug Blynk ke Serial

#define APP_DEBUG // Mode debug aplikasi

// Sensor Pendeteksi Objek
#define TRIG_PIN1  25 // Pin trigger sensor pendeteksi objek
#define ECHO_PIN1  26 // Pin echo sensor pendeteksi objek
#define SERVO_PIN 14 // Pin kontrol servo

// Sensor Kapasitas
#define TRIG_PIN2  32 // Pin trigger sensor kapasitas
#define ECHO_PIN2  33 // Pin echo sensor kapasitas

LiquidCrystal_I2C lcd(0x27, 16, 2); // Objek LCD menggunakan I2C

Servo myservo; // Objek servo
double SetDelay, Input, Output, ServoOutput;

void setupServo() {
  myservo.attach(SERVO_PIN); // Menghubungkan servo ke pin
}

void moveServo(double servoPosition) {
  myservo.write(servoPosition); // Menggerakkan servo ke posisi tertentu
}

void setDelay(double servoPosition) {
  if (servoPosition == 90) {
    SetDelay = 1000; // Menetapkan waktu jeda jika servo berada pada posisi 90 derajat
  } else {
    SetDelay = 1000; // Menetapkan waktu jeda jika servo bukan pada posisi 90 derajat
  }
  delay(SetDelay); // Menunda eksekusi program
}

float readObjectDistance() {
  delay(1);
  // Membaca jarak dari sensor pendeteksi objek
  digitalWrite(TRIG_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN1, LOW);
  long j = pulseIn(ECHO_PIN1, HIGH);
  float jarak_objek = j / (29 * 2);
  
  return jarak_objek; // Mengembalikan jarak objek
}

BlynkTimer timer;
long tinggi = 19; // Tinggi bin (cm)
bool servoEnabled = true; // Flag untuk mematikan/mengaktifkan servo

void capacity() {
  float objectDistance = readObjectDistance();
  
  // Mengkonversi jarak objek menjadi sudut untuk menggerakkan servo
  if (objectDistance > 30) {
    ServoOutput = 0; // Jika jarak lebih dari 10 cm, servo tidak bergerak
  } else {
    ServoOutput = 90; // Jika jarak kurang dari atau sama dengan 10 cm, servo bergerak 180 derajat
  }
  
  if (objectDistance > 0 && servoEnabled) {
    moveServo(ServoOutput); // Menggerakkan servo sesuai posisi yang ditentukan
    setDelay(ServoOutput); // Menetapkan waktu jeda setelah menggerakkan servo
  }
  
  // Membaca jarak dari sensor kapasitas
  digitalWrite(TRIG_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);
  long t = pulseIn(ECHO_PIN2, HIGH);
  long jarak_sampah = t / (29 * 2);
  Serial.println(jarak_sampah);
  double level = tinggi - jarak_sampah;

  lcd.init(); // Inisialisasi LCD
  lcd.backlight(); // Menghidupkan backlight LCD
  lcd.clear(); // Membersihkan tampilan LCD
  
  if (level > 0) {
    long percentage = ((level / tinggi)) * 100; // Menghitung persentase kapasitas
    Blynk.virtualWrite(V0, percentage); // Mengirimkan data persentase ke widget Blynk

    // Update LCD
    lcd.setCursor(0, 0);
    lcd.print("Kapasitas:");
    lcd.setCursor(0, 1);
    lcd.print(percentage);
    lcd.print("%");
    delay(500);
    
    if (percentage >= 90) {
      servoEnabled = false; // Matikan servo jika kapasitas mencapai 90 persen
    } else {
      servoEnabled = true; // Aktifkan servo jika kapasitas di bawah 90 persen
    }
  }
  else {
    Blynk.virtualWrite(V0, 0); // Mengirimkan data persentase 0 ke widget Blynk

    lcd.setCursor(0, 0);
    lcd.print("Smart Trash Bin");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring 1");
  }
}

void setup() {
  pinMode(TRIG_PIN1, OUTPUT); // Mengatur pin sebagai output untuk trigger sensor pendeteksi objek
  pinMode(ECHO_PIN1, INPUT); // Mengatur pin sebagai input untuk echo sensor pendeteksi objek
  pinMode(TRIG_PIN2, OUTPUT); // Mengatur pin sebagai output untuk trigger sensor kapasitas
  pinMode(ECHO_PIN2, INPUT); // Mengatur pin sebagai input untuk echo sensor kapasitas
  Serial.begin(115200); // Memulai komunikasi Serial dengan baud rate 115200
  delay(1000); // Memberi waktu tunggu 1 detik
  Blynk.begin(auth, ssid, pass); // Menghubungkan ke server Blynk dengan token otentikasi dan informasi jaringan WiFi
  timer.setInterval(5L, capacity); // Mengatur interval timer untuk memanggil fungsi capacity setiap 5 milidetik
  setupServo(); // Panggil fungsi setupServo untuk menghubungkan servo
}

void loop() {
  Blynk.run(); // Menjalankan Blynk
  timer.run(); // Menjalankan timer
}
