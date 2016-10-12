#define exitFailSafe 3
#define motorForward 5
#define motorReverse 6
#define motorForward7b 9
#define motorReverse7b 10
#define motorBrake 18
#define motorStop 96

#define resetPin1 4
#define errorPin1 5

#define resetPin2 8
#define errorPin2 9

#define MIDPOINT 1

//#define MAXPOWER 1500
//#define MAXDIR 1200

#define MAXPOWER 1200
#define MAXDIR 1200

#define SENSORUPDATEINTERVAL 32000

#define FWDPIN A3
#define BWDPIN A2
#define LPIN A1
#define RPIN A0 //BLUE

#include <SoftwareSerial.h>

boolean Pressed=false;

int movePower=0;
int dirPower=0;

int movePressed=0;
int dirPressed=0;

int fPower=0;
int lDir=0;
  
int moveMul=0;
int dirMul=0;

String a1="", a2="";

byte move=0, dir=0;
byte dmove=0, ddir=0;

SoftwareSerial driverAlpha(3, 2); // RX, TX
SoftwareSerial driverOmega(7, 6); // RX, TX

int sensorUpdateTimer=SENSORUPDATEINTERVAL;

int batStatus1=0;
int batStatus2=0;

int key_w=0;
int key_s=0;
int key_a=0;
int key_d=0;

void init_pololu()
{

  
  
  
  
  
  pinMode(FWDPIN,INPUT);
  pinMode(BWDPIN,INPUT);
  pinMode(LPIN,INPUT);
  pinMode(RPIN,INPUT);
  
  digitalWrite(FWDPIN,LOW);
  digitalWrite(BWDPIN,LOW);
  digitalWrite(LPIN,LOW);
  digitalWrite(RPIN,LOW);

/*pinMode(A0,OUTPUT);
pinMode(A1,OUTPUT);
pinMode(A2,OUTPUT);
pinMode(A3,OUTPUT);
pinMode(A4,OUTPUT);
pinMode(A5,OUTPUT);
pinMode(A6,OUTPUT);
pinMode(A7,OUTPUT);
  digitalWrite(A0,HIGH);
 digitalWrite(A1,HIGH);
 digitalWrite(A2,HIGH);
 digitalWrite(A3,HIGH);
 digitalWrite(A4,HIGH);
 digitalWrite(A5,HIGH);
 digitalWrite(A6,HIGH);
 digitalWrite(A7,HIGH);
*/


  pinMode(resetPin1,OUTPUT);
  pinMode(errorPin1,INPUT);

  pinMode(resetPin2,OUTPUT);
  pinMode(errorPin2,INPUT);

  Serial.write("Inited\n");
   
}

void reset_pololu()
{
  digitalWrite(resetPin1,LOW);
  digitalWrite(resetPin2,LOW);
  delay(100);
  digitalWrite(resetPin1,HIGH);
  digitalWrite(resetPin2,HIGH);  
  Serial.write("Reseted\n"); 
}

void read_status_pololu()
{
  if(digitalRead(errorPin1))
    Serial.write("Errored\n");     
  else
    Serial.write("Clear\n");         
}

void send_command(byte deviceID, byte commandID, int numDataBytes, byte data[])
{
  driverAlpha.print(0xAA); //header
  driverAlpha.print(deviceID); //device identifyer
  driverAlpha.print(commandID); //command id
  for (int i=0;i<numDataBytes;i++)
  {  
    driverAlpha.print(data[i]); //
  }
}

