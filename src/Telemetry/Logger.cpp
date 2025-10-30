#include <WiFi.h>
#include "Logger.h"
#include "Credentials.h"

WiFiUDP udp;
IPAddress client(0, 0, 0, 0);
const uint16_t WIFI_PORT = 57476;

bool udpStarted = false;

/**
 * @brief Send a buffer to the target client
 *
 * @param buffer Buffer
 * @param size Buffer size
 */
void writeUDP(uint8_t *buffer, size_t size)
{
    udp.beginPacket(client, WIFI_PORT);
    udp.write(buffer, size);
    udp.endPacket();
}

/**
 * @brief Send string data to the target client
 *
 * @param data String data
 */
inline void writeUDP(const char data[])
{
    writeUDP((uint8_t *)data, strlen(data));
}

void setupLogger()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(10);
    }
    delay(1000); // Give it a minimum of 1s to fully connect
    print(">>> Serial port connected");

    print("Starting WiFi...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD, 1, 0, 1, false);
    print(">>> Hosting at [");
    print(WiFi.softAPIP().toString());
    print("]\n");

    print("Opening UDP...");
    udp.begin(WIFI_PORT);
    print("UDP opened.\n");
    udpStarted = true;
}

void print(const char message[])
{
    Serial.print(message);
    writeUDP(message);
}

void print(const String message)
{
    print(message.c_str());
}

void updateLogger()
{
    if (udp.parsePacket())
    {
        client = udp.remoteIP();
        print("[heartbeat detected from ");
        print(client.toString());
        print("]\n");
    }
}
