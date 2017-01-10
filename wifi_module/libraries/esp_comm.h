#ifndef esp_comm_h
#define esp_comm_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class esp_comm
{
 public:
   esp_comm(int pin1, int pin2);
   int check_user_pin(String shelf_id, String pin,String list_of_items);
   void shelf_withdrawal(String RFID);
   String shelf_id;
   String pin;
   String list_of_items;
   char resp[];
   int _pin1;
   int _pin2;
};

#endif
