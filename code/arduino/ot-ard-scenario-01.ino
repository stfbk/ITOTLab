/****************************************************************************************
   LIBRARIES INCLUSION
 ****************************************************************************************/

#include <SPI.h>
#include <Ethernet.h>

/*
   Both the headers are needed to use ModbusTCP official library.
*/
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

/*
   Include the library needed to interface with the 3-axis accelerometer.

   The MMA7660 library is taken from Groove GitHub official repository:

   https://github.com/Seeed-Studio/Accelerometer_MMA7660

   To make the library available, clone the repository in the "libraries" folder inside
   the Arduino's home folder. For example, if you are using Linux and you keep the
   default IDE configuration, you are going to have the header file inside the following
   folder:

   /home/<your-username>/Arduino/libraries/Accelerometer_MMA7660/MMA7660.h
*/
#include <MMA7660.h>

/****************************************************************************************
   CONSTANTS DECLARATION
 ****************************************************************************************/

/*
   Configure the PINs and the other parameters to allow the Arduino to get the
   right values from the sensors.
*/
#define TEMP_SENSOR_IREG    0x00  // The Modbus register's offset (Analog input)
#define TEMP_SENSOR_PIN       A0  // The analog input pin
#define TEMP_THERMISTOR     4275  // The B's value of the thermistor
#define TEMP_R0           100000  // R0 = 100k

#define ACC_SENSOR_X_IREG   0x02  // The Modbus register's offset (I2C input)
#define ACC_SENSOR_Y_IREG   0x04  // The Modbus register's offset (I2C input)
#define ACC_SENSOR_Z_IREG   0x06  // The Modbus register's offset (I2C input)

#define OR_SENSOR_X_IREG    0x08  // The Modbus register's offset (I2C input)
#define OR_SENSOR_Y_IREG    0x09  // The Modbus register's offset (I2C input)
#define OR_SENSOR_Z_IREG    0x0A  // The Modbus register's offset (I2C input)

/*
  Enter a MAC address and use the DHCP server to get a valid address.
*/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/****************************************************************************************
   GLOBAL VARIABLE DECLARATION
 ****************************************************************************************/

/*
   Listen on port 502/TCP, the default for ModbusTCP
*/
EthernetServer s(502);

/*
   ModbusTCP server object
*/

ModbusTCPServer mbs;

/*
   Accelerometer object
*/

MMA7660 acc;

/****************************************************************************************
   UNIONS DECLARATION
 ****************************************************************************************/

/*
   Float union, used to transform a 32-bits float into a
   two-element array of unsigned, 16-bits integers
*/

typedef union
{
  float f;
  uint16_t i[2];
} FLOATUNION;

/****************************************************************************************
   SETUP METHOD FOR ARDUINO'S INITIALIZATION
 ****************************************************************************************/

void setup() {

  /*
     Open serial communications for debug purposes (waiting
     until it is ready.
  */
  Serial.begin(9600);

  while (!Serial) {
    ; // Wait for serial port to connect.
  }

  Serial.println("[INFO] Setup procedure started");

  /*
    Initialize the Ethernet device. It needed, you can use Ethernet.init(pin)
    to configure the CS pin, as follows:

    Ethernet.init(10);  // Most Arduino shields
    Ethernet.init(5);   // MKR ETH shield
    Ethernet.init(0);   // Teensy 2.0
    Ethernet.init(20);  // Teensy++ 2.0
    Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
    Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

    Once done (not mandatory) it is time to get an IP from the DHCP server. The
    following line provides to the "begin" method only the MAC address previously
    defined, asking for the other configuration to any listening DHCP server.
  */
  Ethernet.begin(mac);

  /*
     Check if Ethernet hardware is present
  */
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {

    Serial.println("[ERROR] Ethernet shield was not found. Freezing...");

    while (true) {

      /*
         Do nothing, no point running without Ethernet hardware
      */
      delay(100);

    }
    
  }

  /*
     Check if the Ethernet cable is connected
  */
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("[INFO] Ethernet cable is not connected.");
  }

  /*
    Checking local IP address
  */
  Serial.print("[INFO] ModbusTCP server address: ");
  Serial.println(Ethernet.localIP());

  /*
     Start listening for clients
  */
  s.begin();

  /*
     Starting ModbusTCP server
  */
  while (!mbs.begin()) {
    
    /*
       Do nothing, no point running without ModbusTCP server instance
    */
    Serial.println("[ERROR] Failed to start Modbus TCP Server. Retrying in 1 second...");
    delay (100);
  }

  /*
     Initialize the accelerometer
  */
  acc.init();

  /*
     Configure registers - 11 x 16 bits for storing temperature, acceleration and orientation
  */
  mbs.configureInputRegisters(TEMP_SENSOR_IREG, 11);

  Serial.println("[INFO] Setup procedure completed");

}

