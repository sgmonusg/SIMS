#include <SoftwareSerial.h>
#include <LCD5110_Graph.h>
#include <Keypad.h>
//#include <String.h>

#define Shelf_no 1

// global variables
char buf_out[500];
String data_String;
int len_str=0;
char buf_in;
int i=0;

typedef struct node
{
  short int item_number; 
  String item_name;
  String RFID_code;
  String box_no;
  String shelf_no;
  struct node* address;
}node;

struct node* root = malloc(sizeof(node));

/*
 * Everything related to keypad
 */

const byte numRows= 4;
const byte numCols= 4;

char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {A5, A4, A3, A2};
byte colPins[numCols] = {A1, A0, 3, 2};

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// object for SoftwareSerial class
SoftwareSerial myserial(5,4);
SoftwareSerial esp(6,7);

/*
 * Everything related to LCD Screen
 */
LCD5110 lcdobj(8, 9, 10, 12, 11);

extern uint8_t logo2[];
extern uint8_t SmallFont[];
extern uint8_t TinyFont[];
/*
 * functions for the lcd screen
 */

void screen_saver();
void greet();
String enterpin();
void nav_list();
void unload();
void create_list(String list);
int check_pin(int, String, String);
void list_remove(String RFID);
void list_goto(int counter);
void navigate_list();
int check_user_pin(String shelf_id, String pin,String list_of_items);
void client_mode_pin(String shelf_id, String pin);
int server_mode(String list_of_items);
bool des_resp(char resp[]);
void send_RFID(String RFID);
void send_userpin(String pin);
void shelf_withdrawal(String RFID);
void check_keyboard_input();


void setup() 
{
  Serial.begin(115200); // Open serial communications and wait for port to open:
  myserial.begin(9600);
  esp.begin(115200);
  esp.println("AT");
  delay(200);
  if(des_resp("OK")==true){
    Serial.println("Device found");
  }
  else{
    Serial.println(esp.readString());
    Serial.println("Device not found");
    while(1);
  }
  delay(200);
  //RESET//
  esp.println("AT+RST");
  delay(200);
  //TURN ECHO OFF//
  esp.println("ATE0"); 
  delay(200);
  lcdobj.InitLCD(); // initializing the LCD screen 
  lcdobj.setFont(SmallFont); // setting the font to display on the lcd screen
  lcdobj.clrScr();
  lcdobj.drawBitmap(10, -3, logo2, 63, 48); 
  lcdobj.update();
}

void loop()
{
  
  screen_saver();
  check_keyboard_input();
  //while(esp.available()) Serial.write(esp.read());
}



void screen_saver()
{
  lcdobj.clrScr();
  lcdobj.drawBitmap(10, -3, logo2, 63, 48); // display bitmap
  lcdobj.update();
  return;
}

String enterpin()
{
  String list_pin = "";
  int count = 0; 
  String letter = "";
  String star = "";
  lcdobj.clrScr();
  lcdobj.print("ENTER PIN", CENTER, 20);
  lcdobj.update();
  long exe_time = millis();
  while(count < 4)
  {
    if((letter = (String)myKeypad.getKey()) != (String)NO_KEY)
    {
      star += "*";
      lcdobj.clrScr();
      lcdobj.print("ENTER PIN", CENTER, 0);
      lcdobj.print(star, CENTER, 30);
      lcdobj.update();
      exe_time = 0;
      list_pin += letter;
      Serial.println(list_pin);
      count++;
      exe_time = millis();
    }
    if ((millis() - exe_time) > 100000)
    {
      Serial.println("time out") ;
      return (String)("");
    }
  }
  delay(1000);
  return list_pin;
}

void check_keyboard_input()
{
  int return_value;
  String the_list = "";
  if(myKeypad.getKey() == NO_KEY)
  {
    return;
  }
  else
  {    
    String list_pin = enterpin();
    if(list_pin=="")
    {      
      return;
    }

    else
    {
      // function to verify if pin is valid
      // and to check if there is server error
      return_value = check_user_pin((String)Shelf_no, list_pin, the_list);
      
      if (return_value == 0)
      {
        lcdobj.clrScr();
        lcdobj.print("INVALID PIN", CENTER, 20);
        lcdobj.update();
        return;
      }
//      else if(return_value == 2)
//      {
//        lcdobj.clrScr();
//        lcdobj.print("SERVER ERROR", CENTER, 20);
//        lcdobj.update();
//        return;
//      }
      else
      {
        create_list(the_list);
      }
    }
  }
  navigate_list();
}
// end of check if key iis pressed function

// beginning of create list function
void create_list(String list)
{
  int string_no = 0;
  int star_no = 0;
  node* list_cursor = root;
  while(string_no < list.length())
  {
    // create a new node
    node* link = malloc(sizeof(node));
    list_cursor -> address = link;
    list_cursor = list_cursor -> address;
    list_cursor -> item_number = ((int)(list[string_no]) - 48);
    string_no += 2;
    while(list[string_no] != '\n')
    {
      while(list[string_no] != '*')
      {
        list_cursor -> item_name += list[string_no];
        string_no++;
      }
      string_no++;
      while(list[string_no] != '*')
      {
        list_cursor -> RFID_code += list[string_no];
        string_no++;
      }
      string_no++;
      while(list[string_no] != '*')
      {
        list_cursor -> shelf_no += list[string_no];
        string_no++;
      }
      string_no++;
      while(list[string_no] != '*')
      {
        list_cursor -> box_no += list[string_no];
        string_no++;
      }
    }
    string_no++;
  }
  list_cursor -> address = NULL;
}
// end of create list function

