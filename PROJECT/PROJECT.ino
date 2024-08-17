#define BUTTON_PIN2 2 // GIOP21 pin connected to button
#define BUTTON_PIN3 3 // GIOP21 pin connected to button

void checkButtonStates(int buttonState2, int buttonState3) {
  if (buttonState2 == LOW && buttonState3 == HIGH) { 
    Serial.println("FIRE!!!");
    while (true);
  }
}
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the pushbutton pin as an pull-up input
  // the pull-up input pin will be HIGH when the switch is open and LOW when the switch is closed.
  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUTTON_PIN3, INPUT_PULLDOWN);
  Serial.println("Setup done");
}

void loop() {
  // read the state of the switch/button:
  int buttonState2 = digitalRead(BUTTON_PIN2);
  int buttonState3 = digitalRead(BUTTON_PIN3);
  // print out the button's state
  Serial.print("Pin 2: ");
  Serial.print(buttonState2);
  Serial.print("\t");
  Serial.print("Pin 3: ");
  Serial.print(buttonState3);
  Serial.println();
  checkButtonStates(buttonState2, buttonState3);
  delay(100);
}


