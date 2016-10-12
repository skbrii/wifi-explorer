#define exitFailSafe 3
#define motorForward 5
#define motorReverse 6
#define motorForward7b 9
#define motorReverse7b 10
#define motorBrake 18
#define motorStop 96

#define resetPin1 2
#define errorPin1 3

#define resetPin2 A0
#define errorPin2 A1

#define MIDPOINT 1

#include <SPI.h>
#include <Ethernet.h>

boolean Pressed=false;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,2,100);
EthernetServer server(80);

int power=800;
String a1="", a2="";

byte move=0, dir=0;
byte dmove=0, ddir=0;

void init_ethernet()
{
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  Serial.print("\n");
}

void init_pololu()
{
  pinMode(resetPin1,OUTPUT);
  pinMode(errorPin1,INPUT);

  pinMode(resetPin2,OUTPUT);
  pinMode(errorPin2,INPUT);

  Serial1.begin(9600);
  Serial2.begin(9600);
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
  Serial1.print(0xAA); //header
  Serial1.print(deviceID); //device identifyer
  Serial1.print(commandID); //command id
  for (int i=0;i<numDataBytes;i++)
  {  
    Serial1.print(data[i]); //
  }
}

void tst_read_status()
{
  //l 0xAA (170) device number 0x21 (33) variable ID
  Serial1.write(0xAA); //header
  Serial1.write(13); //device identifyer
  Serial1.write(33); //command id
  Serial1.write(1);

  delay(100);

  int numb;
  if(numb=Serial1.available())
  {
    Serial.print(numb,DEC);

    int status1 = Serial1.read();

    int status2 = Serial1.read();

    Serial.write("_");
    Serial.print(status1, BIN);
    Serial.write("_");
    Serial.print(status2, BIN);
    Serial.write("\n");
    Serial.write("Readed\n");
  }
}

void tst_send_command_exit_safe_start()
{
  //Serial1.print(0x83);

  Serial1.write(0xAA); //header
  Serial1.write(13); //device identifyer
  Serial1.write(3); //command id

  Serial2.write(0xAA); //header
  Serial2.write(13); //device identifyer
  Serial2.write(3); //command id

  Serial.print("Try safe start exit\n");
}

void tst_send_command_move_forward(int power)
{
  Serial1.write(0xAA); //header
  Serial1.write(13); //device identifyer
  Serial1.write(5); //command id

  int speed = power;
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  Serial1.write(speed_byte_1); //
  Serial1.write(speed_byte_2); //
  Serial.write("Try Forward\n");     
}

void runPower(int power)
{
  Serial1.write(0xAA); //header
  Serial1.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  Serial1.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  Serial1.write(speed_byte_1); //
  Serial1.write(speed_byte_2); //
  Serial.write("Try Forward\n");
}

void runPowerL(int power)
{
  Serial1.write(0xAA); //header
  Serial1.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  Serial1.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  Serial1.write(speed_byte_1); //
  Serial1.write(speed_byte_2); //
  //Serial.write("Try Forward\n");
}

void runPowerR(int power)
{
  Serial2.write(0xAA); //header
  Serial2.write(13); //device identifyer
  boolean reverse = (power >= 0)?0:1;

  Serial2.write(reverse?motorReverse:motorForward); //command id

    int speed = abs(power);
  byte speed_byte_1 = speed % 32;
  byte speed_byte_2 = speed / 32;

  Serial2.write(speed_byte_1); //
  Serial2.write(speed_byte_2); //
  //Serial.write("Try Forward\n");
}

void setup()
{
  Serial.begin(9600);
  //Serial1.begin(9600);
  init_pololu();
  reset_pololu();

  delay(10);
  tst_send_command_exit_safe_start();
  
  init_ethernet();
  //runPowerL(power);
  //runPowerR(power);
}

void show()
{
  Serial.write("Power is ");
  Serial.print(power, DEC);
  Serial.write('\n');
}

void performAction()
{
  if(Pressed)
    increasePower();
  else
    decreasePower();
  
}

void increasePower()
{
  if(power<1500)
  {
    power+=20;
    increaseAction();
    show();
    dmove=move;
    ddir=dir;
  }
}