// beginning of navigate function
void navigate_list()
{
  node* list_cursor = root;
  String letter = "";
  String RFID = "";
  int item_nos = 0; // counter variable to check the number of items on the list
  int counter = 0;
  int j = 0;
  // check the total number of items in the list
  while(list_cursor -> address != NULL)
  {
    item_nos++;
    list_cursor = list_cursor -> address;
  }    
  while(item_nos == 0 || (letter = myKeypad.getKey()) != 'C')
  {
    if (letter == '*')
    {
      // go up the list
      counter -= 1;
      if (counter < 0)
      {
        counter = item_nos - 1;
      }
      list_goto(counter);
    }
    else if (letter == '#')
    {
      // go down the list
      counter += 1;
      if (counter > item_nos - 1)
      {
        counter = 0;
      }
      list_goto(counter);
    }
    else if (letter == 'C')
    {
      unload();
      return;
    }
    else
    {
      // check if the RFID is read
      if (myserial.available())
      {
        while(myserial.available() && j < 12)
        {
          RFID += (String)myserial.read();
          delay(5);
        }
      }
      shelf_withdrawal(RFID);
      list_remove(RFID);
      delay(2000);
      j = 0;
      RFID = "";
      item_nos--;
      counter++;
    }
  }
  unload();
}

void list_goto(int counter)
{
  node* list_cursor = root;
  int j;
  for(j = 0; j < counter; j++)
  {
    list_cursor = list_cursor -> address;
  }
  lcdobj.clrScr();
  lcdobj.drawRect(0,0,83,32);
  lcdobj.print(list_cursor -> item_name, 1, 2 );
  lcdobj.print(list_cursor -> shelf_no, 1, 12 );
  lcdobj.print(list_cursor -> box_no, 1, 22 );
  lcdobj.print("* up # down", CENTER, 36);
  lcdobj.update();
}

void unload()
{
  node* temp_cursor = NULL;
  node* list_cursor = root;
  while(list_cursor -> address != NULL)
  {
    temp_cursor = list_cursor;
    list_cursor = list_cursor -> address;
    free(temp_cursor);
  }
  free(list_cursor);
  lcdobj.clrScr();
  lcdobj.invertText(false);
  lcdobj.print("DONE!!", CENTER, 0);
  lcdobj.update();
  delay(2000);
  lcdobj.clrScr();
  lcdobj.print("PLEASE SHUT", CENTER,15);
  lcdobj.print("THE DOOR.", CENTER, 25);
  lcdobj.update();
  delay(2000);
}

void list_remove(String RFID)
{
  node* list_cursor = root;
  node* temp_cursor = NULL;
  while(list_cursor -> RFID_code != RFID)
  {
    temp_cursor = list_cursor;
    list_cursor = list_cursor -> address;
  }
  temp_cursor = list_cursor -> address;
  free(temp_cursor);
}


void client_mode_pin(String shelf_id, String pin)
{  
  //ESP IN CLIENT MODE//
  //SEND DATA TO PI WHICH IS RUNNING server_script.py IP:192.168.0.104, PORT:8008//
  esp.println("AT+CIPMUX=1");
  delay(200);
  // create a cleint 
  esp.println("AT+CIPSTART=0,\"TCP\",\"192.168.0.114\",8008");
  delay(200);
  String shelf_n_id = shelf_id + "*" + pin;
  //TO-DO GET USERPIN FOR SENDING TO PI
  //SEND DATA TO PI
  Serial.println(shelf_n_id);
  delay(200);
  send_userpin(shelf_n_id);
  //esp.println("AT+CIPCLOSE");
  delay(200);
}

int server_mode(String list_of_items)
{
   //Server create 
   //receive data on the pi
    esp.println("AT+CIPMUX=1");
    delay(200);
    esp.println("AT+CIPSERVER=1,8008");
    delay(200);
    Serial.println("hi");
    Serial.println("hi");
    Serial.println("hi");
    while(esp.available()) Serial.write(esp.read());
    Serial.println("bye");
    Serial.println("hi");
    Serial.println("hi");
    
//    if(des_resp("OK")==true){
//      Serial.println("server created");
//    }
//    else{
//      Serial.println(esp.readString());
//      Serial.println("server not created");
//      return 2;
//    }
  
     // delay(2000);
//      String message;
//      while(esp.available()){
//        message += esp.read();
//      }
//      while(esp.read()!=':');
//    
//    while(esp.read()!='\0'){
//      message = esp.readString();
//    }
//      Serial.println(message);
//    if(message = "0")
//    {
//      delay(2000);
//      return 0;
//    }
//    else
//    list_of_items = message;
//    delay(2000);
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
  delay(200);
  esp.println(pin);
  delay(200);  
}

int check_user_pin(String shelf_id, String pin,String list_of_items)
{
 
 client_mode_pin(shelf_id, pin);
 int status = server_mode(list_of_items);
 return status;
}

void shelf_withdrawal(String RFID)
{
  Serial.println("hey");
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
