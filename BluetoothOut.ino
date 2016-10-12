#include<CurieBLE.h>
int LED = 13;                                     // select the pin for the LED
int RESET = 7;                                    // BT module uses pin 7 for reset
const int ledPin = 13; // set ledPin to use on-board LED
BLEPeripheral blePeripheral; // create peripheral instance
//BLECentralHelper *bleCentral1 = NULL; // peer central device 

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // create service with a 128-bit UUID (32 characters exclusive of dashes).
                                                               // Long UUID denote custom user created UUID

// create switch characteristic and allow remote device to read and write
BLECharCharacteristic switchChar("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
int led = 13;
const int analogInPin0 = A0;// Analog input pins
const int analogInPin1 = A1;
const int analogInPin2 = A2;
const int analogInPin3 = A3;
 
//Arrays for the 4 inputs**********************************************
float sensorValue[4] = {0,0,0,0};
float voltageValue[4] = {0,0,0,0};
 
//Char used for reading in Serial characters
char inbyte = 0;

void setup() {
  pinMode(LED,OUTPUT);                            // declare the LED and BT RESET pins as output
  pinMode(RESET,OUTPUT);  
  Serial.begin(115200);                           // connect to the serial port

  // set the local name peripheral advertises
  blePeripheral.setLocalName("B & S");
  // set the UUID for the service this peripheral advertises
  blePeripheral.setAdvertisedServiceUuid(ledService.uuid());

  // add service and characteristic
  blePeripheral.addAttribute(ledService);
  blePeripheral.addAttribute(switchChar);

  // assign event handlers for connected, disconnected to peripheral
//  blePeripheral.setEventHandler(BLEConnected, blePeripheralConnectHandler);
//  blePeripheral.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
//  switchChar.setEventHandler(BLEWritten, switchCharacteristicWritten);
// set an initial value for the characteristic
  switchChar.setValue(0);

  // advertise the service
  blePeripheral.begin();
  Serial.println(("Bluetooth device active, waiting for connections..."));
}

void loop() {
  readSensors();
  getVoltageValue();
  sendAndroidValues();
  //when serial values have been received this will be true
  if (Serial.available() > 0)
  {
    inbyte = Serial.read();
    if (inbyte == '0')
    {
      //LED off
      digitalWrite(led, LOW);
    }
    if (inbyte == '1')
    {
      //LED on
      digitalWrite(led, HIGH);
    }
  }
  if (voltageValue[0] > 0)
  {
    digitalWrite(13, HIGH);
    delay(500/voltageValue[0]);
    digitalWrite(13, LOW);
    //delay by 2s. Meaning we will be sent values every 2s approx
    //also means that it can take up to 2 seconds to change LED state
    delay(500/voltageValue[0]);
  }
}
 
void readSensors()
{
  // read the analog in value to the sensor array
  sensorValue[0] = analogRead(analogInPin0);
  sensorValue[1] = analogRead(analogInPin1);
  sensorValue[2] = analogRead(analogInPin2);
  sensorValue[3] = analogRead(analogInPin3);
}
//sends the values from the sensor over serial to BT module
void sendAndroidValues()
 {
  //puts # before the values so our app knows what to do with the data
  Serial.print('#');
  //for loop cycles through 4 sensors and sends values via serial
  for(int k=0; k<4; k++)
  {
    Serial.print(voltageValue[k]);
    Serial.print('+');
    //technically not needed but I prefer to break up data values
    //so they are easier to see when debugging
    //digitalWrite(13, HIGH);
  }
 Serial.print('~'); //used as an end of transmission character - used in app for string length
 Serial.println();
 delay(10);        //added a delay to eliminate missed transmissions
}
 
void getVoltageValue()
{
  for (int x = 0; x < 4; x++)
  {
    voltageValue[x] = ((sensorValue[x]/1023)*5);
  }
}

