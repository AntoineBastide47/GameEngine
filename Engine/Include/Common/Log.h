//
// Log.h
// Author: Antoine Bastide
// Date: 14/11/2024
//

#ifndef LOG_H
#define LOG_H

#include <regex>
#include <string>
#include <unordered_set>

namespace Engine {
  /**
   * Simple logger class that allows the logging of messages, warnings and errors with the current stack trace
   * It is recommended to use the macros: LOG_MESSAGE, LOG_MESSAGE_WITH_TRACE, LOG_WARNING and LOR_ERROR instead of the
   * public functions Message, Warning and Error of the logger to automatically get the calling function's signature
   */
  class Log {
    public:
      /// Print's the given string to the console
      static void Print(const std::string &message);
      /// Print's the given string to the console and adds a new line at the end
      static void Println(const std::string &message);
      /// Print's the given string to the console
      static void Print(const char * &message);
      /// Print's the given string to the console and adds a new line at the end
      static void Println(const char * &message);
      /**
       * Sends the given message to the console with the current stack trace to pinpoint the place it was sent
       * @param msg The message to log
       * @param showTrace If the current stack trace should be logged, defaults to false
       */
      static void Message(const std::string &msg, bool showTrace = false);
      /**
       * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
       * @param msg The message to log
       */
      static void Warning(const std::string &msg);
      /**
       * Sends the given warning to the console with the current stack trace to pinpoint the place it was sent
       * @param msg The message to log
       * @returns nullptr
       */
      static std::nullptr_t Error(const std::string &msg);
    private:
      static std::unordered_set<int64_t> messagesSent;
      Log() = default;
      /**
       * Log's the given message to the console with the stack trace
       * @param msg The message to log
       * @param showTrace If the current stack trace should be logged, defaults to true
       */
      static void show(const std::string &msg, bool showTrace = true);

      /**
       * Takes the complete signature of a function an extracts the parts required to pinpoint the location of the log
       * @param funcSignature The signature of the function that logged the warning
       * @return The extracted signature where --- are sub-namespaces and ? is the operator symbol (+, -, *, /, ...):
       * - the caller function is an operator
       *   Namespace::---::operator?<br>
       * - the caller function is a function
       *   Namespace::---::name<br>
       */
      static std::string extractFromFuncSignature(const std::string &funcSignature);
      /// Hashes the input string and converts it to an int64
      static int64_t hashStringToInt64(const std::string &msg);
  };
}

#endif //LOG_H
