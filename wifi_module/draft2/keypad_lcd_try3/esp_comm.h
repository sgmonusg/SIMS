SoftwareSerial esp(6,7);


void client_mode_pin(String shelf_id, String pin)
{  
  //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
  esp.println("AT+CIPMUX=1");
  if(des_resp("OK")==true){
    Serial.println("CIPMUX SET TO 1, READY TO START TCP");
  }
  delay(5000);
  // create a cleint 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.114\",8008");
  while(esp.available()) Serial.write(esp.read());
  
  delay(8000);
  
  String shelf_n_id = shelf_id + "*" + pin;
  //TO-DO GET USERPIN FOR SENDING TO PI
  //SEND DATA TO PI
  Serial.println(shelf_n_id);
  delay(2000);
  send_userpin(shelf_n_id);
  esp.println("AT+CIPCLOSE");
  delay(1000);
}

int server_mode(String list_of_items)
{
   //Server create 
   //receive data on the pi
    esp.println("AT+CIPMUX=1");
    if(des_resp("OK")==true){
      Serial.println("CIPMUX SET TO 1, READY TO START SERVER");
    }
    delay(2000);
    
    esp.println("AT+CIPSERVER=1,8008");
    delay(2000);
    if(des_resp("OK")==true){
      Serial.println("server created");
    }
    else{
      Serial.println(esp.readString());
      Serial.println("server not created");
      return 2;
    }
  
    delay(2000);
    while(esp.read()!=':');
    String message;
    while(esp.read()!='\0'){
      message = esp.readString();
    }
    Serial.println(message);
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

bool des_resp(char resp[])
{
    if(esp.available())
    {
      if(esp.find(resp)) 
        return true;
    }
    return false;
}

void send_RFID(String RFID)
{
  esp.println("AT+CIPSEND=0,12");
  delay(2000);
  esp.println(RFID);
  delay(2000);
}

void send_userpin(String pin)
{
  esp.println("AT+CIPSEND=0,6");
  delay(2000);
  esp.println(pin);
  delay(2000);  
}

int check_user_pin(String shelf_id, String pin,String list_of_items)
{
 client_mode_pin(shelf_id, pin);
 int status = server_mode(list_of_items);
 return status;
}

void shelf_withdrawal(String RFID)
{
    //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
  esp.println("AT+CIPMUX=1");
  if(des_resp("OK")==true)
  {
    Serial.println("CIPMUX SET TO 1, READY TO START TCP");
  }
  delay(5000);
  // create a cleint 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.114\",8008");
  while(esp.available()) Serial.write(esp.read());
  
  delay(8000);
  
  //TO-DO GET RFID FOR SENDING TO PI
  Serial.println(RFID);
  delay(2000);
  send_RFID(RFID);
  esp.println("AT+CIPCLOSE");
  delay(1000);

}
