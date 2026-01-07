#include <WiFi.h>
#include <AsyncUDP.h>
#include "Logger.h"
#include "Credentials.h"

AsyncUDP udp;
SemaphoreHandle_t mutex;
IPAddress client(0, 0, 0, 0);
uint16_t clientPort = 0;
const uint16_t WIFI_PORT = 57476;

boolean takeMutex()
{
	return xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE;
}

bool pairing = false;
char dump[512];

void onPacketReceive(AsyncUDPPacket packet)
{
	if (pairing && takeMutex())
	{
		client = packet.remoteIP();
		clientPort = packet.remotePort();
		Serial.printf("[heartbeat detected from %s:%d]\n", client.toString(), clientPort);
		udp.connect(client, clientPort);
		xSemaphoreGive(mutex);
	}
}

/**
 * @brief Send a buffer to the target client
 *
 * @param buffer Buffer
 * @param size Buffer size
 */
void writeUDP(uint8_t *buffer, size_t size)
{
	if (!takeMutex())
		return;
	if (udp.connected())
	{
		int sent = udp.writeTo(buffer, size, client, clientPort);
		if (sent != size) 
		{
			Serial.printf("Error sending packet: bytes %d/%d sent.\n", sent, size);
		}
	}
	xSemaphoreGive(mutex);
}

/**
 * @brief Send string data to the target client
 *
 * @param data String data
 */
void writeUDP(const char data[])
{
	writeUDP((uint8_t *)data, strlen(data));
}

void setupLogger()
{
	mutex = xSemaphoreCreateMutex();

	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}
	delay(1000); // Give it a minimum of 1s to fully connect
	println("Serial port connected.");

	println("Starting WiFi...");
	if ((SSID != NULL) && (SSID[0] != '\0'))
	{
		WiFi.softAP(SSID, PASSWORD);
		WiFi.setSleep(false);
		printformat(">>> Hosting at [%s]", WiFi.softAPIP().toString());
		delay(500);
		println("Opening UDP...");
		udp.listen(WIFI_PORT);
		udp.onPacket(onPacketReceive);
		println("UDP opened.");
		return;
	}
	println("Could not initialize WiFi due to empty SSID.");
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
	char buffer[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Serial.println(buffer);
	writeUDP(buffer);
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

void writeUDPF(const char *format, ...)
{
	if (!udp.connected() || client == IPAddress(0, 0, 0, 0))
		return;
	char buffer[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	// Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
	writeUDP(buffer);
}