#include <WiFiLink.h>
#include <SoftwareSerial.h>


char ssid[] = "AvansWlan"; //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
int ip[] = {52, 215, 115, 66};
IPAddress server(ip[0], ip[1], ip[2], ip[3]);
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(1200);


  //Check if communication with wifi module has been established
  if (WiFi.status() == WL_NO_WIFI_MODULE_COMM) {
    Serial.println("Communication with WiFi module not established.");
    while (true); // don't continue:
  }
  connectServer();
}

void connectWifi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    Serial.print('.');
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();



}

void connectServer() {
  if (status != WL_CONNECTED) {
    connectWifi();
  }
  Serial.println("\nStarting connection to server ...");
  // if you get a connection, report back via serial:
  while (!client.connected()) {

    if (client.connect(server, 10002)) {
      Serial.println("connected to server");
    }
    else {
      Serial.print('.');
      delay(10000);
    }
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  while (Serial.available()) {
    char c = Serial.read();
    client.print(c);
  }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println("disconnecting from server.");
    connectServer();
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
