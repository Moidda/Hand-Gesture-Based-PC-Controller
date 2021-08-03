import time
import serial
import pyautogui
from win32gui import GetWindowText, GetForegroundWindow


bluetooth_port_in_your_pc = "COM3" #1.pair with the bluetooth module 2.go to bluetooth settings 3.more bluetooth options 4.com ports
ser = serial.Serial(bluetooth_port_in_your_pc, 9600, timeout = 1) #your baudrate normally 9600


def trim_last_hyphen(text: str):
    text = text.split('-')[-1]
    text = text.strip()
    return text


def swipe():
    pyautogui.hotkey("alt", "tab")
    print("Swipe")


def up():
    pyautogui.press("up")
    print("Up")


def down():
    pyautogui.press("down")
    print("Down")


def pause():
    current_screen = trim_last_hyphen(GetWindowText(GetForegroundWindow()))
    # print(current_screen)
    if current_screen == "Google Chrome":
        pyautogui.hotkey('ctrl', 'tab')
    elif current_screen == 'VLC media player':
        pyautogui.press("space")
    print("Pause")


def retrieveData():
    data = '1000'
    try:
        data = ser.readline().decode('utf-8')
    except:
        pass
    return data


while(True):
    # try:
    #     OutgoingData=input('> ')
    #     ser.write(bytes(OutgoingData,'utf-8'))
    # except KeyboardInterrupt:
    #     print("Closing and exiting the program")
    #     ser.close()
    #     sys.exit(0)
    data = retrieveData()
    print(data)
    if data:
        if data == 'U\n':
            up()
        elif data == 'D\n':
            down()
        elif data == 'S\n':
            swipe()
        elif data == 'P\n':
            pause()
        elif data:
            print(data)

    time.sleep(0.1)