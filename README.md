ARDUINO COMPONENT

if you want to run the expriment yourself please be sure have the following module:
	1. Arduino UNO
	2. Keypad module 
	3. LCD (16x2) module
	4. Voice recognition V3 module
Then connect the module to the corresponding pin in the Arduino code (you can change the pin location if you want just be sure to connect to the corresponding pin hole on the Arduino)
For reference : 
	1. Keypad pin : rowPins : {10, 11, 12, 13} 
				   colPins : {2, 3, 4, 5}
	2. LCD pin : {16, 2}
	3. Voice Recognition V3 pin : {7,8}
	
ARDUINO CODE SIDE

1. Click on link and download Arduino ide (If you don't have Arduino ide in your laptop) : https://www.arduino.cc/en/software
2. Download these following library for the Arduino since the project need them inorder for it to work:
	A. Voice recognition V3 master : https://github.com/elechouse/VoiceRecognitionV3
	B. Liquid crystal i2c : https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c
	C. Keypad, TOTP : these can be found in the Arduino ide library manager
3. The Voice recognition V3 need to be train before being able to use it so use the example sketch VR_Sample_train. After successfully trained the Voice Recognition remember to save it and then load it into the main Arduino program.
4. Run the program

PYTHON CODE SIDE

1. Run the Django server 
2. Create/login into an account on the Django website
3. Create an admin account in the IDE command 
4. Change this part of the code with the name of the admin username that you have created (for reference: usernames = UserInfo.objects.exclude(username=#admin_username).values_list('username', flat=True)
)
5. Check in the Arduino IDE for the port serial 
6. Change the port address in the code (for reference : arduinoData = serial.Serial('/dev/tty.usbserial-14520', 115200))
7. Run the Arduino_serial_script to begin the data tranfers (since this project is using a cable connected from the laptop to the Arduino to facilitate a data transfer remember to connect the Arduino to the laptop BEFORE RUNNING the Arduino_serial_script)
# IMPORTANT 
Due to the way the code is setup only 3 account can be transfer from the laptop to the Arduino UNO
