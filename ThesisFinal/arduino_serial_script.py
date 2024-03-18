import os
import smtplib
#username: #insert_email_address password: #insert_email_passcode
import django
import serial
import requests
import time
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'ThesisFinal.settings')
django.setup()
arduinoData = serial.Serial('/dev/tty.usbserial-14520', 115200) # Accessing the port that the Arduino is connected to, and setting the baud rate

from Website.models import DoorLock, UserInfo

# Get all usernames from the Django database
usernames = UserInfo.objects.exclude(username='matt').values_list('username', flat=True)


def send_email(user_email, otpCode, directions):
    # Set up the SMTP server
    s = smtplib.SMTP(host='smtp.gmail.com', port=587)
    s.starttls()
    s.login('#insert_email_address', '#insert_email_passcode')

    # Create the message
    msg = MIMEMultipart()
    msg['From'] = '#insert_email_address'
    msg['To'] = user_email
    msg['Subject'] = "Your OTP and Directions"
    message = f"Hello,\n\nYour OTP is: {otpCode}\n\nDirections: {directions}"
    msg.attach(MIMEText(message, 'plain'))

    s.send_message(msg)
    s.quit()



# Fetch data for all users
user_data = []
for username in usernames:
    url = f'http://localhost:8000/PIN/{username}/'
    print(f"Handling username {username}")
    try:
        print(f"Sending GET request to {url}")
        response = requests.get(url)
        response.raise_for_status()  # Raise an exception if the HTTP status is an error status
        data = response.json()
        print(f"Received response: {response.status_code} {data}")
        pin = data['pin']
        phone_number = data['phone']
        email =data['email']
        user_data.append((username, pin, phone_number,email))
    except requests.exceptions.RequestException as err:
        print(f"Something went wrong when handling username {username}: {err}")
time.sleep(3)
# Handle data for all users
for username, pin, phone_number,email in user_data:
    user = UserInfo.objects.get(username=username)
    door_lock = DoorLock.objects.get(address__user=user)
    # Send the username, phone number, and PIN to the Arduino
    data_to_send = f"{username};{phone_number};{pin}"
    arduinoData.write((data_to_send + '\r').encode())
    while True:
        if arduinoData.in_waiting > 0:
            acknowledgement = arduinoData.readline().decode('utf-8').rstrip()
            if acknowledgement == "OK":
                print(f"Received 'OK' acknowledgement for user {username}")
                break
# RECEIVING THE OTP
while True:
    if arduinoData.in_waiting > 0:
        registerUser = arduinoData.readline().decode('utf-8').rstrip()
        print(f"User: {registerUser}")
        otpCode = arduinoData.readline().decode('utf-8').rstrip()
        print(f"OTP: {otpCode}")
        directions = arduinoData.readline().decode('utf-8').rstrip()
        print(f"Directions: {directions}")
        user_email = next((email for username, pin, phone_number, email in user_data if username == registerUser), None)
        if user_email is not None:
            send_email(user_email, otpCode, directions)
        break

# will implement command line after testing with the arduino user data
'''
if door_lock.Door_status != status:
    command = 'unlock' if door_lock.Door_status else 'lock'
    print(f"Sending to Arduino: {command}")
    ser.write(command.encode())
    print(f"Sent to Arduino: {command}")  # Print the command sent to Arduino
else:
    DoorLock.objects.filter(address__user=user).update(Door_status=status)
'''
# Will implement when the data transfer is set up
'''
if arduinoData.in_waiting > 0:
    line = arduinoData.readline().decode('utf-8').rstrip()
    status = line == "Door is unlocked"
'''