
#include <SdFat.h>
// SD chip select pin
const uint8_t chipSelect = SS;
// file system
SdFat sd;
// Serial print stream
ArduinoOutStream cout(Serial);
// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))
//------------------------------------------------------------------------------

 int i = 0;
 int n = 0;
 int connection;
  
  
void setup() {
  Serial.begin(9600);
  while (!Serial) {}  // wait for Leonardo
  
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  
  /******************************__.
  creo le 3 directory 
  */
  sd.mkdir("ACQ"); //directory dovre andranno i dati ACQuisiti ancora da spedire
  sd.mkdir("RTS"); //directory dovre andranno i dati acquisiti Ready To Send
  sd.mkdir("RTS"); //directory dovre andranno i dati acquisiti spediti SeNT
 
}
void loop() {
 
  // ****metto un blocco  pratico
  cout << pstr(" Type any character to start.") << endl;
  while (Serial.read() <= 0) {}
  delay(400);  // catch Due reset problem

  if (i == 10) i = 0;     //ogni dieci faccio un ciclo
  
  //costruisco la mia stringa di dati
  String dataString = "LightLog,";
  dataString += i;
  dataString += "\nSoilHumidity,";
  dataString += i*10;
  dataString += "\nTemperature, ";
  dataString += i*100;
  
  Serial.println ("fatto il calcolo : ");
  Serial.println (dataString); 
 
  /* creo il primo file nella cartella di acquisizione, al prossimo
  giro di loop questa cartella sarà vuota perchè il file da me creato e spostano nella dir RTS*/ 
  SdFile file( "ACQ/a.txt", O_WRITE | O_CREAT);
  if (!file.isOpen()) error("a");
  file.print(dataString);                              //contollo
  file.print("sono in ACQ/a.txt");                     //contollo
  cout << pstr("sono in ACQ/a.txt") << endl;
  sd.ls(LS_R);                                         //guardo cosa c'e nella mia SD
  //file.close();
  
  // se ho già a.txt nella cartella RTS allora lo cancello altrimenti avrò errore dopo quando lo muovo qui
  if(file.remove(sd.vwd(), "RTS/a.txt")){
    Serial.print("c'era qualcosa in RTS ma lo ho tolto "); 
    cout << pstr("vediamo le cartelle") << endl;
  sd.ls(LS_R);                                         //guardando il seriale qui dovrei vedere ls -r di SD e RTS dovrebbe essere vuota
  }
  
  //sposto il file a.txt da ACQ in RTS
  if (!file.rename(sd.vwd(), "RTS/a.txt")) error("a");     
  file.println("sono in RTS/a.txt");

  // list files
  cout << pstr("sono in RTS/a.txt") << endl;
  sd.ls(LS_R);
  
  Serial.print("ho già chiuso, escluso questo ");
  Serial.print(n);
  Serial.println(" loop ");


  /*cout << pstr("type 1 or right connection and trasmission ofn content, any other key for wrong ones") << endl;
  while (connection <= 0) {
  connection = Serial.read();
  Serial.println(connection);
  
  Serial.println(connection, DEC);
 // while (connection <= 0) {
  if (connection == 1)  {
    Serial.print(" premuto uno e si vede qui ");
    Serial.print(connection, DEC);
      }
  }*/
  delay(400);  // catch Due reset problem

  

  // close file before rename(oldPath, newPath)
  file.close();
/*
  // move DIR1 into DIR2 and rename it DIR3
  if (!sd.rename("DIR1", "DIR2/DIR3")) error("DIR2/DIR3");

  // open file for append in new location and add a line
  if (!file.open("DIR2/DIR3/NAME3.TXT", O_WRITE | O_APPEND)) {
    error("DIR2/DIR3/NAME3.TXT");
  }
  file.println("A line for DIR2/DIR3/NAME3.TXT");

  // list files
  cout << pstr("------") << endl;
  sd.ls(LS_R);
*/
  i++;
  n++;
  cout << pstr("Done") << endl;
}
