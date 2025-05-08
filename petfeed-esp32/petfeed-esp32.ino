#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "hotspot";
const char* password = "password";

// NTP setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);  // GMT +5:30 (IST)

// Servo setup
Servo myServo;
const int servoPin = 18;
bool servoActuated = false;
unsigned long servoStartTime = 0;
unsigned long servoDuration = 5000; // Default, overwritten by user

// User input
int triggerHour = 0;
int triggerMinute = 0;
int triggerSecond = 0;

void setup() {
  Serial.begin(115200);

  // Attach servo
  myServo.setPeriodHertz(50);
  myServo.attach(servoPin, 500, 2400);
  myServo.write(0);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  // Start NTP
  timeClient.begin();

  // Get trigger time from user
  Serial.println("Enter trigger time and duration in format: HH MM SS DURATION_SECONDS");

  while (Serial.available() == 0) {
    // Wait for user input
  }

  // Read values
  triggerHour = Serial.parseInt();
  triggerMinute = Serial.parseInt();
  triggerSecond = Serial.parseInt();
  int durationSeconds = Serial.parseInt();

  // Convert to milliseconds
  servoDuration = (unsigned long)durationSeconds * 1000;

  Serial.printf("Servo will trigger at %02d:%02d:%02d for %lu milliseconds\n",
                triggerHour, triggerMinute, triggerSecond, servoDuration);
}

void loop() {
  timeClient.update();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  Serial.printf("Time: %02d:%02d:%02d\n", hour, minute, second);

  // Trigger the servo
  if (!servoActuated && hour == triggerHour && minute == triggerMinute && second == triggerSecond) {
    Serial.println("Actuating servo...");
    myServo.write(90);
    servoActuated = true;
    servoStartTime = millis();
  }

  // Reset the servo
  if (servoActuated && millis() - servoStartTime >= servoDuration) {
    Serial.println("Resetting servo...");
    myServo.write(0);
    servoActuated = false;
  }

  delay(500);
}
