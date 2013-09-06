
  float VWC;        //contenuto volumetrico di acqua
  int counter = 0;  //counter solo per controllo connessione
  int i = 0;        //per fare al media
  int n = 5;        // numero di elementi su cui medio
  int sumLight = 0;
  float sumSoil = 0;
  float M1 = 8.18;  //coeff.angolare 0<->1.1V
  float M2 = 25.0;  //coeff.angolare 1.1<->1.3V
  float M3 = 48.0;  //coeff.angolare 1.3<->1.8V
  float M4 = 25.0;  //coeff.angolare 1.8<->2.2V
  float cB1 = -1.0;  //intercetta 0<->1.1V
  float B2 = 17.5;  //intercetta 1.1<->1.3V
  float B3 = 47.4;  //intercetta 1.3<->1.8V
  float B4 = 5.0;  //intercetta 1.8<->2.2V
  //float otherSensorReading; //to store the analograed     (22-lug l'ho dichiarato nel loop)

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
}
void loop()
{
 /* // read the sensor on A0
  int sensorReading = pow(10.0, 5.0*analogRead(A0)/1024.0 ); */
  float otherSensorReading;
  otherSensorReading =  analogRead(A1)*5.0/1024.0; // da riferirsi ad un AREF di 5Vt
  
  Serial.print ("lettura da 0 a 5V :");
  Serial.println (otherSensorReading);
 
  if (i = 0) {
   // sensorReading = 0;
    otherSensorReading = 0;
   }
  if (i < n+1) {
    //sumLight += sensorReading;
    sumSoil += otherSensorReading;
  }
  else {
    //sensorReading = sumLight/n;
   // sumLight = 0;
    otherSensorReading = sumSoil/n;
  /************************* 
  algoritmo per implementare la funzione proposta da vegetronix: http://vegetronix.com/Products/VH400/VH400-Piecewise-Curve.phtml
  **************************/
    if ( otherSensorReading >= 0.0 && otherSensorReading < 1.1) VWC = M1*otherSensorReading + B1;
    if ( otherSensorReading >= 1.1 && otherSensorReading < 1.3) VWC = M2*otherSensorReading + B2;
    if ( otherSensorReading >= 1.3 && otherSensorReading < 1.82) VWC = M3*otherSensorReading + B3;
    if ( otherSensorReading >= 1.82 && otherSensorReading < 3.0) VWC = M4*otherSensorReading + B4;  //messo 3 per ampliare il fondo scala(giac-franco)
    sumSoil = 0;
    i = 0; 
  }
  
  i ++;
  // convert the data to a String
  String dataString = "\nSoilHumidity,";
  dataString += int(VWC);
  
 
  
  //controllo che il counter non superi 100 per aver un Dentedisega e controllare in modo significativo la connessione
  if (counter >= 100) counter = 0;
  
  dataString += "\nCounter,";
  dataString += counter++;
  
  Serial.println(dataString);
   
}

