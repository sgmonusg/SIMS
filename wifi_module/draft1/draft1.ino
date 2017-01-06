#include <SoftwareSerial.h>

#define pc Serial
SoftwareSerial esp(10, 11);

char buf_out[500];
String data_String;
int len_str=0;
char buf_in;
int i=0;

bool des_resp(char resp[])
{
if(esp.available())
{
if(esp.find(resp)) return true;
}
return false;
}

void setup() {
  
Serial.begin(115200);
esp.begin(115200);
//delay(2000);
pc.println("\r\nPress a to begin process...");
while(pc.read()!='a');
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

//esp.println("AT+RST");
//delay(2000);

esp.println("AT+CWJAP=\"ARC\",\"arc@bphc\"");
delay(2000);


esp.println("AT+CIFSR");
delay(2000);
read_data_upto('\n',buf_out,'\n');
pc.println(buf_out);
pc.println("hey");
//pc.print("yay");
esp.println("AT+CIPMUX=1");
delay(2000);
esp.println("AT+CIPSERVER=1,8008");
delay(2000);
if(des_resp("OK")==true)
{
pc.println("server found");
}
else
{
pc.println(esp.readString());
pc.println("server not found");
while(1);
}
//esp.println("AT+CIPMODE=0");


pc.println("bye");
}

void read_data_upto(char terminator,char buf[])
{
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
while(buff!=begin_char)
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
void loop() {

while(pc.available()) esp.write(pc.read());
while(esp.available()) pc.write(esp.read());


}
