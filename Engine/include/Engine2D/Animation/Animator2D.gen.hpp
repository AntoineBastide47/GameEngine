// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_ANIMATOR2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Animation::Animator2D>("Engine2D::Animation::Animator2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(animations, format, json["animations"]);\
      Engine::Reflection::_e_saveImpl(parameters, format, json["parameters"]);\
      Engine::Reflection::_e_saveImpl(transitions, format, json["transitions"]);\
      Engine::Reflection::_e_saveImpl(currentAnimationName, format, json["currentAnimationName"]);\
      Engine::Reflection::_e_saveImpl(defaultAnimationName, format, json["defaultAnimationName"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(animations, format, json.At("animations"));\
      Engine::Reflection::_e_loadImpl(parameters, format, json.At("parameters"));\
      Engine::Reflection::_e_loadImpl(transitions, format, json.At("transitions"));\
      Engine::Reflection::_e_loadImpl(currentAnimationName, format, json.At("currentAnimationName"));\
      Engine::Reflection::_e_loadImpl(defaultAnimationName, format, json.At("defaultAnimationName"));\
    }\
  }\
  RENDER_ANIMATOR2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_ANIMATOR2D
#else
#define RENDER_ANIMATOR2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(animations, "Animations", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(parameters, "Parameters", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(transitions, "Transitions", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(currentAnimationName, "Current Animation Name", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(defaultAnimationName, "Default Animation Name", readOnly);\
    return changed;\
  }
#endif

  #define REFLECT_ANIMATIONCONDITION\
  static inline const bool _reg_ANIMATIONCONDITION = [] {\
    Engine::Reflection::ReflectionFactory::RegisterEnum<Engine2D::Animation::Animator2D::AnimationCondition>(\
    "Engine2D::Animation::Animator2D::AnimationCondition", {\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"LESS_THAN", Engine2D::Animation::Animator2D::AnimationCondition::LESS_THAN},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"LESS_OR_EQUAL", Engine2D::Animation::Animator2D::AnimationCondition::LESS_OR_EQUAL},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"GREATER_THAN", Engine2D::Animation::Animator2D::AnimationCondition::GREATER_THAN},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"GREATER_OR_EQUAL", Engine2D::Animation::Animator2D::AnimationCondition::GREATER_OR_EQUAL},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"EQUAL", Engine2D::Animation::Animator2D::AnimationCondition::EQUAL},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"NOT_EQUAL", Engine2D::Animation::Animator2D::AnimationCondition::NOT_EQUAL},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"IF_TRUE", Engine2D::Animation::Animator2D::AnimationCondition::IF_TRUE},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"IF_FALSE", Engine2D::Animation::Animator2D::AnimationCondition::IF_FALSE},\
      std::pair<std::string, Engine2D::Animation::Animator2D::AnimationCondition>{"IF_TRIGGER", Engine2D::Animation::Animator2D::AnimationCondition::IF_TRIGGER},\
      }\
    );\
    return true;\
  }();\


  #define REFLECT_PARAMETERTYPE\
  static inline const bool _reg_PARAMETERTYPE = [] {\
    Engine::Reflection::ReflectionFactory::RegisterEnum<Engine2D::Animation::Animator2D::ParameterType>(\
    "Engine2D::Animation::Animator2D::ParameterType", {\
      std::pair<std::string, Engine2D::Animation::Animator2D::ParameterType>{"BOOL", Engine2D::Animation::Animator2D::ParameterType::BOOL},\
      std::pair<std::string, Engine2D::Animation::Animator2D::ParameterType>{"INT", Engine2D::Animation::Animator2D::ParameterType::INT},\
      std::pair<std::string, Engine2D::Animation::Animator2D::ParameterType>{"FLOAT", Engine2D::Animation::Animator2D::ParameterType::FLOAT},\
      std::pair<std::string, Engine2D::Animation::Animator2D::ParameterType>{"TRIGGER", Engine2D::Animation::Animator2D::ParameterType::TRIGGER},\
      }\
    );\
    return true;\
  }();

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
      Engine::Reflection::_e_loadImpl(type, format, json.At("type"));\
    }\
  }\
  RENDER_PARAMETER

#if !ENGINE_EDITOR
#define RENDER_PARAMETER
#else
#define RENDER_PARAMETER\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(type, "Type", readOnly);\
    return changed;\
  }
#endif

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
      Engine::Reflection::_e_loadImpl(parameter, format, json.At("parameter"));\
      Engine::Reflection::_e_loadImpl(condition, format, json.At("condition"));\
      Engine::Reflection::_e_loadImpl(threshold, format, json.At("threshold"));\
    }\
  }\
  RENDER_TRANSITIONCONDITION

#if !ENGINE_EDITOR
#define RENDER_TRANSITIONCONDITION
#else
#define RENDER_TRANSITIONCONDITION\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(parameter, "Parameter", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(condition, "Condition", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(threshold, "Threshold", readOnly);\
    return changed;\
  }
#endif

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
      Engine::Reflection::_e_loadImpl(to, format, json.At("to"));\
      Engine::Reflection::_e_loadImpl(hasExitTime, format, json.At("hasExitTime"));\
      Engine::Reflection::_e_loadImpl(exitTime, format, json.At("exitTime"));\
      Engine::Reflection::_e_loadImpl(conditions, format, json.At("conditions"));\
    }\
  }\
  RENDER_ANIMATIONTRANSITION

#if !ENGINE_EDITOR
#define RENDER_ANIMATIONTRANSITION
#else
#define RENDER_ANIMATIONTRANSITION\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(to, "To", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(hasExitTime, "Has Exit Time", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(exitTime, "Exit Time", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(conditions, "Conditions", readOnly);\
    return changed;\
  }
#endif
