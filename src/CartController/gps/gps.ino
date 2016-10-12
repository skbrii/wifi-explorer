#include <SoftwareSerial.h>

// all this shit doesn't work

// must implement gga, rmc, gsa

/* must be 

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
*/

byte gHash = 0;
char gsMessage[80];
byte gMessageStrIdx;

enum FORMAT { FORMAT_UNSUPPORTED, FORMAT_VTG, FORMAT_GLL, FORMAT_GSV, FORMAT_GGA, FORMAT_RMC, FORMAT_GSA };

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
}
 
char waitPeekCharFromSerial1() {
    while( ! Serial1.available() )
        delayMicroseconds(1);
    return Serial1.peek();    
}

void skipCurrentMessage() {
    char c = waitPeekCharFromSerial1();
    while( c != '$' ) {
        readCharFromSerial1();
        c = waitPeekCharFromSerial1();
    }
}

char waitReadCharFromSerial1() {
    while( ! Serial1.available() )
        delayMicroseconds(1);
    return Serial1.read();    
}

char readCharFromSerial1() {
    char c;
    static boolean hashModifyAllowed = true; 
    do
        c = waitReadCharFromSerial1();       
    while( c == ' ' ); //?
    if( c == '*' ) 
        hashModifyAllowed = false;
    if( c == '$' ) {
        gMessageStrIdx = 0;
        hashModifyAllowed = true;
        gHash = 0;
    }
    if( c == '\n' ) {
        gsMessage[ gMessageStrIdx++ ] = '\0';
    }
    
    if( hashModifyAllowed )
        gHash ^= c;
    gsMessage[ gMessageStrIdx++ ] = c;
    return c;
}

boolean checkMessageSource( const char* sSource ) {
    short int sourceWord = sSource[0];
    sourceWord <<= 8;
    sourceWord |= sSource[1];
    if( sourceWord == 0x4750 )
        return true;
    else 
        return false;
}

byte defineMessageFormat( const char* sFormat0 ) {
    int formatBytes = 0;
    formatBytes = sFormat0[2];
    formatBytes <<= 8;
    formatBytes |= sFormat0[3];
    formatBytes <<= 8;
    formatBytes |= sFormat0[4];
    switch( formatBytes ) {
        case 0x565447 : return FORMAT_VTG;
        case 0x474c4c : return FORMAT_GLL; 
        case 0x475356 : return FORMAT_GSV; 
        case 0x474741 : return FORMAT_GGA; 
        case 0x524d43 : return FORMAT_RMC; 
        case 0x475341 : return FORMAT_GSA;
        default : return FORMAT_UNSUPPORTED;               
    }    
}

void loop() {
    char symbol = readCharFromSerial1();
    byte formatType = FORMAT_UNSUPPORTED;
    boolean sourceIsCorrect = false;
    if( symbol != '$' ) {
        skipCurrentMessage();
        Serial.println( "Error! Invalid beginning. Message skipped." );
        return;    
    }
    char szFirstField[10];
    readFieldTo( szFirstField );
    sourceIsCorrect = checkMessageSource( szFirstField );
    if( ! sourceIsCorrect ) {
        skipCurrentMessage();
        Serial.println( "Error! Invalid source. Message skipped." );
        return;
    }
    formatType = defineMessageFormat( szFirstField );   
    if( formatType == FORMAT_UNSUPPORTED ) {
        skipCurrentMessage();
        Serial.println( "Error! Invalid format. Message skipped." );
        return;    
    }        
    switch( formatType ) {
//       case FORMAT_VTG : parseVtgMessage(); break;
//        case FORMAT_GLL : parseGllMessage(); break;
//        case FORMAT_GSA : parseGSAMessage(); break;
//        case FORMAT_GSV : parseGSAMessage(); break;
//        case FORMAT_RMC : parseRMCMessage(); break;
//        case FORMAT_GGA : parseGgaMessage(); break;
        default : 
          Serial.println( "Error! Format not supported yet. Message skipped." );
          //skipCurrentMessage();
          Serial.println( "The incorrect message was the following:" );
          Serial.println( gsMessage );
          Serial.println("");
          return;
    }    
    Serial.println( "The message was the following:" );
    Serial.println( gsMessage );    
}

struct Time {
    byte hours;
    byte minutes;
    float seconds;
} gTime ;

struct Date {
    byte day;
    byte month;
    short int year;
} gDate ;

struct Latitude {
    short int degrees;
    float minutes;
    char northSouth;
};

struct Longitude {
    short int degrees;
    float minutes;
    char westEast;
};

struct Position {
    Latitude latitude;
    Longitude longitude;
} gPosition ;

Time gFixTime;

