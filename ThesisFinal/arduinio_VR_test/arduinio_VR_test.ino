#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <TOTP.h>
#include <time.h>
#include <LiquidCrystal_I2C.h>
VR vr(7, 8);  
uint8_t buf[64];
LiquidCrystal_I2C lcd(0x27, 16, 2);  

enum Command {
  cmd_up = 0,
  cmd_down,
  cmd_left,
  cmd_right,
};

void setup() {
  vr.begin(9600);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  for (int i = cmd_up; i <= cmd_right; i++) {
    vr.load(i);
  }
  calc_otp();
}

void loop() {
  test1();
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
  for (int i = 0; i < binaryOtp.length(); i +=4 ) {  
    String biPair1 = binaryOtp.substring(i, i+2);  
    String biPair2 = binaryOtp.substring(i+2, i+4);  
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

String binaryOtp;
String directions;
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
void calc_otp() {
  otpCode = generateTOTP();
  char otpCharArray[otpLength + 1];
  otpCode.toCharArray(otpCharArray, otpLength + 1);
  Serial.println(otpCode);
  binaryOtp = Otp_conversion(otpCode);
  Serial.println(binaryOtp);
  directions = Bi_to_Direct(binaryOtp);
  Serial.println(directions);
}
String accumulatedCommands = "";  // String to accumulate the binary commands
unsigned long startTime;
unsigned long countdownTime = 12000;  // Countdown time in milliseconds

void processVoiceCommand() {
  startTime = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTP :");
  while (millis() - startTime <= countdownTime) {
    if (vr.recognize(buf, 50) > 0) {
      String command;
      switch (buf[1]) {
        case cmd_up:
          command = "00";
          break;
        case cmd_down:
          command = "01";
          break;
        case cmd_left:
          command = "10";
          break;
        case cmd_right:
          command = "11";
          break;    
      }
      accumulatedCommands += command;
      Serial.println(accumulatedCommands);
      if (accumulatedCommands.length() == binaryOtp.length()) {
        if (accumulatedCommands == binaryOtp) {
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
        accumulatedCommands = "";
      }
    }
  }  
}
void test1(){
  test2();
}
void test2(){
  processVoiceCommand();
}



