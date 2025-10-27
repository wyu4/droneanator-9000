#include <Arduino.h>

/**
 * @brief Initializes everything needed to log data.
 *
 */
void startLogger();

/**
 * @brief Print a message to the logger
 *
 * @param message Message to log
 */
void print(const char message[]);

/**
 * @brief Print a message to the logger and create a new line
 *
 * @param message Message to log
 */
void println(const char message[]);

/**
 * @brief Print a message to the logger
 *
 * @param message Message to log
 */
void print(const Printable &message);

/**
 * @brief Print a message to the logger and create a new line
 *
 * @param message Message to log
 */
void println(const Printable &message);

/**
 * @brief Everything that should go in the flight controller's loop
 * 
 */
void loggerLoop();