void tst_read_status()
{
  //l 0xAA (170) device number 0x21 (33) variable ID
  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  driverAlpha.write(33); //command id
  driverAlpha.write(1);

  delay(100);

  int numb;
  if(numb=driverAlpha.available())
  {
    Serial.print(numb,DEC);
    int status1 = driverAlpha.read();
    int status2 = driverAlpha.read();
    Serial.write("_");
    Serial.print(status1, BIN);
    Serial.write("_");
    Serial.print(status2, BIN);
    Serial.write("\n");
    Serial.write("Readed\n");
  }
}

  //Pololu Protocol	0xAA (170)	device number	0x21 (33)	variable ID (get variable ommand)
  /*
  0	Error Status	The set bits of this variable indicate the errors that are currently stopping the motor. The motor can only be driven when this register has a value of 0. (See Section 3.4 for error descriptions.)
    Bit 0: Safe Start Violation
    Bit 1: Required Channel Invalid
    Bit 2: Serial Error
    Bit 3: Command Timeout
    Bit 4: Limit/Kill Switch
    Bit 5: Low VIN
    Bit 6: High VIN
    Bit 7: Over Temperature
    Bit 8: Motor Driver Error
    Bit 9: ERR Line High
    Bits 10-15: reserved
  1	Errors Occurred	The set bits of this register indicate the errors that have occurred since this register was last cleared. This status register has the same bit assignments as the Error Status register documented above. Reading this variable clears all of the bits.
  2	Serial Errors Occurred	The set bits of this variable indicate the serial errors that have occurred since this variable was last cleared. Reading this variable clears all of the bits. (See Section 3.4 for serial error descriptions.)
    Bit 0: reserved
    Bit 1: Frame
    Bit 2: Noise
    Bit 3: RX Overrun
    Bit 4: Format
    Bit 5: CRC
    Bits 6-16: reserved
  3	Limit Status	The set bits of this variable indicate things that are currently limiting the motor controller.
    Bit 0: Motor is not allowed to run due to an error or safe-start violation.
    Bit 1: Temperature is active reducing target speed.
    Bit 2: Max speed limit is actively reducing target speed (target speed > max speed).
    Bit 3: Starting speed limit is actively reducing target speed to zero (target speed < starting speed).
    Bit 4: Motor speed is not equal to target speed because of acceleration, deceleration, or brake duration limits.
    Bit 5: RC1 is configured as a limit/kill switch and the switch is active (scaled value ≥ 1600).
    Bit 6: RC2 limit/kill switch is active (scaled value ≥ 1600).
    Bit 7: AN1 limit/kill switch is active (scaled value ≥ 1600).
    Bit 8: AN2 limit/kill switch is active (scaled value ≥ 1600).
    Bit 9: USB kill switch is active.
    Bits 10-15: reserved
  127	Reset Flags	Flags indicating the source of the last board reset. This variable does not change while the controller is running and is not reported under the Status Tab of the Simple Motor Control Center. You can view it in the Device Information window of the Control Center, which is available from the Device menu, and for the first two seconds after start-up, the yellow status LED flashes a pattern that indicates the last reset source (see Section 3.5).
    0x04 (4): RST pin pulled low by external source.
    0x0C (12): Power reset (VIN got too low or was disconnected).
    0x14 (20): Software reset (by firmware upgrade process).
    0x24 (38): Watchdog timer reset (should never happen; this could indicate a firmware bug).
  20	Target Speed
  21	Speed
  22	Brake Amount
  23	Input Voltage
  24	Temperature
  28	System Time (Low)
  29	System Time (High)
  */
  
  
int tst_read_battery_status()
{
  
  //l 0xAA (170) device number 0x21(33) variable ID
  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  driverAlpha.write(33); //command id
  driverAlpha.write(23);

  delay(100);

  int numb;
  if(numb=driverAlpha.available())
  {
    //Serial.print(numb,DEC);
    byte status1 = driverAlpha.read();
    byte status2 = driverAlpha.read();
    
    //Serial.write("_");
    //Serial.print(status1, BIN);
    //Serial.write("_");
    //Serial.print(status2, BIN);
    //Serial.write("\\");
    
    int stat=status1 + 256*status2;
    return stat;
    
    //Serial.print(stat, DEC); 
    //Serial.write("\n");
    //Serial.write("Readed\n");
  }
}

int tst_read_battery_status_2()
{
  
  //l 0xAA (170) device number 0x21(33) variable ID
  driverOmega.write(0xAA); //header
  driverOmega.write(13); //device identifyer
  driverOmega.write(33); //command id
  driverOmega.write(23);

  delay(100);

  int numb;
  if(numb=driverOmega.available())
  {
    //Serial.print(numb,DEC);
    byte status1 = driverOmega.read();
    byte status2 = driverOmega.read();
    
    //Serial.write("_");
    //Serial.print(status1, BIN);
    //Serial.write("_");
    //Serial.print(status2, BIN);
    //Serial.write("\\");
    
    int stat=status1 + 256*status2;
    return stat;
    
    //Serial.print(stat, DEC); 
    //Serial.write("\n");
    //Serial.write("Readed\n");
  }
}


void tst_send_command_exit_safe_start()
{
  //driverAlpha.print(0x83);

  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  driverAlpha.write(3); //command id

  driverOmega.write(0xAA); //header
  driverOmega.write(13); //device identifyer
  driverOmega.write(3); //command id

  //Serial.print("Try safe start exit\n");
}

void tst_send_command_move_forward(int power)
{
  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  driverAlpha.write(5); //command id

  int speed = power;
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  driverAlpha.write(speed_byte_1); //
  driverAlpha.write(speed_byte_2); //
  Serial.write("Try Forward\n");     
}

void runPower(int power)
{
  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  driverAlpha.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  driverAlpha.write(speed_byte_1); //
  driverAlpha.write(speed_byte_2); //
  Serial.write("Try Forward\n");
}

