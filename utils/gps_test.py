import serial

serial_port = 'COM11' 
baud_rate = 9600

ser = serial.Serial(serial_port, baud_rate)

try:
    while True:
        line = ser.readline().decode().strip()
        print("Received:", line)
except KeyboardInterrupt:
    ser.close()
    print("Serial port closed.")
