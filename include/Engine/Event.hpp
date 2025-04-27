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
      Event() = default;
      virtual ~Event() = default;

      /// Addition assignment for shorthand call to Event::AddCallbacks
      template<typename... Callbacks> void operator+=(const Callbacks &... callback) {
        (AddCallback(callback), ...);
      }
      /// Addition assignment for shorthand call to Event::RemoveCallbacks
      template<typename... Callbacks> void operator-=(const Callbacks &... callback) {
        (RemoveCallback(callback), ...);
      }

      /**
       * Registers a callback function that will be executed when the event is triggered.
       * @param callback The callback function to add. This should be a non-capturing lambda or function pointer.
       */
      void AddCallback(const Callback &callback) {
        callbacks.push_back(std::make_shared<Callback>(callback));
      }

      /**
       * Registers multiple callback functions that will be executed when the event is triggered.
       * @param callbacks The callback functions to add. Each should be a non-capturing lambda or function pointer.
       */
      template<typename... Callbacks> void AddCallbacks(const Callbacks &... callbacks) {
        (AddCallback(callbacks), ...);
      }

      /**
       * Unregisters a callback function, preventing it from being executed when the event is triggered.
       * @param callback The callback function to remove. Each should be a non-capturing lambda or function pointer.
       */
      void RemoveCallback(const Callback &callback) {
        auto it = std::remove_if(
          callbacks.begin(), callbacks.end(),
          [&callback](const Callback &ptr) {
            return ptr.template target<void(Args...)>() == callback.template target<void(Args...)>();
          }
        );
        callbacks.erase(it, callbacks.end());
      }

      /**
       * Remove multiple callback functions that will be executed when the event is triggered.
       * @param callbacks The callback functions to add. Each should be a non-capturing lambda or function pointer.
       */
      template<typename... Callbacks> void RemoveCallbacks(const Callbacks &... callbacks) {
        (RemoveCallback(callbacks), ...);
      }
    protected:
      /// Simple function before the Trigger function calls the callbacks
      virtual void preTrigger() {}
      /**
       * Executes each callback function that has been registered with the
       * event. Each callback is executed in the order it was registered.
       */
      void trigger(Args... args) {
        preTrigger();
        for (const std::shared_ptr<Callback> callback: callbacks) {
          (*callback.get())(args...);
        }
      }
    private:
      /// The list of all callbacks registered to this event
      std::vector<std::shared_ptr<Callback>> callbacks;
  };
}

#endif //EVENT_H
