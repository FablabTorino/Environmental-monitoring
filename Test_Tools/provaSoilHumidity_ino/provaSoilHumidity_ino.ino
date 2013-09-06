
  float VWC;        //contenuto volumetrico di acqua
  int counter = 0;  //counter solo per controllo connessione
  int i = 0;        //per fare al media
  int n = 5;        // numero di elementi su cui medio
  float M1 =10;  //coeff.angolare 0<->1.1V
  float M2 = 25.0;  //coeff.angolare 1.1<->1.3V
  float M3 = 48.08;  //coeff.angolare 1.3<->1.8V
  float M4 = 26.32;  //coeff.angolare 1.8<->2.2V
  float cB1 = 1.0;  //intercetta 0<->1.1V
  float B2 = 17.5;  //intercetta 1.1<->1.3V
  float B3 = 47.5;  //intercetta 1.3<->1.8V
  float B4 = 7.89;  //intercetta 1.8<->2.2V
  float vettore1[20];
  float vettore2[20];
  float vettore3[20];

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
 for (i=0; i<20 ;i++){ 
  vettore1[i] =  analogRead(A1);
  vettore2[i]=vettore1[i]*5.0/1024.0; // da riferirsi ad un AREF di 5Vt
  if ( vettore2[i] >= 0.0 && vettore2[i] < 1.1) VWC = M1*vettore2[i] - B1;
  if ( vettore2[i] >= 1.1 && vettore2[i] < 1.3) VWC = M2*vettore2[i] - B2;
  if ( vettore2[i] >= 1.3 && vettore2[i] < 1.82) VWC = M3*vettore2[i] - B3;
  if ( vettore2[i] >= 1.82 && vettore2[i] < 3.0) VWC = M4*vettore2[i] - B4;  //messo 3 per ampliare il fondo scala(giac-franco)
  vettore3[i] = VWC;
 }
 
 for (i=0; i<20 ;i++){ 
 Serial.print("lettura analogica 0-1024  :");
 Serial.print(vettore1[i]);
 Serial.print("  lettura analogica 0 - 3V  :");
 Serial.print(vettore2[i]);
 Serial.print("  VWC  :");
 Serial.println(vettore3[i]);
   
 }
 delay(3000);
}

