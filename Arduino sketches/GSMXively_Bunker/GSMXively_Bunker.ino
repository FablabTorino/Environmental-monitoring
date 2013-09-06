/*
 Pachube client with Strings
 
 This sketch connects two analog sensors to Pachube (http://www.pachube.com)
 through a Telefonica GSM/GPRS shield.
 
 This example has been updated to use version 2.0 of the Pachube.com API. 
 To make it work, create a feed with two datastreams, and give them the IDs
 sensor1 and sensor2. Or change the code below to match your feed.
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * Analog sensors attached to A0 and A1
 * GSM shield attached to an Arduino
 * SIM card with a data plan
 
 created 8 March 2012
 by Tom Igoe
 and adapted for GSM shield by David Del Peral
 
 This code is in the public domain.
 
 */

// Include the GSM library
#include <GSM.h>

// Pachube login information
#define APIKEY         "k6xUkCu1PbfV0eSa9QwC47vh4CjfGdJSPPQESHDsM3PdVbn4"  // replace your pachube api key here
#define FEEDID         2092230271                     // replace your feed ID
#define USERAGENT      "GSM"//"Xively-Arduino-Lib/1.0"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "mobile.vodafone.it" // replace your GPRS APN
#define GPRS_LOGIN     ""    // replace with your GPRS login
#define GPRS_PASSWORD  "" // replace with your GPRS password

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);     // numeric IP for api.pachube.com
char server[] = "api.xively.com";       // name address for Pachube API

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 300000L;  // delay between updates to Pachube.com

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  //variabili per il calcolo del VWC per approssimare la curva con una successione di spezzate __.
  float M1 = 8.18;  //coeff.angolare 0<->1.1V
  float M2 = 25;  //coeff.angolare 1.1<->1.3V
  float M3 = 48;  //coeff.angolare 1.3<->1.8V
  float M4 = 25;  //coeff.angolare 1.8<->2.2V
  float B1 = -1;  //intercetta 0<->1.1V
  float B2 = 17.5;  //intercetta 1.1<->1.3V
  float B3 = 47.4;  //intercetta 1.3<->1.8V
  float B4 = 5;  //intercetta 1.8<->2.2V
  float XXX; //to store the analograed
  
  /**************************************************** 
  XXX =  analogRead(A1)*5/1024; // da riferirsi ad un AREF di 5V
  if ( XXX >= 0 && XXX < 1.1) VWC = M1*XXX + B1;
  if ( XXX >= 1.1 && XXX < 1.3) VWC = M2*XXX + B2;
  if ( XXX >= 1.3 && XXX < 1.82) VWC = M3*XXX + B3;
  if ( XXX >= 1.82 && XXX < 2.2) VWC = M4*XXX + B4;
  ok??????????????????????????????????????????????
  **************************************************/

  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password 
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("Connected to GPRS network");
}

void loop()
{
  // read the sensor on A0
  int sensorReading = pow(10.0, 5.0*analogRead(A0)/1024.0 ); 

  // convert the data to a String
  String dataString = "LightLog,";
  dataString += sensorReading;

  //you can append multiple readings to this String to 
  // send the pachube feed multiple values
  int otherSensorReading = analogRead(A1)/6;
  dataString += "\nSoilHumidity,";
  dataString += otherSensorReading;
  //  dataString +="\n\n";
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client
  if (!client.connected() && lastConnected)
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval))
  {
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server
void sendData(String thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println(thisData);
    int len = thisData.length()+2;
    Serial.print("len: ");
    Serial.println(len);

    Serial.println("connecting...");

    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.print("Host: api.xively.com\n");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(len);

    // last pieces of the HTTP PUT request
    client.print("Content-Type: text/csv\n");
    client.println("Connection: close\n");
    client.println();

    // here's the actual content of the PUT request
    client.println(thisData);
  } 
  else
  {
    // if you couldn't make a connection
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}

