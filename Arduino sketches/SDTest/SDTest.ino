#include <SdFat.h>
#include <SdFatUtil.h>
// SD chip select pin
const uint8_t chipSelect = SS;
// file system
SdFat sd;
// Serial print stream
ArduinoOutStream cout(Serial);
// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

int ggmm = 1;
int dat = 0;

void setup() {

  Serial.begin(9600);
  while (!Serial) {
  }  // wait for Leonardo
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  sd.mkdir("ACQ"); //directory dovre andranno i dati ACQuisiti ancora da spedire
  sd.mkdir("RTS"); //directory dovre andranno i dati acquisiti Ready To Send
  sd.mkdir("SNT"); //directory dovre andranno i dati acquisiti spediti SeNT

}

void loop() {

  cout << pstr(" Type any character to start.") << endl;
  while (Serial.read() <= 0) {
  }
  delay(400);  // catch Due reset problem


  //compongo una data
  String date ="0";
  date +=ggmm;
  date +=ggmm;
  date +="0";
  ggmm++;
  if(ggmm ==10) ggmm=1;
  Serial.print ("data   :"); 
  Serial.println (date); 


  //costruisco la mia stringa di dati
  if (dat == 10) dat = 0;
  String dataString = " LightLog,";
  dataString += dat;
  dataString += " SoilHumidity,";
  dataString += dat*10;
  dataString += " Temperature, ";
  dataString += dat*100;
  Serial.print ("dataString   :"); 
  Serial.println (dataString); 
  dat ++;


  //creo il nome del file per scrivere dentro i dati:solo per ACQ 
  String nameoffile;
  nameoffile = "/ACQ/"+date+".txt";
  int lettersnumber=14; //lunghezza del path
  char nameoffile_C[lettersnumber];
  nameoffile.toCharArray(nameoffile_C, lettersnumber);


  //controllo il nome del file appena creato  
  Serial.println ("--...---"); 
  Serial.println (nameoffile_C);


  //apro il file in ACQ
  SdFile file(nameoffile_C , O_RDWR | O_APPEND | O_CREAT);
  sd.ls(LS_R);  
  //scrivo all'interno del file aperto
  if (!file.isOpen()) error("non ho creato il file contenente i dati");
  file.print(dataString);

  //creo il nome del file per rinominarlo in RTS 
  nameoffile = "/RTS/"+date+".txt";
  //nameoffile = +date+".txt";
  nameoffile.toCharArray(nameoffile_C, lettersnumber);
  Serial.print ("creato il nuovo nomefile : ");
  Serial.println (nameoffile_C);
  int escape = false;   //to exit from the while where rename in another directory
  while (!escape) {
    if(file.rename(sd.vwd(), nameoffile_C)){
      escape = true;
      Serial.println ("lo ho rimoninato in RTS  ");  // se esco qui ho rinominato il file senza trovare in RTS lo stesso nome
    }  
    else {
      Serial.println ("Error nel rinominarlo ");
      escape = true;                //togliere<<<<--------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
  }
  file.close();
  sd.ls(LS_R); 



  //mi sposto in RTS
  if (!sd.chdir("RTS/")) {
    sd.errorHalt("chdir failed. Did you run eventlog.pde?");
  }

  //leggo tutti i file in RTS, immagazzino i nomi in fileRTS[] e li sposto
  //char *fileRTS;
  char fileRTS[10];
  String sfileRTS;
  while (file.openNext(sd.vwd(), O_READ)) {
    //while (file.openNext(*RTS, O_READ)) {  
    file.getFilename(fileRTS);
    file.close();
    sfileRTS=String(fileRTS);
  }
  Serial.print("ho letto :");
  delay(1000);
  Serial.println(sfileRTS);
  //file.close();




  //da qui entro in lettura. leggo i dati e poi dovrò spedire storetoSNT
  //  int c;
  //  String fname;
  //  String storetoSND;   
  // // SdFile file1;
  //  ifstream file1(fileRTS);
  //  if (!file1.is_open()) sd.errorHalt("open failed in RTS");
  //
  //  while ((c = file.get()) >= 0) {
  //    storetoSND+= (char)c;
  //  }


  int c;
  String sfname;
  char fname[70];

  // String storetoSND;
  SdFile myFile;
  if (!myFile.open(fileRTS, O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  //while ((c = myFile.read()) >= 0) Serial.write(c);
  //  sd.ls(LS_R);



  while ((c = myFile.read()) >= 0) {
  //char line[70];
  //while ((c = myFile.fgets(line, sizeof(line))) >= 0){ 
   // Serial.write(c); 
    sfname += char(c);
    //file.close();
  }
  //sfname.toCharArray(fname, 70); 
  Serial.print("i dati ->");
  //  Serial.println(storetoSND);
  //  Serial.print("letti dal file");
  Serial.println(sfname);
  //  Serial.write(c);
  myFile.close();
  sd.ls(LS_R);

  Serial.println ("done"); 
}







