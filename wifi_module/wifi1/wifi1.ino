#include<SoftwareSerial.h>

SoftwareSerial Serial1(10,11);
#define pc Serial
#define esp Serial1

char buf_out[500];
//String data="GET index.php?data=";
//String data1=" HTTP/1.1\r\nHost: 10.1.1.40\r\n\r\n";
String data_String;
int len_str=0;
char buf_in;
int i=0;
//const char* retstr="\n\r";
bool des_resp(char resp[])
{
if(esp.available())
{
if(esp.find(resp)) return true;
}
return false;
}
void setup() {

pc.begin(9600);
esp.begin(9600);
delay(5000);
pc.println("\r\nPress a to begin process...");
while(pc.read()!='a');
pc.println("Starting...");
pc.println("Testing AT command...");
delay(2000);
esp.println("AT");
delay(2000);
if(des_resp("OK")==true)
{
pc.println("Device found");
}
else
{
pc.println(esp.readString());
pc.println("Device not found");
while(1);
}
pc.println("Disabling echo and checking version...");
esp.println("ATE0");
delay(2000);
if(des_resp("OK")==true) pc.println("Echo off");
esp.println("AT+GMR");
delay(2000);
read_data_upto('\n',buf_out,'\n');
pc.print("Firmware:");
pc.println(buf_out);

esp.println("AT+CIPMUX=0");
delay(2000);
read_data_upto('\n',buf_out,'\n');
pc.println(buf_out);
esp.println("AT+CIPSTART=\"TCP\",\"10.1.1.40\",80");
delay(2000);
if(des_resp("Linked")==true) pc.println("Connected.");
pc.println("Enter your data and press enter:");
buf_in=0;
while(buf_in!=13)
{
if(pc.available())
{
buf_in=pc.read();
if(buf_in!=13) { buf_out[i++]=buf_in; pc.print(buf_in);
}
}
}
buf_out[i]='\0';
String str(buf_out);
data_String=data+str+data1;
len_str=data_String.length();
esp.print("AT+CIPSEND=");
esp.println(len_str);
esp.print(data_String);
delay(3000);
if(des_resp("SEND OK")==true) pc.println("\n\rDone.");
esp.println("AT+CIPCLOSE");

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
}
}

