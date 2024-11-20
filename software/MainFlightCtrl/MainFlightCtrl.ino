#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// Definicje pinów dla Seeed Xiao ESP32-C3
#define SWITCH_PIN 3
#define KEY_PIN 2
#define ENCODER_A_PIN 4
#define ENCODER_B_PIN 5
#define ENCODER_SW_PIN 10

// Adres I2C wyświetlacza (zazwyczaj 0x27 lub 0x3F)
#define I2C_ADDR 0x27

// Inicjalizacja wyświetlacza LCD I2C (adres, kolumny, wiersze)
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

// Zmienne globalne
int menuIndex = 0;
bool communicationStatus = false;

// BLE
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// UUID dla usługi i charakterystyki BLE
#define SERVICE_UUID        "181A"
#define CHARACTERISTIC_UUID "2A56"

// Funkcje pomocnicze
void displayMenu() {
  lcd.clear();
  if (menuIndex == 0) {
    lcd.print("> Start");
  } else if (menuIndex == 1) {
    lcd.print("> Check Comm");
  }

  if (communicationStatus) {
    lcd.setCursor(15, 1);
    lcd.print("(C)");
  }
}

void startSequence() {
  lcd.clear();
  lcd.print("Waiting for keys");

  // Czekanie na przełączenie obu przełączników
  while (!(digitalRead(SWITCH_PIN) && digitalRead(KEY_PIN))) {
    delay(100);
  }

  // Odliczanie
  for (int i = 10; i >= 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print("Start in: ");
    lcd.print(i);
    lcd.print("s ");
    delay(1000);
  }

  if (deviceConnected) {
    pCharacteristic->setValue("START");
    pCharacteristic->notify();
  }
}

void checkCommunication() {
  if (deviceConnected) {
    pCharacteristic->setValue("KOMUNIKACJA");
    pCharacteristic->notify();

    // Oczekiwanie na odpowiedź (symulowane)
    delay(500);
    communicationStatus = true;
  } else {
    lcd.clear();
    lcd.print("No device conn");
    delay(2000);
  }
}

// Klasa obsługująca BLE serwer
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  pinMode(SWITCH_PIN, INPUT);
  pinMode(KEY_PIN, INPUT);
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);

  // Inicjalizacja I2C i wyświetlacza
  Wire.begin(6, 7);  // SDA = GPIO 6, SCL = GPIO 7
  lcd.init();
  lcd.backlight();

  // Inicjalizacja BLE
  BLEDevice::init("ESP32-C3");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  displayMenu();
}

void loop() {
  static int lastMenuIndex = -1;

  // Obsługa enkodera
  static int lastEncoderA = LOW;
  int encoderA = digitalRead(ENCODER_A_PIN);

  if (encoderA != lastEncoderA) {
    if (digitalRead(ENCODER_B_PIN) != encoderA) {
      menuIndex++;
    } else {
      menuIndex--;
    }

    if (menuIndex < 0) menuIndex = 1;
    if (menuIndex > 1) menuIndex = 0;
    
    lastEncoderA = encoderA;
  }

  // Wyświetlaj menu, gdy indeks zmienił się
  if (menuIndex != lastMenuIndex) {
    displayMenu();
    lastMenuIndex = menuIndex;
  }

  // Obsługa kliknięcia enkodera
  if (digitalRead(ENCODER_SW_PIN) == LOW) {
    delay(200);  // Debounce
    if (menuIndex == 0) {
      startSequence();
    } else if (menuIndex == 1) {
      checkCommunication();
    }

    displayMenu();
  }
}