void runPowerL(int power)
{
  driverAlpha.write(0xAA); //header
  driverAlpha.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  driverAlpha.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  driverAlpha.write(speed_byte_1); //
  driverAlpha.write(speed_byte_2); //
  //Serial.write("Try Forward\n");
}

void runPowerR(int power)
{
  driverOmega.write(0xAA); //header
  driverOmega.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  driverOmega.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  driverOmega.write(speed_byte_1); //
  driverOmega.write(speed_byte_2); //
  //Serial.write("Try Forward\n");
}

void setup()
{
  Serial.begin(9600);
  driverAlpha.begin(9600);
  driverOmega.begin(9600);
  //driverAlpha.begin(9600);
  init_pololu();
  reset_pololu();

  delay(10);
  tst_send_command_exit_safe_start();
  
  
  delay(100);
  if(digitalRead(errorPin1))
    Serial.println("err 1");
  if(digitalRead(errorPin2))
    Serial.println("err 2");
    
}

void show()
{
  Serial.write("Power is ");
  Serial.print(movePower, DEC);
  Serial.write('\n');
}

void action()
{
    
    if(move>MIDPOINT)
    {
      moveMul=1;
    }
    else
    if(move<MIDPOINT)
    {
      moveMul=-1;
    }
    
    if(dir>MIDPOINT)
    {
      dirMul=1;
    }
    else
    if(dir<MIDPOINT)
    {
      dirMul=-1;  
    }
  
  fPower=moveMul*movePower;
  lDir=dirMul*dirPower;
  
  tst_send_command_exit_safe_start();
  runPowerL(-fPower-lDir); //make some driver error: noise, protocol?!?
  runPowerR(fPower-lDir);
}

void performAction()
{
  char changed=0;
  if(movePressed)
    changed+=increaseMovePower();
  else
    changed+=decreaseMovePower();
  
  if(dirPressed)
    changed+=increaseDirPower();  
  else
    changed+=decreaseDirPower();
  
  if(changed)
  {
    action();
    Serial.print("We have changes\n");
  }
  else
  {
    updateSensors();
  }
}

char increaseMovePower()
{
  if(movePower<400)
    movePower=400;
    
  if(movePower<MAXPOWER)
  {
    movePower+=40;
    return 1;
  }
  else
  {
    return 0; 
  }
}


char decreaseMovePower()
{
    if(movePower>50)
    {
      movePower-=40;
      return 1;
    }
    else
    {
      return 0;
    }
}

char increaseDirPower()
{
  if(dirPower<400)
    dirPower=400;
    
  if(dirPower<MAXDIR)
  {
    dirPower+=80;
    return 1;
  }
  else
  {
    return 0;
  }
}


char decreaseDirPower()
{
    if(dirPower>50)
    {
      dirPower-=80;
      return 1;
    }
    else
    {
      return 0;
    }
}

void updateSensors()
{
  if(sensorUpdateTimer<=0)
  {
    batStatus1 = tst_read_battery_status();
    batStatus2 = tst_read_battery_status_2();
    sensorUpdateTimer=SENSORUPDATEINTERVAL;
    Serial.print("updated\n");
  }
  else
  {
    sensorUpdateTimer--;
  }
}

void readKeys()
{
  key_w=digitalRead(FWDPIN);
  key_s=digitalRead(BWDPIN);
  key_a=digitalRead(LPIN);
  key_d=digitalRead(RPIN);
}

void loop()
{
    Serial.println("new client");
    boolean changes = true;
    while (true) {

      performAction();
      
      if(changes) 
      {
        
        //Serial.print("New\n");
        char inByte=0;
        
        //get 
        //byte command = client.read();
        
        byte command = 1;
        
        switch(command)
        {
          case 0:
            reset_pololu();
            delay(10);
            tst_send_command_exit_safe_start();
            break;
            
          case 1:

            readKeys();
            
            move = 1+key_w-key_s;
            dir = 1-key_a+key_d;
           
            Serial.print(move);
            Serial.print(" ");
            Serial.println(dir);
            
           
            //move = 1; // client.read();
            //dir = 1;  // client.read();
            
            if(move!=MIDPOINT)
            {
              if(movePower<400)
                movePower=400;
              movePressed=1;
            }
            else
            {
              movePressed=0;
            }

            if(dir!=MIDPOINT)
            {
              if(dirPower<400)
                dirPower=400;
              dirPressed=1;
            }
            else
            {
              dirPressed=0;
            }
            
            break;
          
          case 2:
            move = 1;// client.read();
            dir = 1; // client.read();
            movePower=0;
            dirPower=0;
            
            break;
            
            
          case 10:
            readKeys();
            
            
            break;
          case 3:
            //Serial.print(batStatus1);
            //Serial.print(batStatus2);
            
          default:
            ;
        }
        
        continue;
      }
    }
    Serial.println("client disconected");
}



