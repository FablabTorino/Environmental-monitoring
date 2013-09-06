
  float VWC;        //contenuto volumetrico di acqua
  int i = 0;        //per fare al media
  int n = 300;        // numero di elementi su cui medio
  float M1 =10;  //coeff.angolare 0<->1.1V
  float M2 = 25.0;  //coeff.angolare 1.1<->1.3V
  float M3 = 48.08;  //coeff.angolare 1.3<->1.8V
  float M4 = 26.32;  //coeff.angolare 1.8<->2.2V
  float cB1 = 1.0;  //intercetta 0<->1.1V
  float B2 = 17.5;  //intercetta 1.1<->1.3V
  float B3 = 47.5;  //intercetta 1.3<->1.8V
  float B4 = 7.89;  //intercetta 1.8<->2.2V
  float otherSensorReading = 0;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  if (i == n) i = 0;
  otherSensorReading = i/100.0; 
  if ( otherSensorReading >= 0.0 && otherSensorReading < 1.1) VWC = M1*otherSensorReading - B1;
  if ( otherSensorReading >= 1.1 && otherSensorReading < 1.3) VWC = M2*otherSensorReading - B2;
  if ( otherSensorReading >= 1.3 && otherSensorReading < 1.82) VWC = M3*otherSensorReading - B3;
  if ( otherSensorReading >= 1.82 && otherSensorReading < 3.0) VWC = M4*otherSensorReading - B4;  //messo 3 per ampliare il fondo scala(giac-franco)
 
  Serial.print ("il valore in Volt  : ");
  Serial.print (otherSensorReading);
  Serial.print (" la trasformazione  ");
  Serial.println (VWC);
  delay (1000);
  i++;
   
}


