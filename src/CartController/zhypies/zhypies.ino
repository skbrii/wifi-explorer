#include <SoftwareSerial.h>

#define SoftSerial Serial1

#define RAZMER_STROKI 256

typedef enum Signature{ // <-- the us of typedef is optional.
    gga=0x474741,
    gll=0x474C4C,
    gsa=0x475341,
    gsv=0x475356,
    rmc=0x524D43,
    vtg=0x565447,
};

unsigned char stroka[RAZMER_STROKI];
int konets = 0;
byte est_nachalo = false;
char proverka[2];
long signature;
int strEnd;
char checkSumm[3]="";
boolean check=false;
char i=0;
int hash = 0;
char paramNum;


struct dannye_ot_zhps {
  byte vremya_chasy;
  byte vremya_minuty;
  byte vremya_sekundy;
  long shirota;
  long dolgota;
  byte kolichestvo_sputnikov;
  long pogreshnost;
  long visota;
  long skorost;
  long napravlenie;
  int data_god;
  byte data_mesyats;
  byte data_chislo;  
} dannye;

void setup() {
  SoftSerial.begin(9600);
  Serial.begin(9600);
}


void parse(char bukva, char paramNum){
  /*
  if(signature == 0 && strEnd > 6){
    //we have signature
    signature = 0;
    signature |= ((long)stroka[3])<<16;
    signature |= ((long)stroka[4])<<8;
    signature |= ((long)stroka[5]); 
  }*/
  
  if(paramNum==1 && strEnd <= 6 )
  {
    //we have signature bytes
    signature = 0;
    signature |= ((long)stroka[3])<<16;
    signature |= ((long)stroka[4])<<8;
    signature |= ((long)stroka[5]); 
    Serial.print(signature,HEX);
  }
  
  
  
  if (paramNum > 0)
  {
      //Serial.print("#");
      //Serial.print(paramNum,DEC);
      //Serial.print("|");
      Serial.print(bukva);
    
    //if* reset signature&&paramNum&&return
    //if, paramNum++
    //Serial.print("#");
    //Serial.print(bukva);
    
    //we have current bukva for analysing
    //2 counters: for args, for chars, splitted by ","
    //Serial.print(bukva);
  }
}

void loop() {
  if(SoftSerial.available()) {
    char bukva = SoftSerial.read();
    //stroka[strEnd++] = bukva;
    if(bukva == '$'){
      if (hash==strtol(checkSumm,NULL,16))
      {
        stroka_poluchena(); //finalization
      }
      paramNum=0;
      strEnd = 0;
      check=false;
      i=0;
      hash=0;
    }
    else
    {
      if(check == true){
        checkSumm[i++] = bukva;
      }
      else
      {
        //parse usual expression
        hash ^= bukva;
        if(bukva == ',')
        {
            paramNum++;
            Serial.print("#");
            Serial.print(paramNum,DEC);
            Serial.print("|");
        }
        else
        {   //parse like int float etc
            parse(bukva,paramNum);
            //put into storage
        }
        
      }
        
      if(bukva == '*'){
        hash ^= bukva;
        check=true;
      }
    }
    stroka[strEnd++] = bukva;
  }
}

void proverka_stroki() {
  unsigned char chislo = 0;
  for(int i = 1; i < konets - 3; i ++)
    chislo ^= stroka[i];
  proverka[0] = chislo_v_bukvu(chislo >> 4);
  proverka[1] = chislo_v_bukvu(chislo & 0x0f);
}

unsigned char chislo_v_bukvu(byte chislo) {
  return chislo < 10 ? '0' + chislo : 'A' + chislo - 10;
}


//Depricated
void stroka_poluchena() {  
  
  //Serial.print(" + ");
  //Serial.write(stroka, strEnd);
  
  Serial.print("---\n");
  
  return;
  
  signature = 0;
  signature |= ((long)stroka[3])<<16;
  signature |= ((long)stroka[4])<<8;
  signature |= ((long)stroka[5]); 
  
  /* sig decode
    gga 474741
    gll 474C4C
    gsa 475341
    gsv 475356
    rmc 524D43
    vtg 565447 */
  
  switch(signature){
    case 0x474741:
      razbor_gga();
      break;
    case 0x474C4C:
      razbor_gll();
      break;
    case 0x475341:
      razbor_gsa();
      break;
    case 0x475356:
      razbor_gsv();
      break;
    case 0x524D43:
      razbor_rmc();
      break;
    case 0x565447:
      razbor_vtg();
      break;
    default:
      Serial.print("Neponyatnoe soobschenie\n");
      break;
  }
}

void razbor_gga() {
  Serial.print("gga\n");
}

void razbor_gll() {
  Serial.print("gll\n");
}

void razbor_gsa() {
  Serial.print("gsa\n");
}

void razbor_gsv() {
  Serial.print("gsv\n");
}

void razbor_rmc() {
  Serial.print("rmc\n");
}

void razbor_vtg() {
  Serial.print("vtg\n");
}

void oshibochka(byte kod_oshibki) {
  Serial.print(" ! Oshibochka :( ");
  switch(kod_oshibki) {
    case 1:
      Serial.print("net okonchaniya stroki");
      break;
    case 2:
      Serial.print("net nachala stroki");
      break;
    case 3:
      Serial.print("proverka ne udalas'");
      break;
  }
  Serial.print("\n");
}
