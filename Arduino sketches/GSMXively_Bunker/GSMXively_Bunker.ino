// Include the GSM library
#include <GSM.h>


//
// Xively data
//

// Replace your Xively API key here
#define XIVELY_API_KEY "ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789"

// Replace your feed ID
#define XIVELY_FEED_ID 1111111111

// User agent is the project name
#define XIVELY_USER_AGENT "GSM"

// Name address for Xively API
char xivelyServerNameAddress[] = "api.xively.com";

// Numeric IP for api.xively.com
//   (If you don't want to use DNS (and reduce your sketch size)
//   use the numeric IP instead of the name for the server)
//IPAddress xivelyServerNameAddress(216, 52, 233, 120);


//
// SIM and APN data
//

// PIN number
#define SIM_PIN_NUMBER ""

// Replace your GPRS APN
#define GPRS_APN "bluevia.movistar.es"

// Replace with your GPRS login
#define GPRS_LOGIN ""

// Replace with your GPRS password
#define GPRS_PASSWORD ""


//
// GSM shield library instance
//

GSMClient gsmClient;
GPRS gprsAccess;
GSM gsmAccess;


//
// Variabili per il calcolo del VWC (Volumetric Water Content) per approssimare la curva con una successione di spezzate
//

float VWC;

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


// Time between two main measure/communicate iterations (in seconds)
#define SLEEP_INTERVAL 60

int counter = 0;


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

//  digitalWrite(3, HIGH);

  openCommunicationChannel();
}


//
// loop()
//

void loop()
{
  // Read all sensors data
  String dataString = readSensors();

  // Controllo che il counter non superi 99 per aver un dente di sega e controllare in modo significativo la connessione
  if (counter >= 100)
    counter = 0;

  dataString += createChannelValue("Counter", counter++);

  dataString += "\n";
  sendData(dataString);

  sleep();
}


//
// sendData()
//
//   This method makes a HTTP connection to the server
//

void sendData(String thisData)
{
  while(gsmClient.connected())
  {
    while (gsmClient.available())
    {
      char c = gsmClient.read();
      Serial.print(c);
    }
  }

  // If there's a successful connection
  if (gsmClient.connect(xivelyServerNameAddress, 80))
  {
    int len = thisData.length();

    Serial.print("Content: ");
    Serial.println(thisData);
    Serial.print("Content length: ");
    Serial.println(len);

    Serial.println("Sending data...");

    // Send the HTTP PUT request:
    gsmClient.print("PUT /v2/feeds/");
    gsmClient.print(XIVELY_FEED_ID);
    gsmClient.println(".csv HTTP/1.1");
    gsmClient.print("Host: api.xively.com\n");
    gsmClient.print("X-ApiKey: ");
    gsmClient.println(XIVELY_API_KEY);
    gsmClient.print("User-Agent: ");
    gsmClient.println(XIVELY_USER_AGENT);
    gsmClient.print("Content-Length: ");
    gsmClient.println(len);

    // Last pieces of the HTTP PUT request
    gsmClient.print("Content-Type: text/csv\n");
    gsmClient.println("Connection: close\n");
    gsmClient.println();

    // Here's the actual content of the PUT request
    gsmClient.println(thisData);
    
    while(gsmClient.connected())
    {
      while (gsmClient.available())
      {
        char c = gsmClient.read();
        Serial.print(c);
      }
    }
    
    gsmClient.stop();
  } 
  else
    Serial.println("Can't connect to server");
}


//
// readSensors()
//

String readSensors()
{
  // Read the sensor on A0 (da riferirsi ad un AREF di 3.3V)
  float lightSensorReading = pow(10.0, 3.3 * analogReadAverage(A0, 10) / 1024.0);

  // Convert the data to a String
  //   You can append multiple readings to this String to 
  //   send the Xively feed multiple values
  String dataString = createChannelValue("LightLog", lightSensorReading);

  //
  // Algoritmo per implementare la funzione proposta da Vegetronix (http://vegetronix.com/Products/VH400/VH400-Piecewise-Curve.phtml)
  //

  float humiditySensorReading = analogReadAverage(A1, 40) * 3.3 / 1024.0;

  if ((humiditySensorReading >= 0.0) && (humiditySensorReading < 1.1))
    VWC = M1 * humiditySensorReading - cB1;

  if ((humiditySensorReading >= 1.1) && (humiditySensorReading < 1.3))
    VWC = M2 * humiditySensorReading - B2;

  if ((humiditySensorReading >= 1.3) && (humiditySensorReading < 1.82))
    VWC = M3 * humiditySensorReading - B3;

  if ((humiditySensorReading >= 1.82) && (humiditySensorReading < 2.2))
    VWC = M4 * humiditySensorReading - B4;

  dataString += createChannelValue("SoilHumidity", int(VWC));


  //
  // Voltaggio batteria LiPo
  //
  //   Misuro il voltaggio in mV della batteria tramite 10K-A2-10K (dissipando costantemente 0,185mA@3,7V per eseguire questa misura!)
  //

  // un partitore resistivo in ingresso permette di misurare fino a 6,6V, divido per 512 invece di moltiplicare per 2
  float  batteryVoltage = analogReadAverage(A2, 10) * 3300.0 / 512.0;

  dataString += createChannelValue("VoltaggioBatteria", int(batteryVoltage));
  
  return dataString;
}


//
// openCommunicationChannel()
//

void openCommunicationChannel()
{
  if (gprsAccess.getStatus() == GPRS_READY)
    return;

  if (gsmAccess.getStatus() != GSM_READY)
  {
    Serial.println("Starting GSM connection...");
    gsmAccess.begin(SIM_PIN_NUMBER);
    while(gsmAccess.getStatus() != GSM_READY);
    Serial.println("... done");
  }
  
  Serial.println("Starting GPRS connection...");
  gprsAccess.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD);
  while(gprsAccess.getStatus() != GPRS_READY);
  Serial.println("... done");
}


//
// sleep()
//

void sleep()
{
  Serial.println("Sleeping...");
  delay(SLEEP_INTERVAL * 1000UL);
  Serial.println("Awake!");
}


//
// createChannelValue()
//

String createChannelValue(String channelName, int value)
{
  return channelName + "," + value + "\n";
}


//
// analogReadAverage()
//

float analogReadAverage(uint8_t pin, int readCount)
{
  long sum = 0L;
  
  for (int r = readCount; r; --r)
    sum += analogRead(pin);

  return sum / (float) readCount;
}
