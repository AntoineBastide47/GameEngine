//
// Log.h
// Author: Antoine Bastide
// Date: 14/11/2024
//

#ifndef LOG_H
#define LOG_H

#include <regex>
#include <string>

namespace Engine {
  /**
   * Simple logger class that allows the logging of messages, warnings and errors with the current stack trace
   * It is recommended to use the macros: LOG_MESSAGE, LOG_MESSAGE_WITH_TRACE, LOG_WARNING and LOR_ERROR instead of the
   * public functions Message, Warning and Error of the logger to automatically get the calling function's signature
   */
  // TODO: add HashMap[(log, count)] to check if log was already sent to not pollute the console
  class Log {
    public:
      /** Print's the given string to the console */
      static void print(const std::string &message);
      /** Print's the given string to the console and adds a new line at the end */
      static void println(const std::string &message);
      /** Print's the given string to the console */
      static void print(const char* &message);
      /** Print's the given string to the console and adds a new line at the end */
      static void println(const char* &message);
      /**
       * Sends the given message to the console with the current stack trace to pinpoint the place it was sent
       * @param msg The message to log
       * @param funcSignature The signature of the function that logged the error
       * @param showTrace If the current stack trace should be logged, defaults to false
       */
      static void Message(const std::string &msg, const std::string &funcSignature, bool showTrace = false);
      /**
       * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
       * @param funcSignature The signature of the function that logged the warning
       * @param msg The message to log
       */
      static void Warning(const std::string &msg, const std::string &funcSignature);
      /**
       * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
       * @param funcSignature The signature of the function that logged the error
       * @param msg The message to log
       * @returns nullptr
       */
      static std::nullptr_t Error(const std::string &msg, const std::string &funcSignature);
    private:
      Log() = default;
      /**
       * Log's the given message to the console with the stack trace
       * @param msg The message to log
       * @param showTrace If the current stack trace should be logged, defaults to true
       */
      static void show(const std::string &msg, bool showTrace = true);

      /**
       * Takes the complete signature of a function a extracts the parts required to pinpoint the location of the log
       * @param funcSignature The signature of the function that logged the warning
       * @return The extracted signature where --- are sub-namespaces and ? is the operator symbol (+, -, *, /, ...):
       * - the caller function is an operator
       *   Namespace::---::operator?<br>
       * - the caller function is a function
       *   Namespace::---::name<br>
       */
      static std::string extractFromFuncSignature(const std::string &funcSignature);
  };
}

/**
 * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
 * @param msg std::string => The message to log
 */
#define LOG_MESSAGE(msg) Engine::Log::Message(msg, __PRETTY_FUNCTION__, false)
/**
 * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
 * @param msg std::string => The message to log
 */
#define LOG_MESSAGE_WITH_TRACE(msg) Engine::Log::Message(msg, __PRETTY_FUNCTION__, true)
/**
 * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
 * @param msg std::string => The message to log
 */
#define LOG_WARNING(msg) Engine::Log::Warning(msg, __PRETTY_FUNCTION__)
/**
 * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
 * @param msg std::string => The message to log
 */
#define LOG_ERROR(msg) Engine::Log::Error(msg, __PRETTY_FUNCTION__)

#endif //LOG_H
