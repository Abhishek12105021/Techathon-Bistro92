#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C pins
#define SDA_PIN 21
#define SCL_PIN 20

// Button pins
#define BUTTON1_PIN 14 // Opens menu / cancels order
#define BUTTON2_PIN 15 // Selects item / adds to cart / submits order (long press)
#define BUTTON3_PIN 16 // Scrolls up / increments quantity
#define BUTTON4_PIN 17 // Scrolls down / decrements quantity

// Menu items
const char* menuItems[] = {"Burger", "Pizza", "Pasta", "Salad"};
const int numMenuItems = 4;
int currentMenuIndex = 0;
int quantity = 1;
bool inQuantityMode = false;
String cart = "";

// Button state variables
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
unsigned long button2PressTime = 0;
const unsigned long longPressDuration = 2000; // 2 seconds for long press
bool orderSubmitted = false;

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Initialize I2C with custom pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed - check I2C connections or address"));
    for (;;); // Halt if display fails
  }
  Serial.println("OLED initialized");

  // Clear and test display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Bistro 92 Menu");
  display.display();
  Serial.println("Initial display updated");

  // Initialize buttons with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
}

void displayMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (orderSubmitted) {
    display.println("Order Submitted!");
    display.println("Press Btn1 to Reset");
  } else if (!inQuantityMode) {
    display.println("Bistro 92 Menu");
    for (int i = 0; i < numMenuItems; i++) {
      if (i == currentMenuIndex) {
        display.print("> ");
      } else {
        display.print("  ");
      }
      display.println(menuItems[i]);
    }
    display.println("\nCart:");
    display.println(cart);
  } else {
    display.println("Select Quantity");
    display.print(menuItems[currentMenuIndex]);
    display.print(": ");
    display.println(quantity);
  }
  display.display();
}

void loop() {
  // Button 1: Open menu or cancel order
  bool button1State = digitalRead(BUTTON1_PIN);
  if (button1State == LOW && lastButton1State == HIGH) {
    cart = "";
    inQuantityMode = false;
    currentMenuIndex = 0;
    quantity = 1;
    orderSubmitted = false;
    displayMenu();
    // Serial.println("Button 1 pressed - reset");
    delay(200); // Debounce
  }
  lastButton1State = button1State;

  // Button 2: Select item / add to cart / submit order (long press)
  bool button2State = digitalRead(BUTTON2_PIN);
  if (button2State == LOW && lastButton2State == HIGH) {
    button2PressTime = millis(); // Record press start time
    // Serial.println("Button 2 pressed");
  }
  if (button2State == LOW && lastButton2State == LOW && !orderSubmitted) {
    // Check for long press
    if (millis() - button2PressTime >= longPressDuration) {
      if (!inQuantityMode && cart != "") {
        Serial.println("Order Submitted to Cloud:");
        Serial.println(cart);
        orderSubmitted = true;
        displayMenu();
      }
      button2PressTime = millis(); // Prevent repeated triggers
    }
  }
  if (button2State == HIGH && lastButton2State == LOW && !orderSubmitted) {
    // Short press
    if (millis() - button2PressTime < longPressDuration) {
      if (!inQuantityMode) {
        inQuantityMode = true;
        quantity = 1;
      } else {
        cart += String(menuItems[currentMenuIndex]) + " x" + String(quantity) + "\n";
        inQuantityMode = false;
        currentMenuIndex = 0;
        Serial.println("Cart Updated: " + cart);
      }
      displayMenu();
    }
  }
  lastButton2State = button2State;

  // Button 3: Scroll up / increment quantity
  if (digitalRead(BUTTON3_PIN) == LOW && !orderSubmitted) {
    if (!inQuantityMode) {
      currentMenuIndex = (currentMenuIndex - 1 + numMenuItems) % numMenuItems;
    } else {
      quantity++;
    }
    displayMenu();
    // Serial.println("Button 3 pressed");
    delay(200); // Debounce
  }

  // Button 4: Scroll down / decrement quantity
  if (digitalRead(BUTTON4_PIN) == LOW && !orderSubmitted) {
    if (!inQuantityMode) {
      currentMenuIndex = (currentMenuIndex + 1) % numMenuItems;
    } else if (quantity > 1) {
      quantity--;
    }
    displayMenu();
    // Serial.println("Button 4 pressed");
    delay(200); // Debounce
  }
}