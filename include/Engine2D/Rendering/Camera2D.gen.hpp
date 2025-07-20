// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_CAMERA2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::Camera2D>("Engine2D::Rendering::Camera2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(rotationOffset, format, json["rotationOffset"]);\
      Engine::Reflection::_e_saveImpl(damping, format, json["damping"]);\
      Engine::Reflection::_e_saveImpl(shakeCoefficientsX, format, json["shakeCoefficientsX"]);\
      Engine::Reflection::_e_saveImpl(shakeCoefficientsY, format, json["shakeCoefficientsY"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_loadImpl(rotationOffset, format, json["rotationOffset"]);\
      Engine::Reflection::_e_loadImpl(damping, format, json["damping"]);\
      Engine::Reflection::_e_loadImpl(shakeCoefficientsX, format, json["shakeCoefficientsX"]);\
      Engine::Reflection::_e_loadImpl(shakeCoefficientsY, format, json["shakeCoefficientsY"]);\
    }\
  }\
  private: 

  #define SERIALIZE_SHAKEWAVE _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::Camera2D::ShakeWave>("Engine2D::Rendering::Camera2D::ShakeWave");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(amplitude, format, json["amplitude"]);\
      Engine::Reflection::_e_saveImpl(frequency, format, json["frequency"]);\
      Engine::Reflection::_e_saveImpl(phase, format, json["phase"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(amplitude, format, json["amplitude"]);\
      Engine::Reflection::_e_loadImpl(frequency, format, json["frequency"]);\
      Engine::Reflection::_e_loadImpl(phase, format, json["phase"]);\
    }\
  }
} // namespace Engine::Reflection
