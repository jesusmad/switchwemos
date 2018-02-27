#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//WIFI
const char ssid[] = "Nope";
const char pass[] = "N0-T3-1mp0rt4!";
String MAC = "";
WiFiUDP Udp;

//HOST
const char * host = "nube.althreeus.com";
const unsigned int remotePort = 8888;

//OUTPUTS
const unsigned int led = 2;     
const unsigned int relayOne = 4; //D2
const unsigned int relayTwo = 5; //D1

//Buffer to hold incoming packets
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; 

//Relay status
int relaysStatus[] = {0, 0};

void setup() {
  configure();
  connectToWlan();
}

void configure() {
  delay(4000);
  pinMode(led, OUTPUT);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);
  MAC = WiFi.macAddress();
}

void connectToWlan() {
  //Connect to WLAN
  WiFi.begin(ssid, pass);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }  
}


void loop() {
  
 sendPacket();
 delay(100);
 readPacket();
 getValues();
 relays();
 
 delay(10000);
}

void sendPacket() {
 
   Udp.beginPacket(host, remotePort);
   Udp.println(MAC);
   Udp.endPacket();

   Udp.begin(Udp.localPort());  
}

void readPacket() {
  
  int packetSize = Udp.parsePacket();
  
  if (packetSize) {
      IPAddress remote = Udp.remoteIP();
        
      // Leo contenido del buffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    }
  
}

void getValues() {  

  char * token = strtok(packetBuffer, ":");
  
  while(token)
  {
    int rnum = atoi(token);
    token = strtok(NULL, ",");
    int rstatus = 0;
    if(token)
        rstatus = atoi(token);

    relaysStatus[rnum] = rstatus; 
   
    token = strtok(NULL, ":");
  }

  //Clean Packet Buffer
  for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) packetBuffer[i] = 0;
  
}

void relays() {

  for (int i = 0; i < 2; i++) {

    int stat = relaysStatus[i];
    
    switch (i) {
    case 0:
    
      if (stat == 0) digitalWrite(relayOne, LOW);
      else digitalWrite(relayOne, HIGH);  
      break;
      
    case 1:
    
      if (stat == 0) digitalWrite(relayTwo, LOW);      
      else digitalWrite(relayTwo, HIGH);
      break;
      
    default:
      break;
    }  
  }
  
}



