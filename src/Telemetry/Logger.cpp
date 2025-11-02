#include <WiFi.h>
#include "Logger.h"
//#include "Credentials.h"

WiFiUDP udp;
IPAddress client(0, 0, 0, 0);
const uint16_t WIFI_PORT = 57476;

bool udpStarted = false;
bool pairing = false;

/**
 * @brief Send a buffer to the target client
 *
 * @param buffer Buffer
 * @param size Buffer size
 */
void writeUDP(uint8_t *buffer, size_t size)
{
    if (!udpStarted)
        return;
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
    println("Serial port connected.");

    println("Starting WiFi...");
    //WiFi.softAP(SSID, PASSWORD);
    printformat(">>> Hosting at [%s]", WiFi.softAPIP().toString());
    delay(500);
    println("Opening UDP...");
    udp.begin(WIFI_PORT);
    println("UDP opened.");
    udpStarted = true;
}

void println(const char message[])
{
    Serial.println(message);
    writeUDP(message);
}

void println(const String message)
{
    println(message.c_str());
}

void printformat(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Serial.println(buffer);
    writeUDP(buffer);
}

void updateLogger()
{
    if (pairing && udp.parsePacket())
    {
        client = udp.remoteIP();
        printf("[heartbeat detected from %s]", client.toString());
    }
}

void setPairingMode(const bool enabled)
{
    pairing = enabled;
    if (enabled)
    {
        println("Enabling pairing mode.");
    }
    else
    {
        println("Disabling pairing mode.");
    }
}