// not completed 
// should be debugged
boolean parseGllMessage() {
    Serial.println( "Parsing GLL-message..." );    
    Position position;  
    Time fixTime; // it's Greenwich time !
    char szBuffer[15];
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        position.latitude.degrees = get2DigitIntFrom( & szBuffer[0] );
        position.latitude.minutes = (float) atof( & szBuffer[2] );    
    } else
        Serial.println( "Empty latitude field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        if( szBuffer[0] != 'N' && szBuffer[0] != 'S' ) {
            skipCurrentMessage();
            Serial.println( "Error! Invalid latitude type. Message skipped." );
            return false;
        } else
            position.latitude.northSouth = szBuffer[0];
    } else
        Serial.println( "Empty latitude type field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {    
        position.longitude.degrees = get2DigitIntFrom( & szBuffer[0] );
        position.longitude.minutes = (float) atof( & szBuffer[2] );
    } else
        Serial.println( "Empty longitude field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {    
        if( szBuffer[0] != 'E' && szBuffer[0] != 'W' ) {
            skipCurrentMessage();
            Serial.println( "Error! Invalid longitude type. Message skipped." );
            return false;
        } else
            position.longitude.westEast = szBuffer[0];                
    } else
        Serial.println( "Empty longitude type field." );    
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        fixTime.hours = (byte) get2DigitIntFrom( & szBuffer[0] );
        fixTime.minutes = (byte) get2DigitIntFrom( & szBuffer[2] );
        fixTime.seconds = (float) atof( & szBuffer[4] );
    } else
        Serial.print( "Empty fix time field." );
    boolean dataValid = false;
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        switch( szBuffer[0] ) {
            case 'V' : 
                //Serial.println( "GLL-data not valid." );
                break;
            case 'A' : 
                dataValid = true;
            default : 
                skipCurrentMessage();
                Serial.println( "Error! Invalid validacity field value. Message skipped." );
                return false;
        }
    } else {
        Serial.println( "Error! Empty validacity field." );
    }
    boolean ctrlSummIsCorrect = checkCtrlSumm();
    boolean dataIsRefreshed = false;     
    if( ctrlSummIsCorrect ) {
        if( dataValid ) {    
            gPosition = position;
            gFixTime = fixTime;
            Serial.println( gPosition.latitude.degrees );
            Serial.print( " deg " );
            Serial.print( position.latitude.minutes );
            Serial.print( " min " );
            Serial.print( position.latitude.northSouth );
            Serial.print( gPosition.longitude.degrees );
            Serial.print( " deg " );
            Serial.print( position.longitude.minutes );
            Serial.print( " min" );
            Serial.print( position.longitude.westEast );
            Serial.print( gFixTime.hours );
            Serial.print(":");
            Serial.print( gFixTime.minutes );
            Serial.print(":");
            Serial.print( gFixTime.seconds );
            Serial.print( " GMT" );
            dataIsRefreshed = true;    
        } else
            Serial.println( "Data not valid." );
    } else 
        Serial.println( "Error! Invalid check summ." );
    return dataIsRefreshed;
}

// not completed 
// should be debugged
boolean parseGgaMessage() {
    Position position;
    Time fixTime;
    char szBuffer[15]; 
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        fixTime.hours = (byte) get2DigitIntFrom( & szBuffer[0] );
        fixTime.minutes = (byte) get2DigitIntFrom( & szBuffer[2] );
        fixTime.seconds = (float) atof( & szBuffer[4] );  
    } else 
        Serial.println( "Empty fix time field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        position.latitude.degrees = get2DigitIntFrom( & szBuffer[0] );
        position.latitude.minutes = (float) atof( & szBuffer[0] );
    } else
        Serial.println( "Empty latitude field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {
        if( szBuffer[0] != 'N' && szBuffer[0] != 'S' ) {
            skipCurrentMessage();
            Serial.println( "Error! Invalid latitude type. Message skipped." );
            return false;
        }
        else
            position.latitude.northSouth = szBuffer[0];
    } else
        Serial.println( "Empty latitude type field." );
    readFieldTo( szBuffer );
    if( strlen( szBuffer ) != 0 ) {    
        position.longitude.degrees = get2DigitIntFrom( & szBuffer[0] );
        position.longitude.minutes = (float) atof( & szBuffer[2] );
    } else
        Serial.println( "Empty longitude field." );
    readFieldTo( szBuffer );  
    if( strlen( szBuffer ) != 0 ) { 
        if( szBuffer[0] != 'E' && szBuffer[0] != 'W' ) {
            skipCurrentMessage();
            Serial.println( "Error! Invalid longitude type. Message skipped." );
            return false;
        } else
            position.longitude.westEast = szBuffer[0];
    } else
        Serial.println( "Empty logitude type field." ); 
    Serial.print( '\n' );
    Serial.print( fixTime.hours );
    Serial.print( ':' );
    Serial.print( fixTime.minutes );
    Serial.print( ':' );
    Serial.print( fixTime.seconds );
    Serial.print( '\n' );
    skipCurrentMessage();
    return false;
}

short int get2DigitIntFrom( const char* szBuffer0 ) {
    char sz2DigitNum[3];
    byte i = 0;
    for( byte j = 0; i < 2; i++, j++ )
        sz2DigitNum[i] = szBuffer0[j];    
    sz2DigitNum[i] = '\0';
    return atoi( sz2DigitNum );
}

/*
void skipField() {
    char c;
    do
        c = readCharFromSerial1();
    while ( c != ',' || c != '*' );
}
*/

void readFieldTo( char* szBuffer0 ) {
    byte i = 0;    
    do  
        szBuffer0[i] = readCharFromSerial1();
    while( szBuffer0[ i++ ] != ',' );
    szBuffer0[ --i ] = '\0';
}

boolean checkCtrlSumm() {
    char szCtrlSumm[3];
    szCtrlSumm[0] = readCharFromSerial1();
    szCtrlSumm[1] = readCharFromSerial1();   
    szCtrlSumm[2] = '\0';
    char szHash[3];
    itoa( gHash, szHash, 16 );   
    if ( strcmp( szCtrlSumm, szHash ) )
        return false; 
    else
        return true;    
}
