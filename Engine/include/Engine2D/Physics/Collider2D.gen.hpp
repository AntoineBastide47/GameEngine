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
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(type, format, json["type"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(type, format, json["type"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(elasticity, "Elasticity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "PositionOffset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isTrigger, "IsTrigger", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(autoCompute, "AutoCompute", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(position, "Position", readOnly);\
    return changed;\
  }\
  private: 

  #define SERIALIZE_AABB _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::Collider2D::AABB>("Engine2D::Physics::Collider2D::AABB");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(min, format, json["min"]);\
      Engine::Reflection::_e_saveImpl(max, format, json["max"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(min, format, json["min"]);\
      Engine::Reflection::_e_loadImpl(max, format, json["max"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(min, "Min", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(max, "Max", readOnly);\
    return changed;\
  }

  #define SERIALIZE_CIRCLECOLLIDER2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::CircleCollider2D>("Engine2D::Physics::CircleCollider2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(type, format, json["type"]);\
      Engine::Reflection::_e_saveImpl(radius, format, json["radius"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(type, format, json["type"]);\
      Engine::Reflection::_e_loadImpl(radius, format, json["radius"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(elasticity, "Elasticity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "PositionOffset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isTrigger, "IsTrigger", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(autoCompute, "AutoCompute", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(position, "Position", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(radius, "Radius", readOnly);\
    return changed;\
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
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(type, format, json["type"]);\
      Engine::Reflection::_e_saveImpl(size, format, json["size"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(type, format, json["type"]);\
      Engine::Reflection::_e_loadImpl(size, format, json["size"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(elasticity, "Elasticity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "PositionOffset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isTrigger, "IsTrigger", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(autoCompute, "AutoCompute", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(position, "Position", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(size, "Size", readOnly);\
    return changed;\
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
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_saveImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_saveImpl(position, format, json["position"]);\
      Engine::Reflection::_e_saveImpl(type, format, json["type"]);\
      Engine::Reflection::_e_saveImpl(vertices, format, json["vertices"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(elasticity, format, json["elasticity"]);\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json["isTrigger"]);\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json["autoCompute"]);\
      Engine::Reflection::_e_loadImpl(position, format, json["position"]);\
      Engine::Reflection::_e_loadImpl(type, format, json["type"]);\
      Engine::Reflection::_e_loadImpl(vertices, format, json["vertices"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(elasticity, "Elasticity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "PositionOffset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isTrigger, "IsTrigger", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(autoCompute, "AutoCompute", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(position, "Position", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(vertices, "Vertices", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
