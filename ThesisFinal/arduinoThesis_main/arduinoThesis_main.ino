#include <VoiceRecognitionV3.h>
#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <TOTP.h>
#include <time.h>
//the numpad
const byte ROWS = 4; 
const byte COLS = 4; 
//row and pin for keypad
byte rowPins[ROWS] = { 10, 11, 12, 13 }; 
byte colPins[COLS] = { 2, 3, 4, 5 };
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

/* initialize an instance of class NewKeypad */
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 
// the lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// voice recgoniton module 
VR vr (7,8);
uint8_t buf[64];

// the command (TESTING)
enum Command {
  cmd_up = 0,
  cmd_down,
  cmd_left,
  cmd_right,
};

/*
// the gsm
SoftwareSerial SIM800L (3,2);
int irsensor = A0;
int i = 0;
*/

// THE DOOR
int Lock = 13;
void LockedPosition(int locked)
{
  if (locked) digitalWrite(Lock, LOW);
  else digitalWrite(Lock, HIGH);
}

//ARRAY TO STORE USER DATA 
struct User {
  String username;
  String phoneNumber;
  String pin;
};

User users[5];  // array to store user data in this demo it will be limited to 5
int numUsers = 0; 

void setup() {
  vr.begin(9600);
  Serial.begin(115200); //Setting the baud rate
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("your id:");
  for (int i = cmd_up; i <= cmd_right; i++) {
    vr.load(i);
    /*
    if (EEPROM.read(i - 1) != i) {  // Only write if data has changed
      EEPROM.write(i - 1, i);
    }
*/
}
}

const char separator PROGMEM = ';';

void loop() {
  unsigned long startTime = millis();
  while (true) {
    if (Serial.available() > 0) {
      String data = Serial.readStringUntil('\r');
      Serial.println(data);
      user_data(data);
      startTime = millis(); // Reset the start time after receiving data
    } else if (millis() - startTime > 8000) { // If more than 8 seconds have passed with no data
      getid();
      break;
    }
  }
}
//recieving and reading the user data for parsing
void user_data(String data) {
  // recieve format : username;phoneNumber;PIN
  int firstSemicolon = data.indexOf(separator);
  int secondSemicolon = data.indexOf(separator, firstSemicolon + 1);
  if (firstSemicolon != -1 && secondSemicolon != -1) {
    users[numUsers].username = data.substring(0, firstSemicolon);
    users[numUsers].phoneNumber = data.substring(firstSemicolon + 1, secondSemicolon);
    users[numUsers].pin = data.substring(secondSemicolon + 1);
    numUsers++;
    Serial.println(F("OK"));
  } else {
    Serial.println(F("Error: Invalid data format"));
  }
}


// CHECKING USER PIN 
void getid() {
  bool autPin = false;
  while (!autPin) {
    String id = ""; 
    int lid = id.length();
    while (lid < 4) {
      char customKey = keypad.getKey();
      if (customKey) {
        lcd.setCursor(0, 1);
        id = id + customKey;
        lcd.print(id);
        lid = id.length();       
      }
    }
    for (int i = 0; i < numUsers; i++) {
      if (id == users[i].pin) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hello " + users[i].username);
        Serial.println(users[i].username);
        lcd.setCursor(0, 1);
        lcd.print("id " + users[i].pin);
        delay(3000);
        calc_otp();
        autPin = true;
        break;
      }
    }
    if (!autPin) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Incorrect PIN.");
    }
  }
}

// Function to convert a digit to a 4-bit binary string
String digitToBinary(int num) {
  String binary = "";
  for (int i = 3; i >= 0; i--) {
    binary += ((num >> i) & 1);
  }
  return binary;
}

// Function to convert OTP to binary
String Otp_conversion(String otpCode) {
  // Convert each digit of the OTP to a 4-bit binary string
  String binaryOtp = "";
  for (int i = 0; i < otpCode.length(); i++) {
    binaryOtp += digitToBinary(otpCode[i] - '0');  // Subtract '0' to convert char to int
  }  
  return binaryOtp;
}


// BINARY TO DIRECTION CONVERSION
String Bi_to_Direct(String binaryOtp) {
  String directions = "";
  for (int i = 0; i < binaryOtp.length(); i += 4) { 
    String biPair1 = binaryOtp.substring(i, i+2);  // Get the first binary pair
    String biPair2 = binaryOtp.substring(i+2, i+4);  // Get the second binary pair
    if (biPair1 == "00") {
      directions += "up ";
    } else if (biPair1 == "01") {
      directions += "down ";
    } else if (biPair1 == "10") {
      directions += "left ";
    } else if (biPair1 == "11") {
      directions += "right ";
    }
    if (biPair2 == "00") {
      directions += "up ";
    } else if (biPair2 == "01") {
      directions += "down ";
    } else if (biPair2 == "10") {
      directions += "left ";
    } else if (biPair2 == "11") {
      directions += "right ";
    }
  }
  return directions;
}


