// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_COLLIDER2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::Collider2D>("Engine2D::Physics::Collider2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
    }\
  }\
  private: 

  #define SERIALIZE_CIRCLECOLLIDER2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::CircleCollider2D>("Engine2D::Physics::CircleCollider2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(radius, format, json["radius"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(radius, format, json["radius"]);\
    }\
  }\
  private: 

  #define SERIALIZE_BOXCOLLIDER2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::BoxCollider2D>("Engine2D::Physics::BoxCollider2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(width, format, json["width"]);\
      Engine::Reflection::_e_saveImpl(height, format, json["height"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(width, format, json["width"]);\
      Engine::Reflection::_e_loadImpl(height, format, json["height"]);\
    }\
  }\
  private: 

  #define SERIALIZE_POLYGONCOLLIDER2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::PolygonCollider2D>("Engine2D::Physics::PolygonCollider2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(offset, format, json["offset"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
    }\
  }\
  private: 
} // namespace Engine::Reflection