/****************************************************************************************
   ARDUINO'S MAIN
 ****************************************************************************************/

void loop() {
  
  /*
     Accelerometer X, Y and Z components
  */
  float acc_x, acc_y, acc_z;
  uint8_t or_x, or_y, or_z;
  
  /*
     Union for float to uint16_t array
  */
  FLOATUNION u;

  /*
     Maintain DHCP lease and wait for a client to connect
  */
  Ethernet.maintain();
  EthernetClient c = s.available();

  /*
     As soon as a client connects, the sensors are read and the appropriate values
     are stored in the related registers.
  */
  if (c) {

    Serial.println("[INFO] New client connected.");

    /*
       Acquire temperature and save it into the Modbus analog register at offset 0
       Note: the temperature is converted through instructions from the datasheet
    */

    Serial.println("[INFO] Getting temperature from sensor");
    int analogT = analogRead(TEMP_SENSOR_PIN);

    float R = (1023.0 / analogT - 1.0) * TEMP_R0;
    float t = 1.0 / (log(R / TEMP_R0) / TEMP_THERMISTOR + 1 / 298.15) - 273.15;

    u.f = t;

    mbs.inputRegisterWrite(TEMP_SENSOR_IREG     , u.i[0]);
    mbs.inputRegisterWrite(TEMP_SENSOR_IREG + 1 , u.i[1]);

    Serial.println("[INFO] Temperature acquired");

    /*
       Acquire the 3-axis accelerometer components (X,Y,Z) and store it in
       three 32-bit registers
    */
    Serial.println("[INFO] Getting XYZ acceleration from sensor");

    acc.getAcceleration(&acc_x, &acc_y, &acc_z);

    u.f = acc_x;

    mbs.inputRegisterWrite(ACC_SENSOR_X_IREG     , u.i[0]);
    mbs.inputRegisterWrite(ACC_SENSOR_X_IREG + 1 , u.i[1]);

    u.f = acc_y;

    mbs.inputRegisterWrite(ACC_SENSOR_Y_IREG     , u.i[0]);
    mbs.inputRegisterWrite(ACC_SENSOR_Y_IREG + 1 , u.i[1]);

    u.f = acc_z;

    mbs.inputRegisterWrite(ACC_SENSOR_Z_IREG     , u.i[0]);
    mbs.inputRegisterWrite(ACC_SENSOR_Z_IREG + 1 , u.i[1]);

    Serial.println("[INFO] Acceleration acquired");

    /*
       Acquire the 3-axis orientation components (X,Y,Z) and store it in
       three 16-bit registers (even though they are 8 bit uint8_t values)
    */
    Serial.println("[INFO] Getting XYZ orientation from sensor");

    acc.getXYZ(&or_x, &or_y, &or_z);

    mbs.inputRegisterWrite(OR_SENSOR_X_IREG, or_x);
    mbs.inputRegisterWrite(OR_SENSOR_Y_IREG, or_y);
    mbs.inputRegisterWrite(OR_SENSOR_Z_IREG, or_z);

    Serial.println("[INFO] Orientation acquired");

    /*
       Let the ModbusTCP instance accept the connection
    */
    mbs.accept(c);

    while (c.connected()) {

      Serial.println("[INFO] Polling ModbusTCP client");
      
      /*
         Poll for Modbus TCP requests, while client connected
      */
      mbs.poll();

    }

  }
}