void decreasePower()
{
    if(power>50)
    {
      power-=20;
      decreaseAction();
      show();
    }
    
}

void decreaseAction()
{
  if(dmove!=MIDPOINT)
  {
    if(dmove>MIDPOINT)
    {
      //Serial.print("Move forward");
        //move forward
        runPowerL(-power);
        runPowerR(power);
    }
    else
    {
      //Serial.print("Move backward");
        runPowerL(power);
        runPowerR(-power);
      //move backward
    }
  }
  if(ddir!=MIDPOINT)
  {
    if(ddir>MIDPOINT)
    {
      //Serial.print("Move right");
        runPowerL(power);
        runPowerR(power);
        //rotate right
    }
    else
    {
      //Serial.print("Move left");
        runPowerL(-power);
        runPowerR(-power);
      //rotate left
    }
  }
}

void increaseAction()
{
  if(move!=MIDPOINT)
  {
    if(move>MIDPOINT)
    {
      //Serial.print("Move forward");
        //move forward
        runPowerL(-power);
        runPowerR(power);
    }
    else
    {
      //Serial.print("Move backward");
        runPowerL(power);
        runPowerR(-power);
      //move backward
    }
  }
  if(dir!=MIDPOINT)
  {
    if(dir>MIDPOINT)
    {
      //Serial.print("Move right");
        runPowerL(power);
        runPowerR(power);
        //rotate right
    }
    else
    {
      //Serial.print("Move left");
        runPowerL(-power);
        runPowerR(-power);
      //rotate left
    }
  }
}

void loop()
{

  EthernetClient client = server.available();
  if (client) 
  {
    Serial.println("new client");
    while (client.connected()) {
      
      performAction();
      
/*      if(move!=MIDPOINT || dir!=MIDPOINT)
      {
        increasePower();
        dmove=move;
        ddir=dir;
      }
      else
      {
        decreasePower();
      }*/
      
      if(client.available()) 
      {
        
        Serial.write('New\n');
        char inByte=0;
        move = client.read();
        dir = client.read();
        
        if(move!=MIDPOINT || dir!=MIDPOINT)
        {
          if(power<400)
            power=400;
          Pressed=1;
        }
        else
        {
          Pressed=0;
        }
        
        Serial.print(move,DEC);
        Serial.write('\n');
        Serial.print(dir,DEC);
        Serial.write('\n');

        
        continue;
      }
    }
    Serial.println("client disconected");
  }
}
/*
  if (Serial.available()) {
 int inByte = Serial.read();   
 switch(inByte)
 {
 case 'h':
 Serial.write("r - Reset pololu\ns - status\ne - exit safe start\np - move\n+ - increase power\n- - decrease power\nl - list params\nf - moveforward\nb - backward\na - rotate\n");
 break;
 
 case 'r':
 reset_pololu();
 break;
 
 case 's':
 power=0;
 runPowerL(power);
 runPowerR(power);
 //read_status_pololu();
 break;
 
 case 'e':
 tst_send_command_exit_safe_start();
 break;
 
 case '+':
 if(power<4000)
 {
 power+=100;
 runPowerL(power);
 runPowerR(power);
 show();
 //        tst_send_command_move_forward(power);
 }
 break;
 
 case '-':
 if(power>-4000)
 {
 power-=100;
 runPowerL(power);
 runPowerR(power);
 show();
 //        tst_send_command_move_forward(power);
 
 }
 break;
 
 case 'l':
 Serial.write("Power is ");
 Serial.print(power, DEC);
 Serial.write('\n');
 break; 
 
 //case 'p':
 //  runPower(power);
 //      tst_send_command_move_forward(power);
 //  break;
 
 
 case 'f':
 runPowerL(power);
 runPowerR(-power);
 break;
 
 case 'b':
 runPowerL(-power);
 runPowerR(power);
 break;
 
 case 'a':
 runPowerL(power);
 runPowerR(power);
 break;
 
 case 'w':
 tst_read_status();
 break;  
 default: 
 ;
 }
 }
 }
 
 */


