//
// Event.h
// Author: Antoine Bastide
// Date: 16/11/2024
//

#ifndef EVENT_H
#define EVENT_H

#include <functional>

namespace Engine {
  /**
   * The Event class provides a way to register, deregister, and trigger
   * callback functions. It is designed for event-driven systems where
   * multiple actions (callbacks) can be associated with an event and
   * executed when the event occurs.
   *
   * @tparam Args The types of arguments that will be passed to each callback function when the event is triggered.
   */
  template<typename... Args> class Event {
    public:
      using Callback = std::function<void(Args...)>;
      using CallbackHandle = std::shared_ptr<Callback>;

      Event() = default;
      virtual ~Event() = default;

      /// Addition assignment for shorthand call to Event::AddCallbacks
      CallbackHandle operator+=(const Callback &callback) {
        return AddCallback(callback);
      }

      /// Addition assignment for shorthand call to Event::RemoveCallbacks
      void operator-=(const CallbackHandle &callback) {
        RemoveCallback(callback);
      }

      /**
       * Registers a callback function that will be executed when the event is triggered.
       * @param callback The callback function to add. This should be a non-capturing lambda or function pointer.
       * @returns the handle of this callback, used to late remove it if needed
       */
      CallbackHandle AddCallback(Callback callback) {
        auto handle = std::make_shared<Callback>(std::move(callback));
        callbacks.push_back(handle);
        return handle;
      }

      /**
       * Unregisters a callback function, preventing it from being executed when the event is triggered.
       * @param handle The handle of the callback to remove
       */
      void RemoveCallback(const CallbackHandle &handle) {
        callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), handle), callbacks.end());
      }
    protected:
      /// Executes each callback function that has been registered with the event. Each callback is executed in the order it was registered.
      void trigger(Args... args) {
        callbacks.erase(
          std::remove_if(
            callbacks.begin(), callbacks.end(), [&](const std::shared_ptr<Callback> &cb) {
              if (cb) {
                (*cb)(args...);
                return false;
              }
              return true;
            }
          ),
          callbacks.end()
        );
      }
    private:
      /// The list of all callbacks registered to this event
      std::vector<std::shared_ptr<Callback>> callbacks;
  };
}

#endif //EVENT_H
