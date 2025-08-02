//
// Animator.cpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#include "Engine2D/Animation/Animator2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Macros/Assert.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Animation/AnimationSystem.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D::Animation {
  void Animator2D::AddAnimation(const std::string &name, const Animation2D &animation) {
    if (const auto it = animations.find(name); it != animations.end())
      it->second = animation;
    else
      animations.emplace(name, animation);

    // Set the default and current animation to the first added animation
    if (!defaultAnimation && animations.size() == 1) {
      defaultAnimation = &animations.begin()->second;
      defaultAnimationName = name;
    }
  }

  void Animator2D::RemoveAnimation(const std::string &name) {
    animations.erase(name);
  }

  void Animator2D::Play(const std::string &name) {
    if (animations.contains(name)) {
      Animation2D &animation = animations.at(name);
      currentAnimation = &animation;
      currentAnimationName = name;
      Restart();

      const auto renderer = Entity()->GetComponent<Rendering::SpriteRenderer>();
      if (!renderer)
        return;
      renderer->SetSprite(Engine::ResourceManager::GetSprite(animation.spriteName));
    }
  }

  void Animator2D::Pause() const {
    if (currentAnimation)
      currentAnimation->paused = true;
  }

  void Animator2D::Resume() const {
    if (currentAnimation)
      currentAnimation->paused = false;
  }

  void Animator2D::Restart() const {
    if (currentAnimation) {
      auto &anim = *currentAnimation;
      anim.currentFrame = anim.reverse ? 0 : (anim.vertical ? anim.frameCountY : anim.frameCountX) - 1;
      anim.elapsedTime = 0;
      anim.completed = false;
      anim.paused = false;
    }
  }

  Animation2D *Animator2D::GetAnimation(const std::string &name) {
    if (const auto it = animations.find(name); it != animations.end())
      return &it->second;
    return nullptr;
  }

  Animation2D *Animator2D::GetCurrentAnimation() const {
    return currentAnimation;
  }

  void Animator2D::SetBool(const std::string &name, const bool value) {
    parameters[name] = Parameter{value, BOOL};
  }

  bool Animator2D::GetBool(const std::string &name) {
    ENGINE_ASSERT(parameters.contains(name), "Parameter " + name + " of type bool does not exist in this animator.");
    ENGINE_ASSERT(parameters[name].type == BOOL, "Parameter " + name + " is not of type bool.");
    return parameters[name].boolean;
  }

  void Animator2D::SetInt(const std::string &name, const int value) {
    parameters[name] = Parameter{value};
  }

  int Animator2D::GetInt(const std::string &name) {
    ENGINE_ASSERT(parameters.contains(name), "Parameter " + name + " of type integer does not exist in this animator.");
    ENGINE_ASSERT(parameters[name].type == INT, "Parameter " + name + " is not of type integer.");
    return parameters[name].integer;
  }

  void Animator2D::SetFloat(const std::string &name, const float value) {
    parameters[name] = Parameter{value};
  }

  float Animator2D::GetFloat(const std::string &name) {
    ENGINE_ASSERT(
      parameters.contains(name), "Parameter " + name + " of type floating point does not exist in this animator."
    );
    ENGINE_ASSERT(parameters[name].type == FLOAT, "Parameter " + name + " is not of type floating point.");
    return parameters[name].floating;
  }

  void Animator2D::CreateTrigger(const std::string &name) {
    parameters[name] = Parameter{false, TRIGGER};
  }

  void Animator2D::SetTrigger(const std::string &name) {
    ENGINE_ASSERT(parameters.contains(name), "Parameter " + name + " of type trigger does not exist in this animator.");
    ENGINE_ASSERT(parameters[name].type == TRIGGER, "Parameter " + name + " is not of type trigger.");
    parameters[name].trigger = true;
  }

  void Animator2D::SetDefaultAnimation(const std::string &name) {
    ENGINE_ASSERT(animations.contains(name), "Cannot set non existent " + name + " animation as default.");
    defaultAnimation = &animations.at(name);
  }

  void Animator2D::DeleteAllVariables() {
    parameters.clear();
  }

  int Animator2D::CreateAnimationTransition(
    const std::string &from, const std::string &to, const std::string &parameter, const AnimationCondition condition,
    const float threshold, bool hasExitTime, const float exitTime
  ) {
    ENGINE_ASSERT(
      animations.contains(from), "Cannot create a transition to target non existent " + from + " animation."
    );
    ENGINE_ASSERT(animations.contains(to), "Cannot create a transition to target non existent " + to + " animation.");
    ENGINE_ASSERT(
      parameters.contains(parameter),
      "Cannot create a transition that target's non existent " + parameter + " parameter"
    );

    const Parameter &param = parameters[parameter];
    bool invalidCondition = false;
    switch (param.type) {
      case BOOL:
        invalidCondition = condition != IF_TRUE && condition != IF_FALSE;
        break;
      case INT:
      case FLOAT:
        invalidCondition = condition > NOT_EQUAL;
        break;
      case TRIGGER:
        invalidCondition = condition != IF_TRIGGER;
        break;
    }

    if (!invalidCondition)
      invalidCondition = condition > IF_TRIGGER;

    ENGINE_ASSERT(
      !invalidCondition,
      "Transition condition " + AnimationConditionToString(condition) +
      " is not supported for parameter type " + parameterTypeToString(param.type)
    )

    transitions[from].emplace_back(
      to, hasExitTime, hasExitTime ? std::clamp(exitTime, 0.0f, 1.0f) : 0,
      std::vector{TransitionCondition{parameter, condition, threshold}}
    );
    return transitions[from].size() - 1;
  }

  void Animator2D::RemoveAnimationTransition(const std::string &from, const int index) {
    ENGINE_ASSERT(
      animations.contains(from), "Cannot create a transition to target non existent " + from + " animation."
    );
    ENGINE_ASSERT(0 <= index && index < transitions[from].size(), "Invalid transition index: " + std::to_string(index));

    transitions[from].erase(transitions[from].begin() + index);
  }

  void Animator2D::AddAnimationTransitionCondition(
    const std::string &from, const int index, const std::string &parameter, AnimationCondition condition,
    float threshold
  ) {
    ENGINE_ASSERT(
      animations.contains(from), "Cannot create a transition to target non existent " + from + " animation."
    );
    ENGINE_ASSERT(0 <= index && index < transitions[from].size(), "Invalid transition index: " + std::to_string(index));
    ENGINE_ASSERT(
      parameters.contains(parameter),
      "Cannot create a transition that target's non existent " + parameter + " parameter"
    );

    const Parameter &param = parameters[parameter];
    bool invalidCondition = false;
    switch (param.type) {
      case BOOL:
        invalidCondition = condition != IF_TRUE && condition != IF_FALSE;
        break;
      case INT:
      case FLOAT:
        invalidCondition = condition > NOT_EQUAL;
        break;
      case TRIGGER:
        invalidCondition = condition != IF_TRIGGER;
        break;
    }

    if (!invalidCondition)
      invalidCondition = condition > IF_TRIGGER;

    ENGINE_ASSERT(
      !invalidCondition,
      "Transition condition " + AnimationConditionToString(condition) +
      " is not supported for parameter type " + parameterTypeToString(param.type)
    )

    const auto it = std::ranges::find_if(
      transitions[from][index].conditions, [parameter](const TransitionCondition &cond) {
        return cond.parameter == parameter;
      }
    );

    if (it == transitions[from][index].conditions.end())
      transitions[from][index].conditions.emplace_back(parameter, condition, threshold);
    else {
      it->condition = condition;
      it->threshold = threshold;
    }
  }

  void Animator2D::RemoveAnimationTransitionCondition(
    const std::string &from, const int index, const std::string &parameter
  ) {
    ENGINE_ASSERT(
      animations.contains(from), "Cannot create a transition to target non existent " + from + " animation."
    );
    ENGINE_ASSERT(0 <= index && index < transitions[from].size(), "Invalid transition index: " + std::to_string(index));
    ENGINE_ASSERT(
      parameters.contains(parameter),
      "Cannot create a transition that target's non existent " + parameter + " parameter"
    );

    std::erase_if(
      transitions[from][index].conditions, [parameter](const TransitionCondition &condition) {
        return condition.parameter == parameter;
      }
    );
  }

  std::string Animator2D::AnimationConditionToString(const AnimationCondition condition) {
    switch (condition) {
      case LESS_THAN:
        return "LESS_THAN";
      case LESS_OR_EQUAL:
        return "LESS_OR_EQUAL";
      case GREATER_THAN:
        return "GREATER_THAN";
      case GREATER_OR_EQUAL:
        return "GREATER_OR_EQUAL";
      case EQUAL:
        return "EQUAL";
      case NOT_EQUAL:
        return "NOT_EQUAL";
      case IF_TRUE:
        return "IF_TRUE";
      case IF_FALSE:
        return "IF_FALSE";
      case IF_TRIGGER:
        return "IF_TRIGGER";
      default:
        return "UNKNOWN";
    }
  }

  void Animator2D::OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) {
    if (!defaultAnimation && !defaultAnimationName.empty() && animations.contains(defaultAnimationName))
      defaultAnimation = &animations.at(defaultAnimationName);

    if (!currentAnimation && !currentAnimationName.empty() && animations.contains(currentAnimationName))
      currentAnimation = &animations.at(currentAnimationName);
  }

  void Animator2D::forward() {
    Entity()->Scene()->animationSystem.addAnimator(this);
  }

  void Animator2D::recall() {
    Entity()->Scene()->animationSystem.removeAnimator(this);
  }

  void Animator2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (animations.empty())
      return;

    const auto renderer = Entity()->GetComponent<Rendering::SpriteRenderer>();
    if (!renderer)
      return;

    if (!currentAnimation)
      Play(defaultAnimationName);

    const auto sprite = renderer->GetSprite();
    if (!sprite)
      return;

    auto &rect = sprite->rect;
    if (Animation2D &animation = *currentAnimation; !animation.paused && !animation.completed) {
      // Increment the duration of the animation
      animation.elapsedTime += Game2D::DeltaTime();

      // If any transitions conditions are met, change the currently playing animation
      if (const std::string nextAnimation = checkForNextAnimation(); nextAnimation != "")
        Play(nextAnimation);

      // Set the current frame of the animation
      const int frameCount = animation.FrameCount();
      const float totalDuration = frameCount * animation.frameDuration;
      const int frame = std::floor(std::fmod(animation.elapsedTime, totalDuration) * animation.frameDurationInv);
      if (animation.reverse)
        animation.currentFrame = frameCount - 1 - frame;
      else
        animation.currentFrame = frame;

      // When at the end of the animation, check if it needs to restart
      if (animation.elapsedTime >= totalDuration) {
        Restart();
        if (!animation.loop)
          animation.completed = true;
      }

      // Update the rect of the sprite to reflect the animation
      if (animation.vertical) {
        rect.y = (animation.currentFrame + animation.frameOffsetY) * rect.w;
        rect.x = animation.frameOffsetX * rect.z;
      } else {
        rect.x = (animation.currentFrame + animation.frameOffsetX) * rect.z;
        rect.y = animation.frameOffsetY * rect.w;
      }
    }
  }

  const std::string &Animator2D::checkForNextAnimation() {
    static const std::string empty = "";

    if (!transitions.contains(currentAnimationName))
      return empty;

    for (const auto &[to, hasExitTime, exitTime, conditions]: transitions[currentAnimationName]) {
      if (!animations.contains(to))
        continue;

      // Wait for the animation to reach a transitionable state
      if (auto &anim = *currentAnimation;
        hasExitTime && anim.elapsedTime <= exitTime * anim.FrameCount() * anim.frameDuration)
        continue;

      bool allConditionsValid = true;
      for (const auto &condition: conditions) {
        if (!parameters.contains(condition.parameter)) {
          allConditionsValid = false;
          break;
        }
        Parameter &param = parameters[condition.parameter];
        bool result = false;
        switch (condition.condition) {
          case LESS_THAN:
            result = (param.type == INT && param.integer < static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating < condition.threshold);
            break;
          case LESS_OR_EQUAL:
            result = (param.type == INT && param.integer <= static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating <= condition.threshold);
            break;
          case GREATER_THAN:
            result = (param.type == INT && param.integer > static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating > condition.threshold);
            break;
          case GREATER_OR_EQUAL:
            result = (param.type == INT && param.integer >= static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating >= condition.threshold);
            break;
          case EQUAL:
            result = (param.type == INT && param.integer == static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating == condition.threshold);
            break;
          case NOT_EQUAL:
            result = (param.type == INT && param.integer != static_cast<int>(condition.threshold)) ||
                     (param.type == FLOAT && param.floating != condition.threshold);
            break;
          case IF_TRUE:
            result = param.boolean;
            break;
          case IF_FALSE:
            result = !param.boolean;
            break;
          case IF_TRIGGER:
            result = param.trigger;
            if (param.trigger)
              param.trigger = false;
            break;
        }

        if (!result) {
          allConditionsValid = false;
          break;
        }
      }

      if (allConditionsValid)
        return to;
    }

    return empty;
  }

  std::string Animator2D::parameterTypeToString(const ParameterType type) {
    switch (type) {
      case BOOL:
        return "BOOL";
      case INT:
        return "INT";
      case FLOAT:
        return "FLOAT";
      case TRIGGER:
        return "TRIGGER";
      default:
        return "UNKNOWN";
    }
  }
}
