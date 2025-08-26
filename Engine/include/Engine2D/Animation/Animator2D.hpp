//
// Animator.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATOR2D_HPP
#define ANIMATOR2D_HPP

#include <unordered_map>
#include <vector>

#include "Engine2D/Component2D.hpp"
#include "Engine2D/Animation/Animation2D.hpp"
#include "Animator2D.gen.hpp"

namespace Engine2D::Animation {
  class Animator2D : public Component2D {
    SERIALIZE_ANIMATOR2D
      friend class AnimationSystem;
    public:
      enum AnimationCondition {
        LESS_THAN, LESS_OR_EQUAL, GREATER_THAN, GREATER_OR_EQUAL, EQUAL, NOT_EQUAL, IF_TRUE, IF_FALSE, IF_TRIGGER
      };

      /// Maps the given animation to it's name in the animator
      /// @note If an animation with the same name already exists, the new one will overwrite it
      void AddAnimation(const std::string &name, const Animation2D &animation);
      /// Removes the animation with the given name
      void RemoveAnimation(const std::string &name);
      /// Play's the animation with the given name
      void Play(const std::string &name);
      /// Stops the animation from continuing to play
      void Pause() const;
      /// Allows the animation to play once again
      void Resume() const;
      /// Restarts the animation if it is non-looping
      void Restart() const;
      /// @returns A pointer to the animation with the given name, or nullptr if no animation has that name
      [[nodiscard]] Animation2D *GetAnimation(const std::string &name);
      /// @returns A pointer to the animation currently played by the animator
      [[nodiscard]] Animation2D *CurrentAnimation() const;

      /// Sets the value of the given boolean parameter
      /// @note Overrides the current value mapped to the given name
      void SetBool(const std::string &name, bool value);
      /// @returns the value of the given boolean parameter
      [[nodiscard]] bool GetBool(const std::string &name);

      /// Sets the value of the given integer parameter
      /// @note Overrides the current value mapped to the given name
      void SetInt(const std::string &name, int value);
      /// @returns the value of the given integer parameter
      [[nodiscard]] int GetInt(const std::string &name);

      /// Sets the value of the given floating point parameter
      /// @note Overrides the current value mapped to the given name
      void SetFloat(const std::string &name, float value);
      /// @returns the value of the given floating point parameter
      [[nodiscard]] float GetFloat(const std::string &name);

      /// Create's the given trigger parameter
      void CreateTrigger(const std::string &name);
      /// Activate's the given trigger parameter
      /// @note requires the given trigger to have been previously created
      void SetTrigger(const std::string &name);

      /// Set's the animation with the given name as the default animation played by the animator
      void SetDefaultAnimation(const std::string &name);

      /// Delete's all the parameters stored in the animator
      /// @note Once you call this, you can't revert the call
      void DeleteAllVariables();

      /**
       * Create's a new animation transition to switch between two animations with an exit time
       * @param from The name of the source animation
       * @param to The name of the destination animation
       * @param parameter The name of the parameter used to transition between the animations
       * @param condition The condition used on the parameter to transition between the animations
       * @param threshold The threshold used to check the condition applied to the given parameter
       * @param hasExitTime Whether the transition should start immediately or wait for a threshold
       * @param exitTime How much of the source animation should complete before the transition starts, between 0 and 1
       * @return The index of this transition relative to the "from" variable
       * @note Exit time will get clamped between 0 and 1 automatically.
       * @note Use the return index value to add more conditions to this transition if needed.
       */
      int CreateAnimationTransition(
        const std::string &from, const std::string &to, const std::string &parameter, AnimationCondition condition,
        float threshold = 0, bool hasExitTime = false, float exitTime = 1
      );

      /**
       * Remove's the given transition from the source animation
       * @param from The name of the source animation
       * @param index The index of this transition relative to the "from" variable
       */
      void RemoveAnimationTransition(const std::string &from, size_t index);

      /**
       * Add's a condition to an exiting transition
       * @param from The name of the source animation
       * @param index The index of this transition relative to the "from" variable
       * @param parameter The name of the parameter used to transition between the animations
       * @param condition The condition used on the parameter to transition between the animations
       * @param threshold The threshold used to check the condition applied to the given parameter
       */
      void AddAnimationTransitionCondition(
        const std::string &from, size_t index, const std::string &parameter, AnimationCondition condition,
        float threshold = 0
      );
      /**
       * Remove's a condition from a transition
       * @param from The name of the source animation
       * @param index The index of this transition relative to the "from" variable
       * @param parameter The name of the parameter used to transition between the animations
       */
      void RemoveAnimationTransitionCondition(const std::string &from, size_t index, const std::string &parameter);

      /// @returns The given animation condition converted to a string representation
      static std::string AnimationConditionToString(AnimationCondition condition);

      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      enum ParameterType {
        BOOL, INT, FLOAT, TRIGGER
      };

      struct Parameter : Reflectable {
        SERIALIZE_PARAMETER
          ParameterType type;

          union {
            bool boolean;
            int integer;
            float floating;
            bool trigger;
          };

          Parameter()
            : type(BOOL), boolean(false) {}

          explicit Parameter(const int v)
            : type(INT), integer(v) {}

          explicit Parameter(const float v)
            : type(FLOAT), floating(v) {}

          Parameter(const bool v, const ParameterType type)
            : type(type), boolean(v) {}
      };

      struct TransitionCondition : Reflectable {
        SERIALIZE_TRANSITIONCONDITION
          std::string parameter;
          AnimationCondition condition;
          float threshold;

          TransitionCondition() = default;

          TransitionCondition(const std::string &parameter, const AnimationCondition condition, const float threshold)
            : parameter(parameter), condition(condition), threshold(threshold) {}
      };

      struct AnimationTransition : Reflectable {
        SERIALIZE_ANIMATIONTRANSITION
          std::string to;
          bool hasExitTime;
          float exitTime;
          std::vector<TransitionCondition> conditions;

          AnimationTransition() = default;

          AnimationTransition(
            const std::string &to, const bool hasExitTime, const float exitTime,
            const std::vector<TransitionCondition> &conditions
          )
            : to(to), hasExitTime(hasExitTime), exitTime(exitTime), conditions(conditions) {}
      };

      /// The animations that this animator can play
      ENGINE_SERIALIZE std::unordered_map<std::string, Animation2D> animations;
      /// All the user defined animator parameters
      ENGINE_SERIALIZE std::unordered_map<std::string, Parameter> parameters;
      /// All the user defined animator transitions
      ENGINE_SERIALIZE std::unordered_map<std::string, std::vector<AnimationTransition>> transitions;
      /// The animation currently played by the animator
      Animation2D *currentAnimation;
      /// The name of the animation currently played by the animator
      ENGINE_SERIALIZE std::string currentAnimationName;
      /// The animation played by default by the animator
      Animation2D *defaultAnimation;
      /// The name of the animation currently played by the animator
      ENGINE_SERIALIZE std::string defaultAnimationName;

      void forward() override;
      void recall() override;

      /// Updates the current animation and transitions
      void update();
      /// @returns the name of the next animation of the first transition that is valid, "" if not
      const std::string &checkForNextAnimation();
      /// @returns The given parameter type converted to a string representation
      static std::string parameterTypeToString(ParameterType type);
  };
}

#endif //ANIMATOR2D_HPP
