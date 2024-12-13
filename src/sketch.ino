#include <WiFi.h>
#include <HTTPClient.h>

#define GREEN_LED 2 // Pin for green LED control
#define RED_LED 40 // Pin for red LED control
#define YELLOW_LED 9 // Pin for yellow LED control

const int BUTTON_PIN = 18;  // Pin number for the pushbutton
const int LDR_PIN = 4;  // Pin number for the LDR sensor

int buttonState = 0;  // Variable for reading the button status
int ldrThreshold = 600; // Threshold value for the LDR sensor
int buttonPressCount = 0; // Counter for button presses
unsigned long lastDebounceTime = 0; // To handle debounce
unsigned long debounceDelay = 50; // Debounce delay in milliseconds

void setup() {
  // Configure LED pins as OUTPUT
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

  // Configure button pin as INPUT
  pinMode(BUTTON_PIN, INPUT);

  // Initialize LEDs to OFF
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN); // Read the LDR value

  if (ldrValue > ldrThreshold) {
    activateNightMode();
  } else {
    activateDayMode();
  }
  checkButtonPress();

}

void activateNightMode() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 1000) { // Blink yellow LED every second
    previousMillis = currentMillis;
    digitalWrite(YELLOW_LED, !digitalRead(YELLOW_LED)); // Toggle the yellow LED
  }
}

void activateDayMode() {
  // Day mode sequence: green -> yellow -> red
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);
  checkButtonPress();

  digitalWrite(YELLOW_LED, HIGH);
  delay(2000);
  digitalWrite(YELLOW_LED, LOW);

  digitalWrite(RED_LED, HIGH);
  delay(1000);
  checkButtonPress();
  delay(1000);
  checkButtonPress();
  delay(1000);
  checkButtonPress();
  delay(1000);
  checkButtonPress();
  delay(1000);
  checkButtonPress();
  digitalWrite(RED_LED, LOW);
}

void checkButtonPress() {
  Serial.println("check Button");

  int reading = digitalRead(BUTTON_PIN);

  if (reading == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
    Serial.println("Button press!");    
    lastDebounceTime = millis();
    buttonPressCount++;

    if (buttonPressCount == 1) {
      Serial.println("Open!");    
      openTrafficLight();
    } else if (buttonPressCount == 3) {
      Serial.println("Aert!");          
      sendAlert();
      buttonPressCount = 0; // Reset the count
    }
  }
}

void openTrafficLight() {
  delay(1000); // Wait 1 second before opening the traffic light
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(3000);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
}

void sendAlert() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://www.google.com.br/");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.println("Alert sent successfully!");
    } else {
      Serial.print("Error sending alert: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}
