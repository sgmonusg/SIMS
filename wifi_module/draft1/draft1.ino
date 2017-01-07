#include <SoftwareSerial.h>
#define pc Serial
SoftwareSerial esp(10, 11);


// global variables
char buf_out[500];
String data_String;
int len_str=0;
char buf_in;
int i=0;

//***function variables ***//
String pin;
String conf_tag;


void setup() {
  Serial.begin(115200);
  esp.begin(115200);
  //delay(2000);
//  pc.println("\r\nPress a to begin process...");
//  while(pc.read()!='a');
  pc.println("Starting...");
  pc.println("Testing AT command...");
  delay(2000);
  esp.println("AT");
  delay(2000);
  if(des_resp("OK")==true){
    pc.println("Device found");
  }
  else{
    pc.println(esp.readString());
    pc.println("Device not found");
    while(1);
  }
  
  delay(2000);
  
  //RESET//
  esp.println("AT+RST");
  delay(2000);
  
  //TURN ECHO OFF//
  esp.println("ATE0");
  if(des_resp("OK")==true){
    pc.println("ECHO off");
  }
  
  delay(2000);
  
//  //IP ADDRESS DISPLAY//
//  esp.println("AT+CIFSR");
//  read_data_upto('"',buf_out,'"');
//  pc.println(buf_out);
//  
//  delay(2000);
  /*
  
  //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
  esp.println("AT+CIPMUX=1");
  if(des_resp("OK")==true){
    pc.println("CIPMUX SET TO 1, READY TO START TCP");
  }
  delay(5000);
  // create a cleint 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.104\",8008");
  while(esp.available()) pc.write(esp.read());
  
  delay(10000);
  
  
  //TO-DO GET USERPIN FOR SENDING TO PI
  //SEND DATA TO PI
  send_userpin("4444");
  
  //To-DO get the conf_tag for sending to pi
  // conf_tag can only hold two values (comp),(fail)
  send_transfer_confirmation("fail");
  
  */
  
  
  //Server create 
  // receive data on the pi
  esp.println("AT+CIPMUX=1");
  if(des_resp("OK")==true){
    pc.println("CIPMUX SET TO 1, READY TO START SERVER");
  }
  delay(2000);
  
  esp.println("AT+CIPSERVER=1,8008");
  delay(2000);
  if(des_resp("OK")==true){
    pc.println("server created");
  }
  else{
    pc.println(esp.readString());
    pc.println("server not created");
    while(1);
  }
  delay(2000);

  while(esp.available()) pc.write(esp.read());
//
  read_data_upto('\0',buf_out,':');
  pc.println(buf_out);
  //TO-DO WRITE  buf_out TO FILE ON THE SD-CARD IN ARDUINO


}
  
  //*****extra functions******//
  
bool des_resp(char resp[]){
    if(esp.available()){
      if(esp.find(resp)) 
        return true;
     }
    return false;
  }
  
  
void read_data_upto(char terminator,char buf[]){
  char buff;
  int i=0;
  if(esp.available())
  {
  buff=esp.read();
  while(buff!=terminator)
  {
  buff=esp.read();
  if(buff!=terminator) buf[i++]=buff;
  }
  buf[i]='\0';
  }
}

void read_data_upto(char terminator,char buf[],char begin_char)
{
  char buff;
  int i=0;
  if(esp.available())
  {
    buff=esp.read();
    while(buff!=begin_char || buff!=begin_char+1)
    {
      buff=esp.read();
    }
    buff=terminator+1;
    while(buff!=terminator)
    {
      buff=esp.read();
      if(buff!=terminator) buf[i++]=buff;
    }
  buf[i]='\0';
  }
}

void send_data(int len){
  pc.println("enter length of data to send:");
  delay(3000);
  int data_length = pc.parseInt();
  delay(1000);
  pc.println(data_length);
  delay(1000);
  pc.println("enter data to send to pi:");
  delay(3000);
  String cipsend = "AT+CIPSEND=0,";
  pc.println(cipsend.concat(data_length));
}

void send_userpin(String pin){
  esp.println("AT+CIPSEND=0,4");
  delay(2000);
  esp.println(pin);
  delay(2000);
  
}

void send_transfer_confirmation(String conf_tag){
  esp.println("AT+CIPSEND=0,4");
  delay(2000);
  esp.println(conf_tag);
  delay(2000);
}

void loop() {

while(pc.available()) esp.write(pc.read());
while(esp.available()) pc.write(esp.read());


}
