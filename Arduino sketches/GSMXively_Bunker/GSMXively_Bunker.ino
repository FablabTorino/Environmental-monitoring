// Include the GSM library
#include <GSM.h>

#include "LowPower.h"


//
// Xively data
//

// Replace your pachube API key here
#define APIKEY "ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789"

// Replace your feed ID
#define FEEDID 1111111111

// User agent is the project name
#define USERAGENT "GSM"

// Name address for Xively API
char server[] = "api.xively.com";

// Numeric IP for api.xively.com
//   (If you don't want to use DNS (and reduce your sketch size)
//   use the numeric IP instead of the name for the server)
// IPAddress server(216, 52, 233, 120);


//
// SIM and APN data
//

// PIN number
#define PINNUMBER ""

// Replace your GPRS APN
#define GPRS_APN "bluevia.movistar.es"

// Replace with your GPRS login
#define GPRS_LOGIN ""

// Replace with your GPRS password
#define GPRS_PASSWORD ""


//
// GSM shield library instance
//

GSMClient client;
GPRS gprsAccess;
GSM gsmAccess;


//
// Variabili per il calcolo del VWC per approssimare la curva con una successione di spezzate
//

float VWC;
int counter = 0;

// Coefficiente angolare 0<->1.1V
float M1 = 10;

// Coefficiente angolare 1.1<->1.3V
float M2 = 25.0;

// Coefficiente angolare 1.3<->1.8V
float M3 = 48.08;

// Coefficiente angolare 1.8<->2.2V
float M4 = 26.32;

// Intercetta 0<->1.1V
float cB1 = 1.0;

// Intercetta 1.1<->1.3V
float B2 = 17.5;

// Intercetta 1.3<->1.8V
float B3 = 47.5;

// Intercetta 1.8<->2.2V
float B4 = 7.89;


// Last time you connected to the server, in milliseconds
unsigned long lastConnectionTime = 0;

// State of the connection last time through the main loop
boolean lastConnected = false;

// Delay between updates to Xively.com
const unsigned long postingInterval = 3000L;

boolean notConnected = true;


//
// setup()
//

void setup()
{
  // ATTENZIONE *** ATTENZIONE *** ATTENZIONE
  // Scollegare il pin AREF da +3,3V prima di caricare altri sketch che non contengano questa riga
  analogReference(EXTERNAL);
 
  // Initialize serial communications and wait for port to open
  Serial.begin(9600);
  while (!Serial);
}


//
// loop()
//

void loop()
{
  //delay(3000);

  String dataString = readSensors();

  digitalWrite(3, HIGH);

  openConnection();

  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
    Serial.println("client available");
  }

  int controller = 0;
  Serial.println(client.connected());
  controller = client.connected();
  Serial.println(controller);
  Serial.println(lastConnected);

  // If there's no net connection, but there was one last time
  // through the loop, then stop the client
//  if (!client.connected() && lastConnected)
//  {
//    Serial.println();
//    Serial.println("disconnecting.");
//    client.stop();
//  }

  // If you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval))
  {
    // Controllo che il counter non superi 100 per aver un dente di sega e controllare in modo significativo la connessione
    if (counter >= 100) counter = 0;
    dataString += "\nCounter,";
    dataString += counter++;

    Serial.println("sono nel if pronto per trasmettere");
    Serial.println("stampo la stringa che spediro'");
    Serial.println(dataString);
    sendData(dataString);

    // Note the time that the connection was made or attempted
    lastConnectionTime = millis();
  }

  // Store the state of the connection for next time through the loop
  Serial.print("come e' il client :  ");
  lastConnected = client.connected();
  Serial.println(lastConnected);
  delay (1000);
  closeConnection();
  delay(1000);
  sleep();
  Serial.println("connection closed");
  Serial.println("_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
  delay (1000);
}


//
// sendData()
//
//   This method makes a HTTP connection to the server
//

void sendData(String thisData)
{
  // If there's a successful connection
  if (client.connect(server, 80))
  {
    Serial.println("sono nel sendData");
    Serial.println(thisData);
    Serial.println(client.connected());
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
}


//
// readSensors()
//

String readSensors()
{
  // Convert the data to a String
  //   You can append multiple readings to this String to 
  //   send the Xively feed multiple values
  String dataString = "LightLog,";

  // Read the sensor on A0 (da riferirsi ad un AREF di 3.3V)
  int sensorReading = pow(10.0, 3.3 * analogRead(A0) / 1024.0 );

  dataString += sensorReading;

  //int otherSensorReading = analogRead(A1)/6;
  float otherSensorReading = 0;


  //
  // Algoritmo per implementare la funzione proposta da Vegetronix (http://vegetronix.com/Products/VH400/VH400-Piecewise-Curve.phtml)
  //

  for (int p = 0; p < 40 ; ++p)
  {
    // Da riferirsi ad un AREF di 3.3V
    //otherSensorReading += analogRead(A1) * 3.3 / 1024.0;
    otherSensorReading = analogRead(A1) * 3.3 / 1024.0;
  }

  //otherSensorReading = otherSensorReading / 10;

  if ((otherSensorReading >= 0.0) && (otherSensorReading < 1.1))
    VWC = M1 * otherSensorReading - cB1;

  if ((otherSensorReading >= 1.1) && (otherSensorReading < 1.3))
    VWC = M2 * otherSensorReading - B2;

  if ((otherSensorReading >= 1.3) && (otherSensorReading < 1.82))
    VWC = M3 * otherSensorReading - B3;

  if ((otherSensorReading >= 1.82) && (otherSensorReading < 2.2))
    VWC = M4 * otherSensorReading - B4;

  dataString += "\nSoilHumidity,";
  dataString += int(VWC);


  //
  // Voltaggio batteria LiPo
  //
  //   Misuro il voltaggio in mV della batteria tramite 10K-A2-10K (dissipando costantemente 0,185mA@3,7V per eseguire questa misura!)
  //

  float batteryvoltage;
  batteryvoltage =  analogRead(A2)*3300.0/512.0; // un partitore resistivo in ingresso permette di misurare fino a 6,6V, divido per 512 invece di moltiplicare per 2

  dataString += "\nVoltaggioBatteria,";
  dataString += int(batteryvoltage);
  // dataString +="\n\n";
  
  return dataString;
}


//
// sleep()
//

void sleep()
{
  for (int g = 0; g < 4 ; ++g)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON); 
    delay (1000);
  }

  delay(1000);

  Serial.println("end sleeping");
}


//
// closeConnection()
//

void closeConnection()
{
  if(client.connected())
  {
    client.stop();
    Serial.println("disconnecting in closeConnection()");
  }

  delay (2000);

  while(notConnected == false)
  {
    if (gsmAccess.shutdown())
    {
      delay(1000);

      digitalWrite(3, LOW);

      notConnected = true;
    }
    else
    {
      delay(1000);
    }
  }
}


//
// openConnection()
//

void openConnection()
{
  if (gprsAccess.getStatus() == GPRS_READY)
  {
    Serial.println("returning due to GPRS_READY");
    return;
  }

  if (gsmAccess.getStatus() != GSM_READY)
  {
    Serial.println("Starting GSM connection...");
 
    gsmAccess.begin(PINNUMBER);
 
    while(gsmAccess.getStatus() != GSM_READY);
 
    Serial.println("... done");
  }
  
  delay(3000);

  Serial.println("Starting GPRS connection...");

  gprsAccess.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);

  while(gprsAccess.getStatus() != GPRS_READY);

  Serial.println("... done");

  notConnected = false;
}

