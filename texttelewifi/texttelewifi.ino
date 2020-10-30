/* Project Name : WiFi Text Teleporter (Under-Construction)
   Receives some text in String format via Browser from one device and types (pastes) it into another device.
   For : Arduino Micro/Leonardo/Due/Pro Micro (ATMega 32U4) & ESP8266 01 WiFi Module with Ai-Thinker Firmware
   Author : Advik Singhania
   Created On : 29th October, 2020; 02:03 AM IST
*/
// Keyboard header file for Arduino to take control of the keyboard and paste the string
#include "Keyboard.h"

String a = ""; // String to store the output upon successful connection

// This function sends commands to the esp8266 module and prints the output into the serial monitor
void esp(String s) {
  Serial1.print(s);
  delay(750);
  while (Serial1.available() > 0)
    Serial.write(Serial1.read());
  delay(2000);
}

void setup() {
  Serial1.begin(115200); // esp8266 module with Ai-Thinker or ESP 1.6.2 firmware operates at 115200 baud rate
  Serial.begin(9600); // Baud rate for serial monitor
  Keyboard.begin(); // Initialize the keyboard
  delay(5000);
  // AT Commands:
  esp("AT+RST\r\n"); // Resetting the module
  esp("AT+CWMODE=1\r\n"); // Mode 1 is the Station mode for the esp8266
  esp("AT+CWJAP=\"Your SSID here\",\"Your Password here\"\r\n"); // Join Access Point with WiFi SSID and Password
  esp("AT+CIPSTA=\"192.168.1.100\",\"192.168.1.1\",\"255.255.255.0\"\r\n"); // Static Station IP Address, Gateway IP and subnet mask
  esp("AT+CIPMUX=1\r\n"); // Allowing multiple connections
  esp("AT+CIPSERVER=1,80\r\n"); // Starting the server for the module at port 80
}

void loop() {
  String web = ""; // HTML Webpage to be displayed:
  web += "<!DOCTYPE html>";
  web += "<html>";
  web += "<head>";
  web += "<title>Text Teleporter</title>";
  web += "</head>";
  web += "<body>";
  web += "<form action=\"/\">Enter text:<br>";
  web += "<input type=\"text\" name=\"text\">";
  web += "<input type=\"submit\" value=\"Send\">";
  web += "</form>";
  web += "</body>";
  web += "</html>\r\n";
  if (Serial1.find("+IPD,")) { // If someone connects
    delay(500);
    while (Serial1.find("?text=")) {// If someone sends a text string
      a = Serial1.readString();
      String text = a.substring(0, a.indexOf("HTTP")); //Filtering the actual text:
      text.replace("%21", "!"); // replacing ASCII codes in hexadecimal to actual characters:
      text.replace("%2C", ",");
      text.replace("%3F", "?");
      text.replace("+", " ");
      Keyboard.println(text); // Typing the text
    }
    int cId = String((Serial1.readString()).charAt(0)).toInt(); // Taking the connection ID which is at the 0th index of the output and converting it to integer
    String cips = "AT+CIPSEND=" + String(cId) + "," + String(web.length()) + "\r\n"; // Initialize the communication channel for the connection ID to send the webpage
    esp(cips);
    delay(500);
    esp(web); // The webpage is sent
    String cipc = "AT+CIPCLOSE=" + String(cId) + "\r\n"; // Closing the channel for that connection ID
    esp(cipc);
  }
  // Blink while there is  no connection
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
}
