const int ledPin =  4;               // the number of the LED pin
int ledState = 0;                    // the state of the LED
char rx_byte = 0;                    // data received from serial (should be 0 or 1)
String rx_str = "";

//More examples here: https://forum.arduino.cc/index.php?topic=396450
//Remember Serial Monitor with no line ending 
void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.println("<Arduino is ready>");
}


void loop()
{
  /*if (Serial.available() > 0) { //If data is available on serial
    rx_byte = Serial.read();    // get the character
    digitalWrite(ledPin, (int)rx_byte - 48); //Turn on/off led
    }*/

  if (Serial.available() > 0) { //If data is available on serial
    rx_byte = Serial.read();    // get the character - '11' to '19' are still valid
    if (rx_byte == '0' || rx_byte == '1') {
      if (rx_byte == '0' && ledState == 1) {
        digitalWrite(ledPin, LOW);
        ledState = 0;
      } else if (rx_byte == '1' && ledState == 0) {
        digitalWrite(ledPin, HIGH);
        ledState = 1;
      }
    } else {
      Serial.println("Invalid INPUT");
    }
    }
  /*if (Serial.available() > 0) { //If data is available on serial
    rx_str = Serial.readString();
    Serial.println(rx_str);
    if (rx_str == "0" || rx_str == "1") {
      if (rx_str == "0" && ledState == 1) {
        digitalWrite(ledPin, LOW);
        ledState = 0;
      } else if (rx_str == "1" && ledState == 0) {
        digitalWrite(ledPin, HIGH);
        ledState = 1;
      }
    } else {
      Serial.println("Invalid INPUT");
    }
  }*/

}
