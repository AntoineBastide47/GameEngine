//
// InputContexts.h
// Author: Antoine Bastide
// Date9/11/2024
//

#ifndef INPUTCONTEXTS_H
#define INPUTCONTEXTS_H

namespace Engine::Input {
  /// The MouseContext struct provides a snapshot of the state of mouse buttons: pressed, help and release states.
  struct KeyAndButtonContext {
    /// Set to true when a mouse button is initially pressed.
    bool pressed;
    /// Set to true if a mouse button is actively pressed down, remaining true as long as the key is held.
    bool held;
    /// Set to true when a mouse button is released after being pressed.
    bool released;
  };
  /**
   * The KeyboardContext struct provides a snapshot of the state of primary keyboard keys
   * (pressed, help and release states) and common modifier keys (shift, control, alt, super, etc.).
   * This struct can be used to manage keyboard input and determine which keys or modifiers
   * are active, as well as the state of lock keys like caps lock and num lock.
   */
  struct KeyboardAndMouseContext : KeyAndButtonContext {
    /// Set to true if the left shift key is currently held down.
    bool leftShiftPressed;
    /// Set to true if the right shift key is currently held down.
    bool rightShiftPressed;
    /// Set to true if the left control key is currently held down.
    bool leftControlPressed;
    /// Set to true if the right control key is currently held down.
    bool rightControlPressed;
    /// Set to true if the left alt/option key is currently held down.
    bool leftAltPressed;
    /// Set to true if the right alt/option key is currently held down.
    bool rightAltPressed;
    /**
     * Set to true if the left Super key is currently held down.
     * - On Windows and Linux, this represents the Windows key (❖).
     * - On macOS, this represents the Command key (⌘).
     */
    bool leftSuperPressed;
    /**
     * Set to true if the right Super key is currently held down.
     * - On Windows and Linux, this represents the Windows key (❖).
     * - On macOS, this represents the Command key (⌘).
     */
    bool rightSuperPressed;
    /// Set to true if Caps Lock is currently held down.
    bool capsLockPressed;
    /// Set to true if Num Lock is currently held down.
    bool numLockPressed;
  };
  /// The MouseScroll struct provides a snapshot of the state of the mouse scroll: up and down.
  struct MouseScroll {
    /// Set to true when scrolling upwards.
    bool up;
    /// Set to true when scrolling downwards.
    bool down;
  };
}

#endif //INPUTCONTEXTS_H
