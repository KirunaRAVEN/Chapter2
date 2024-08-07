import requests
import time
from datetime import datetime
import os

# Function to scrape temperature value
def scrape_temperature():
    url = "https://www.temperatur.nu/termo/gettemp.php?stadname=irf&what=temp"
    try:
        response = requests.get(url)
        if response.status_code == 200:
            temperature = response.text.rstrip()
            return temperature
        else:
            print(f"Failed to retrieve temperature. Status code: {response.status_code}")
            return None
    except Exception as e:
        print(f"Error occurred during scraping: {e}")
        return None

# Function to save temperature and current time to a text file
def save_to_file(temperature):
    if temperature is not None:
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(startDate + "_temperature_log.txt", "a") as file:
            file.write(f"{timestamp} - Temperature: {temperature} °C\n")
            print(f"Temperature: {temperature} °C - Saved at {timestamp}")
    else:
        print("No temperature data to save.")

# Main loop to scrape temperature every 60 seconds

startDate = datetime.now().strftime("%Y_%m_%d")
os.chdir("../../IRF_Temperature_Data")

while True:
    temperature = scrape_temperature()
    save_to_file(temperature)
    time.sleep(60)  # Wait for 30 seconds before scraping again
