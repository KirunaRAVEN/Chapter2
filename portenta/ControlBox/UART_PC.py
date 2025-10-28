import serial

def main():
    # Open COM3 at 115200 baud
    serialPort = serial.Serial('COM3', 115200, timeout=1)

    print("Listening on COM3 @ 115200... (Press Ctrl+C to stop)")

    try:
        while True:
            if serialPort.in_waiting:  # Check if data is available
                line = serialPort.readline() #.decode(errors='ignore').strip()
                if line:
                    print(line)
    except KeyboardInterrupt:
        print("\nStopped by user.")
    finally:
        serialPort.close()
        print("Serial port closed.")

if __name__ == "__main__":
    main()
