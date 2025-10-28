#include <WiFi.h>
#include "Logger.h"
#include "Credentials.h"

WiFiUDP udp;
IPAddress client;
const uint16_t WIFI_PORT = 57476;

bool udpStarted = false;

void setupLogger()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(10);
    }
    delay(1000); // Give it a minimum of 1s to fully connect
    println(">>> Serial port connected");

    println("Starting WiFi...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD, 1, 0, 1, false);
    print(">>> Hosting at [");
    print(WiFi.softAPIP());
    println("]");

    println("Opening UDP...");
    udp.begin(WIFI_PORT);
    println("UDP opened.");
    udpStarted = true;
}

void print(const char message[])
{
    Serial.print(message);
}

void println(const char message[])
{
    Serial.println(message);
};

void print(const Printable &message)
{
    Serial.print(message);
}

void println(const Printable &message)
{
    Serial.println(message);
};

void updateLogger() {
    if (udp.parsePacket()) {
        client = udp.remoteIP();
        print("[heartbeat detected from ");
        print(client);
        println("]");
    }
}