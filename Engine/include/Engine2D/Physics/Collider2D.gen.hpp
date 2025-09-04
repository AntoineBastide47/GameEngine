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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(elasticity, format, json.At("elasticity"));\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json.At("positionOffset"));\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json.At("isTrigger"));\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json.At("autoCompute"));\
      Engine::Reflection::_e_loadImpl(position, format, json.At("position"));\
      Engine::Reflection::_e_loadImpl(type, format, json.At("type"));\
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


  #define REFLECT_COLLIDERTYPE\
  static inline const bool _reg_COLLIDERTYPE = [] {\
    Engine::Reflection::ReflectionFactory::RegisterEnum<Engine2D::Physics::Collider2D::ColliderType>(\
    "Engine2D::Physics::Collider2D::ColliderType", {\
      std::pair<std::string, Engine2D::Physics::Collider2D::ColliderType>{"None", Engine2D::Physics::Collider2D::ColliderType::None},\
      std::pair<std::string, Engine2D::Physics::Collider2D::ColliderType>{"Circle", Engine2D::Physics::Collider2D::ColliderType::Circle},\
      std::pair<std::string, Engine2D::Physics::Collider2D::ColliderType>{"Rectangle", Engine2D::Physics::Collider2D::ColliderType::Rectangle},\
      std::pair<std::string, Engine2D::Physics::Collider2D::ColliderType>{"Polygon", Engine2D::Physics::Collider2D::ColliderType::Polygon},\
      }\
    );\
    return true;\
  }();

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
      Engine::Reflection::_e_loadImpl(min, format, json.At("min"));\
      Engine::Reflection::_e_loadImpl(max, format, json.At("max"));\
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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(elasticity, format, json.At("elasticity"));\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json.At("positionOffset"));\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json.At("isTrigger"));\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json.At("autoCompute"));\
      Engine::Reflection::_e_loadImpl(position, format, json.At("position"));\
      Engine::Reflection::_e_loadImpl(type, format, json.At("type"));\
      Engine::Reflection::_e_loadImpl(radius, format, json.At("radius"));\
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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(elasticity, format, json.At("elasticity"));\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json.At("positionOffset"));\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json.At("isTrigger"));\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json.At("autoCompute"));\
      Engine::Reflection::_e_loadImpl(position, format, json.At("position"));\
      Engine::Reflection::_e_loadImpl(type, format, json.At("type"));\
      Engine::Reflection::_e_loadImpl(size, format, json.At("size"));\
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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(elasticity, format, json.At("elasticity"));\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json.At("positionOffset"));\
      Engine::Reflection::_e_loadImpl(isTrigger, format, json.At("isTrigger"));\
      Engine::Reflection::_e_loadImpl(autoCompute, format, json.At("autoCompute"));\
      Engine::Reflection::_e_loadImpl(position, format, json.At("position"));\
      Engine::Reflection::_e_loadImpl(type, format, json.At("type"));\
      Engine::Reflection::_e_loadImpl(vertices, format, json.At("vertices"));\
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
