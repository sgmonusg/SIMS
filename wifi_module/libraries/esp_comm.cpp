/*
	esp_comm.h - Library for sending and receiving using esp AT commands
*/

#include "Arduino.h"
#include "esp_comm.h"
#include "SoftwareSerial.h"

bool des_resp(char resp[])

esp_comm::esp_comm(int pin1, int pin2)
{
  _pin1 = pin1;
  _pin2 = pin2;
}

SoftwareSerial esp(_pin1,_pin2);

void client_mode_pin(String shelf_id, String pin){  
  //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
  esp.println("AT+CIPMUX=1");
  if(des_resp("OK")==true){
    Serial.println("CIPMUX SET TO 1, READY TO START TCP");
  }
  delay(500);
  // create a cleint 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.114\",8008");
    
  delay(1000);
  
  String shelf_n_id = shelf_id + "*" + pin;
  //TO-DO GET USERPIN FOR SENDING TO PI
  //SEND DATA TO PI
  Serial.println(shelf_n_id);
  delay(200);
  esp.println("AT+CIPSEND=0,6");
  delay(200);
  esp.println(shelf_n_id);
  delay(200);

  //esp.println("AT+CIPCLOSE");
  //delay(1000);
  
}

int server_mode(String list_of_items){
   //Server create 
   //receive data on the pi

    unsigned long now = millis();
    while (millis() - now < 1000)
    esp.read(); 
  
    esp.println("AT+CIPSERVER=1,8008");
    if(des_resp("OK")==true){
      Serial.println("server created");
    }
    else{
      Serial.println(esp.readString());
      Serial.println("server not created");
      return 2;
    }
  
    while(esp.read()!='#');
    String message;
    while(esp.read()!='!'){
      message = esp.readString();
    }
    Serial.println(message);
    if(message = "0")
    {
      delay(200);
      return 0;
    }
    else{
    list_of_items = message;
    delay(2000);
    return 1;
    }
}
  
  //*****extra functions******//
  
bool des_resp(char resp[]){
    if(esp.available()){
      if(esp.find(resp)) 
        return true;
    }
    return false;
}



int esp_comm::check_user_pin(String shelf_id, String pin,String list_of_items){
 client_mode_pin(shelf_id, pin);
 int server_status = server_mode(list_of_items);
 Serial.println(server_status);
 return server_status;
 return 1;
}

void esp_comm::shelf_withdrawal(String RFID){
  //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.114\",8008");
  while(esp.available()) Serial.write(esp.read());
  
  delay(8000);
  
  //TO-DO GET RFID FOR SENDING TO PI
  Serial.println(RFID);
  delay(200);
  esp.println("AT+CIPSEND=0,12");
  delay(200);
  esp.println(RFID);
  delay(200);


}
