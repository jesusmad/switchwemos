#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


//WIFI
const char ssid[] = "SSID";
const char pass[] = "PASS";

String MAC = "";
WiFiUDP Udp;

//HOST
const char * host = "yourdomain.com";
const unsigned int remotePort = 8888;

//MESSAGES
const char * motd = "MOTD";
String msg = "Sending message to ";

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

  msg = msg + host;
}

void configure() {
  Serial.println(motd);
  Serial.begin(115200);
  delay(4000);
  Serial.println("Opening outputs...");
  pinMode(led, OUTPUT);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);
  MAC = WiFi.macAddress();
  
}

void connectToWlan() {
  Serial.println("************** CONNECTING to the WLAN **************");

  Serial.print("WLAN: ");
  Serial.println(ssid);

//  NO DHCP
//  IPAddress ip(172,19,118,50);
//  IPAddress gateway(172,19,0,254);  
//  IPAddress subnet(255,255,0,0);
//  IPAddress dns(8, 8, 8, 8);
  
  WiFi.begin(ssid, pass);

//  NO DHCP
//  WiFi.config(ip, dns, gateway, subnet);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led, LOW);
  Serial.println("");
  Serial.println("Connected!");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(MAC);

  //Open UDP Port
  Udp.begin(Udp.localPort());  
}


void loop() {
  
 sendPacket();
 delay(100);
 readPacket();
 getValues();
 relays();
 
 delay(5000);
}

void sendPacket() {
   Serial.println("************** SENDING PACKET **************"); 
   Serial.println(msg);
   Udp.beginPacket(host, remotePort);
   Udp.println(MAC);
   Udp.endPacket();
   Serial.print("Message sent from port: ");
   Serial.println(Udp.localPort());
}

void readPacket() {
  Serial.println("************** READING PACKET **************"); 
  int packetSize = Udp.parsePacket();
  Serial.print("Received packet size: ");
  Serial.println(packetSize);
  
  if (packetSize) {
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      Serial.print(remote);
      Serial.print(", Port: ");
      Serial.println(Udp.remotePort());
  
      // Read buffer content
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      Serial.print("Packet Content: ");
      Serial.println(packetBuffer);
    }
  
}

void getValues() {  
  Serial.println("************** GETTING VALUES **************"); 
  char * token = strtok(packetBuffer, ":");
  
  while(token)
  {
    int rnum = atoi(token);
    token = strtok(NULL, ",");
    int rstatus = 0;
    if(token)
        rstatus = atoi(token);

    relaysStatus[rnum] = rstatus; 
     
    Serial.print("Relay number: ");
    Serial.println(rnum);
    Serial.print("Value: ");
    Serial.println(rstatus);

    token = strtok(NULL, ":");
  }

  //Clean Packet Buffer
  for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) packetBuffer[i] = 0;
  
}

void relays() {
  Serial.println("************** RELAYS VALUES **************");
  for (int i = 0; i < 2; i++) {
    Serial.print("Relay: ");
    Serial.println(i);
    Serial.print("Status: ");
    int stat = relaysStatus[i];
    char * estado = "";
    switch (i) {
    case 0:
      if (stat == 0) {
        digitalWrite(relayOne, LOW);
        estado = "OFF";
      }      
      else {
         digitalWrite(relayOne, HIGH);
         estado = "ON";
      }      
      break;
      
    case 1:
      if (stat == 0) {
        digitalWrite(relayTwo, LOW);
        estado = "OFF";
      }      
      else {
         digitalWrite(relayTwo, HIGH);
         estado = "ON";
      }
      break;
    default:
      break;
    }  
    Serial.println(estado);
  }
}



