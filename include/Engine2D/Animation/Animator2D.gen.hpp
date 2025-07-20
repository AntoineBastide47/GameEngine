// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_ANIMATOR2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animator2D>("Engine2D::Animation::Animator2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) {}\
  }\
  private: 

  #define SERIALIZE_PARAMETER _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animator2D::Parameter>("Engine2D::Animation::Animator2D::Parameter");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(type, format, json["type"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(type, format, json["type"]);\
    }\
  }

  #define SERIALIZE_TRANSITIONCONDITION _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animator2D::TransitionCondition>("Engine2D::Animation::Animator2D::TransitionCondition");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(parameter, format, json["parameter"]);\
      Engine::Reflection::_e_saveImpl(condition, format, json["condition"]);\
      Engine::Reflection::_e_saveImpl(threshold, format, json["threshold"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(parameter, format, json["parameter"]);\
      Engine::Reflection::_e_loadImpl(condition, format, json["condition"]);\
      Engine::Reflection::_e_loadImpl(threshold, format, json["threshold"]);\
    }\
  }

  #define SERIALIZE_ANIMATIONTRANSITION _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animator2D::AnimationTransition>("Engine2D::Animation::Animator2D::AnimationTransition");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(to, format, json["to"]);\
      Engine::Reflection::_e_saveImpl(hasExitTime, format, json["hasExitTime"]);\
      Engine::Reflection::_e_saveImpl(exitTime, format, json["exitTime"]);\
      Engine::Reflection::_e_saveImpl(conditions, format, json["conditions"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(to, format, json["to"]);\
      Engine::Reflection::_e_loadImpl(hasExitTime, format, json["hasExitTime"]);\
      Engine::Reflection::_e_loadImpl(exitTime, format, json["exitTime"]);\
      Engine::Reflection::_e_loadImpl(conditions, format, json["conditions"]);\
    }\
  }
} // namespace Engine::Reflection