// GENERATING OTP
const int otpLength = 6;
String otpCode; 
uint8_t secretKey[8];
void generateSecretKey() {
  randomSeed(analogRead(1) + millis() + micros());
  for (int i = 0; i < 8; i++) {
    secretKey[i] = random(0, 256);
  }
}

// Generate a TOTP using the current time and the random secret key
String generateTOTP() {
  // Get the current time in seconds
  unsigned long currentTime = millis();
  // Generate a TOTP using the secret key and current time
  TOTP totp((uint8_t*)secretKey, sizeof(secretKey));
  otpCode = totp.getCode(currentTime);
  return otpCode;
}


String binaryOtp;
String directions;

void calc_otp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("sending OTP");
  delay(3000); 
  // Generate a new TOTP
  otpCode = generateTOTP();
  char otpCharArray[otpLength + 1];
  otpCode.toCharArray(otpCharArray, otpLength + 1);
  Serial.println(otpCode);
  binaryOtp = Otp_conversion(otpCode);
  directions = Bi_to_Direct(binaryOtp);
  Serial.println(directions);
  lcd.clear();
  option(); 
}


/*
// SENDING THE OTP
char ph_no[11] = {'\0'};
void send_otp(){
  SIM800L.println("AT+CMGF=1");
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  char dest_set[21] = "AT+CMGS=\"XXXXXXXXXX\"\r";
  for (int i = 0; i < 10; i++) {
    dest_set[i + 9] = ph_no[i + 3];
  }
  SIM800L.println(dest_set);
  delay(1000);
  SIM800L.print("Your OTP for the door is: ");
  SIM800L.println(otpCode);
  String binaryOtp = Otp_conversion(otpCode);
  String directions = Bi_to_Direct(binaryOtp);
  SIM800L.print("Voice OTP instruction: ");
  SIM800L.println(directions);
  delay(100);
  SIM800L.write(26);
  delay(1000);
  while(SIM800L.available()>0){ Serial.write(SIM800L.read());
  }

  
}
*/

//KEYPAD INPUT METHOD
unsigned long startTime;
unsigned long countdownTime = 120000;
void Keypad_Input() {
  startTime = millis();
  String otpLCD = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTP :");
  while (millis() - startTime <= countdownTime) {
    char customKey = keypad.getKey();
    if (customKey) {
      lcd.setCursor(otpLCD.length(), 1);
      otpLCD = otpLCD + customKey;
      lcd.print(customKey);
      if (otpLCD.length() == 6) {
        if (otpLCD == otpCode) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Greeting");
          lcd.setCursor(0, 1);
          lcd.print("welcome home");
          delay(2000);
          return true;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Incorrect OTP");
          delay(2000);
          return false;
        }
      }
    }
    delay(100);
  }
  return false;
}

//VOICE INPUT MEHTOD
String accumulatedOtp = "";
void Voice_Input() {
  startTime = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTP :");
  lcd.setCursor(0, 1);
  while (millis() - startTime <= countdownTime) {
    if (vr.recognize(buf, 50) > 0) {
      String otp;
      switch (buf[1]) {
        case cmd_up:
          otp = "00";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OTP :");
          lcd.setCursor(0, 1);
          lcd.print("up ");
          break;
        case cmd_down:
          otp = "01";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OTP :");
          lcd.setCursor(0, 1);
          lcd.print("down ");
          break;
        case cmd_left:
          otp = "10";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OTP :");
          lcd.setCursor(0, 1);
          lcd.print("left ");
          break;
        case cmd_right:
          otp = "11";
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("OTP :");
          lcd.setCursor(0, 1);
          lcd.print("right ");
          break;    
      }
      accumulatedOtp += otp;
      Serial.println(accumulatedOtp);
      if (accumulatedOtp.length() == binaryOtp.length()) {
        if (accumulatedOtp == binaryOtp) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Greeting");
          lcd.setCursor(0, 1);
          lcd.print("welcome home");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Incorrect OTP");
        }
        // Reset the accumulated commands
        accumulatedOtp = "";
      }
    }
  }
}

void option() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" 1 for keypad");
  lcd.setCursor(0,1);
  lcd.print(" 2 for voice");
  delay(3000);
  char customKey = '\0'; 
  while (customKey != '1' && customKey != '2') { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("your option : ");
    lcd.setCursor(0, 1);
    customKey = keypad.getKey(); 
    switch (customKey) {
      case '1':
        Keypad_Input();
        break;
      case '2':
        Voice_Input();
        break;
    }
    delay(100); // Short delay to avoid reading the key press multiple times
  }
}

/*
// Sends a failure message to the caller
void send_failure() {
  SIM800L.println("AT+CMGF=1");
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  char dest_set[21] = "AT+CMGS=\"XXXXXXXXXX\"\r";
  for (int i = 0; i < 10; i++) {
    dest_set[i + 9] = ph_no[i + 3];
  }
  SIM800L.println(dest_set);
  delay(1000);
  SIM800L.println("You entered incorrect OTP. Please call again.");
  delay(100);
  SIM800L.println((char)26);
  delay(1000);
  while(SIM800L.available()>0){ Serial.write(SIM800L.read());
}
}
*/











