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
  pc.begin(115200);
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
  String test_string;
  int a = check_user_pin("1","1234",test_string);
  pc.println(a);
}  
 
void client_mode_pin(String shelf_id, String pin){  
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
  
  delay(8000);
  
  String shelf_n_id = shelf_id + "*" + pin;
  //TO-DO GET USERPIN FOR SENDING TO PI
  //SEND DATA TO PI
  pc.println(shelf_n_id);
  delay(2000);
  send_userpin(shelf_n_id);
  esp.println("AT+CIPCLOSE");
  delay(1000);
  
  }

int server_mode(String list_of_items){
   //Server create 
   //receive data on the pi
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
      return 2;
    }
  
    delay(2000);
    while(esp.read()!=':');
    String message;
    while(esp.read()!='\0'){
      message = esp.readString();
    }
    pc.println(message);
    if(message = "0")
    {
      delay(2000);
      return 0;
    }
    else
    list_of_items = message;
    delay(2000);
    return 1;
}
  
  //*****extra functions******//
  
bool des_resp(char resp[]){
    if(esp.available()){
      if(esp.find(resp)) 
        return true;
     }
    return false;
}
  
  
void send_RFID(String RFID){
  esp.println("AT+CIPSEND=0,12");
  delay(2000);
  esp.println(RFID);
  delay(2000);
 }

void send_userpin(String pin){
  esp.println("AT+CIPSEND=0,6");
  delay(2000);
  esp.println(pin);
  delay(2000);
  
}


int check_user_pin(String shelf_id, String pin,String list_of_items){
 client_mode_pin(shelf_id, pin);
 int status = server_mode(list_of_items);
 return status;
}

void shelf_withdrawal(String RFID){
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
  
  delay(8000);
  
  //TO-DO GET RFID FOR SENDING TO PI
  pc.println(RFID);
  delay(2000);
  send_userpin(RFID);
  esp.println("AT+CIPCLOSE");
  delay(1000);

}

void loop() {

//while(pc.available()) esp.write(pc.read());
while(esp.available()) pc.write(esp.read());


}
