#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

const char* ssid = "Wokwi-GUEST";
const char* password = "";

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
String menuItems[10]; // Can hold up to 10 menu items
int menuItemIds[10];  // To store item IDs from the API
float menuItemPrices[10]; // To store item prices from the API
int numMenuItems = 0; // Actual number of items fetched
int currentMenuIndex = 0;
int quantity = 1;
bool inQuantityMode = false;
bool orderSubmitted = false;

// Cart structure
struct CartItem {
  int item_id;
  int quantity;
};
CartItem cartItems[10]; // Array to store cart items
int cartSize = 0;       // Number of items in the cart
String cart = "";       // For display purposes

// Button state variables
bool lastButton1State = HIGH;
bool lastButton2State = HIGH;
unsigned long button2PressTime = 0;
const unsigned long longPressDuration = 2000; // 2 seconds for long press

void fetchMenuItems() {
  HTTPClient http;
  http.begin("https://techathon-backend-nine.vercel.app/menu_items"); 
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println("Response payload:");
    Serial.println(payload);

    const size_t capacity = 2048; // Increase if payload grows
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      JsonArray array = doc.as<JsonArray>();
      numMenuItems = 0;
      for (JsonVariant item : array) {
        if (numMenuItems < 10) {
          menuItems[numMenuItems] = item["name"].as<String>();
          menuItemIds[numMenuItems] = item["item_id"].as<int>();
          menuItemPrices[numMenuItems] = item["price"].as<float>(); // Store price
          numMenuItems++;
        }
      }
    } else {
      Serial.println("Failed to parse JSON!");
    }
  } else {
    Serial.print("HTTP GET failed. Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void submitOrder() {
  if (cartSize == 0) {
    Serial.println("Cart is empty, cannot submit order.");
    return;
  }

  HTTPClient http;
  http.begin("https://techathon-backend-nine.vercel.app/orders");
  http.addHeader("Content-Type", "application/json");

  // Create JSON payload
  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(cartSize) + cartSize * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);

  doc["table_id"] = 3; // Hardcoded as in the image
  JsonArray items = doc.createNestedArray("items");
  for (int i = 0; i < cartSize; i++) {
    JsonObject item = items.createNestedObject();
    item["item_id"] = cartItems[i].item_id;
    item["quantity"] = cartItems[i].quantity;
  }

  String payload;
  serializeJson(doc, payload);
  Serial.println("Submitting order with payload:");
  Serial.println(payload);

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Order submission response:");
    Serial.println(response);

    DynamicJsonDocument responseDoc(512);
    DeserializationError error = deserializeJson(responseDoc, response);
    if (!error) {
      String message = responseDoc["message"].as<String>();
      int orderId = responseDoc["order_id"].as<int>();
      if (message == "Order placed successfully") {
        orderSubmitted = true;
        Serial.print("Order placed successfully. Order ID: ");
        Serial.println(orderId);
      }
    } else {
      Serial.println("Failed to parse response JSON!");
    }
  } else {
    Serial.print("HTTP POST failed. Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");

  fetchMenuItems();

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed - check I2C connections or address"));
    for (;;);
  }
  Serial.println("OLED initialized");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Bistro 92 Menu");
  display.display();
  Serial.println("Initial display updated");

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
      // Display item name and price
      display.print(menuItems[i]);
      display.print(" ($");
      display.print(menuItemPrices[i]);
      display.println(")");
    }
    display.println("\nCart:");
    display.println(cart);
  } else {
    display.println("Select Quantity");
    display.print(menuItems[currentMenuIndex]);
    display.print(" ($");
    display.print(menuItemPrices[currentMenuIndex]);
    display.print("): ");
    display.println(quantity);
  }
  display.display();
}

void loop() {
  bool button1State = digitalRead(BUTTON1_PIN);
  bool button2State = digitalRead(BUTTON2_PIN);
  bool button3State = digitalRead(BUTTON3_PIN);
  bool button4State = digitalRead(BUTTON4_PIN);

  if (button1State == LOW && lastButton1State == HIGH) {
    if (orderSubmitted) {
      orderSubmitted = false;
      cartSize = 0;
      cart = "";
    } else if (inQuantityMode) {
      inQuantityMode = false;
      quantity = 1;
    }
  }

  if (button2State == LOW && lastButton2State == HIGH) {
    button2PressTime = millis();
  }
  if (button2State == LOW && lastButton2State == LOW) {
    if (millis() - button2PressTime >= longPressDuration && !orderSubmitted) {
      submitOrder();
      button2PressTime = millis();
    }
  }
  if (button2State == HIGH && lastButton2State == LOW) {
    if (!inQuantityMode && !orderSubmitted) {
      inQuantityMode = true;
    } else if (inQuantityMode && !orderSubmitted) {
      cartItems[cartSize].item_id = menuItemIds[currentMenuIndex];
      cartItems[cartSize].quantity = quantity;
      cartSize++;
      
      cart += menuItems[currentMenuIndex] + ": " + String(quantity) + "\n";
      inQuantityMode = false;
      quantity = 1;
    }
  }

  if (button3State == LOW) {
    if (!inQuantityMode && !orderSubmitted) {
      currentMenuIndex = (currentMenuIndex > 0) ? currentMenuIndex - 1 : numMenuItems - 1;
    } else if (inQuantityMode) {
      quantity++;
      if (quantity > 10) quantity = 10;
    }
    delay(200);
  }

  if (button4State == LOW) {
    if (!inQuantityMode && !orderSubmitted) {
      currentMenuIndex = (currentMenuIndex < numMenuItems - 1) ? currentMenuIndex + 1 : 0;
    } else if (inQuantityMode) {
      quantity--;
      if (quantity < 1) quantity = 1;
    }
    delay(200);
  }

  lastButton1State = button1State;
  lastButton2State = button2State;

  displayMenu();
}