#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int entrySensorPin = 2;  // Pin for the entry sensor
int exitSensorPin = 3;   // Pin for the exit sensor
int entryServoPin = 9;   // Pin for the entry servo
int exitServoPin = 10;   // Pin for the exit servo

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create Servo objects
Servo entryServo;
Servo exitServo;

// Parking slot variables
int totalSlots = 3;
int availableSlots = totalSlots;

// Variables to store sensor states
int lastEntrySensorState = LOW;
int lastExitSensorState = LOW;

void setup() {
  // Initialize the sensors
  pinMode(entrySensorPin, INPUT);
  pinMode(exitSensorPin, INPUT);

  // Attach the servos
  entryServo.attach(entryServoPin);
  exitServo.attach(exitServoPin);

  // Initialize Serial communication for debugging
  Serial.begin(9600);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Use I2C address 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Infinite loop if display is not found
  }

  display.display();
  delay(2000); 
}

void loop() {
  // Read the state of the sensors
  int entrySensorValue = digitalRead(entrySensorPin);
  int exitSensorValue = digitalRead(exitSensorPin);
// Check entry sensor
  if (entrySensorValue == HIGH && lastEntrySensorState == LOW) {
    if (availableSlots > 0) {  // Only open the gate if there are available slots
      entryServo.write(0);  // Open gate
      delay(2000);           // Wait for car to pass
      entryServo.write(90);   // Close gate
      availableSlots--;      // Decrease available slots
      updateDisplay();       // Update OLED display
      Serial.println("Car entered. Gate closed.");
    } else {
      Serial.println("Parking Full. Entry gate remains closed.");
      displayFullMessage();  // Show "Parking Full" message on OLED
    }
    lastEntrySensorState = HIGH;  // Update sensor state
  } else if (entrySensorValue == LOW) {
    lastEntrySensorState = LOW;  // Reset sensor state
  }

  // Check exit sensor
  if (exitSensorValue == HIGH && lastExitSensorState == LOW) {
    if (availableSlots < totalSlots) {  // Only open the exit gate if slots can be freed
      exitServo.write(0);  // Open gate
      delay(2000);          // Wait for car to exit
      exitServo.write(90);   // Close gate
      availableSlots++;     // Increase available slots
      updateDisplay();      // Update OLED display
      Serial.println("Car exited. Gate closed.");
    }
    lastExitSensorState = HIGH;  // Update sensor state
  } else if (exitSensorValue == LOW) {
    lastExitSensorState = LOW;  // Reset sensor state
  }

  delay(100);  // Small delay for stability
}

// Function to update the display with available slots
void updateDisplay() {
  // Clear the display
  display.clearDisplay();
  // Display total slots
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Total Slots: ");
  display.println(totalSlots);

  // Display available slots
  display.setCursor(0, 10);
  display.print("Available: ");
  display.println(availableSlots);
  display.display();
}
void displayFullMessage() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Parking Full!");
  
  display.display();
}
