// Test via serial monitor del sensore Vegetronix

//variabili per il calcolo del VWC - curva approssimata con una successione di spezzate
float VWC;
float VWC1;
float M1 = 10.0;  //coeff.angolare 0<->1.1V
float M2 = 25.0;  //coeff.angolare 1.1<->1.3V
float M3 = 48.08;  //coeff.angolare 1.3<->1.8V
float M4 = 26.32;  //coeff.angolare 1.8<->2.2V
float cB1 = 1.0;  //intercetta 0<->1.1V
float B2 = 17.5;  //intercetta 1.1<->1.3V
float B3 = 47.5;  //intercetta 1.3<->1.8V
float B4 = 7.89;  //intercetta 1.8<->2.2V

void setup()
{
  analogReference(EXTERNAL); // ATTENZIONE *** ATTENZIONE *** ATTENZIONE *** La riga deve essere presente se il pin AREF è collegato ad voltaggio esterno prima di eseguire un analogRead 
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
}

void loop()
{
  /************************* 
   * algoritmo per implementare la funzione proposta da vegetronix: http://vegetronix.com/Products/VH400/VH400-Piecewise-Curve.phtml
   **************************/
   int adcvalue;
   adcvalue = analogRead(A0);
  float voltageSensorReading;
  voltageSensorReading =  adcvalue*3.30/1024.00; // da riferirsi ad un AREF di 3.3V
  if ( voltageSensorReading >= 0.0 && voltageSensorReading < 1.1) VWC = M1*voltageSensorReading - B1;
  if ( voltageSensorReading >= 1.1 && voltageSensorReading < 1.3) VWC = M2*voltageSensorReading - B2;
  if ( voltageSensorReading >= 1.3 && voltageSensorReading < 1.8) VWC = M3*voltageSensorReading - B3;
  if ( voltageSensorReading >= 1.82 && voltageSensorReading < 3.0) VWC = M4*voltageSensorReading - B4;

int adcvalue1;
   adcvalue1 = analogRead(A1);
  float voltageSensorReading1;
  voltageSensorReading1 =  adcvalue1*3.30/1024.00; // da riferirsi ad un AREF di 3.3V
  if ( voltageSensorReading1 >= 0.0 && voltageSensorReading1 < 1.1) VWC1 = M1*voltageSensorReading1 - B1;
  if ( voltageSensorReading1 >= 1.1 && voltageSensorReading1 < 1.3) VWC1 = M2*voltageSensorReading1 - B2;
  if ( voltageSensorReading1 >= 1.3 && voltageSensorReading1 < 1.8) VWC1 = M3*voltageSensorReading1 - B3;
  if ( voltageSensorReading1 >= 1.82 && voltageSensorReading1 < 3.0) VWC1 = M4*voltageSensorReading1 - B4;



  Serial.print("ADC0: ");
  Serial.print(adcvalue);
  Serial.print("\t");
  Serial.print("Voltage0: ");
  Serial.print(voltageSensorReading,3);
  Serial.print("\t");
  Serial.print("VWC0: ");
  Serial.print(VWC,3); 
Serial.print("\t\t");
Serial.print("ADC1: ");
  Serial.print(adcvalue1);
  Serial.print("\t");
  Serial.print("Voltage1: ");
  Serial.print(voltageSensorReading1,3);
  Serial.print("\t");
  Serial.print("VWC1: ");
  Serial.println(VWC1,3); 
  Serial.println();
  
  delay(1000);
  /************************* 
   * Voltaggio batteria LiPo
   **************************/
  // float batteryvoltage; // misuro il voltaggio in mV della batteria tramite 10K-A2-10K (dissipando costantemente 0,185mA@3,7V per eseguire questa misura!)
  //batteryvoltage =  analogRead(A2)*3300.0/512.0; // un partitore resistivo in ingresso permette di misurare fino a 6,6V, divido per 512 invece di moltiplicare per 2

  //int(batteryvoltage);
}




