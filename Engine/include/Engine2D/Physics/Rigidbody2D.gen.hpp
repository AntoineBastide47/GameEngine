// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_RIGIDBODY2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Physics::Rigidbody2D>("Engine2D::Physics::Rigidbody2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(isKinematic, format, json["isKinematic"]);\
      Engine::Reflection::_e_saveImpl(affectedByGravity, format, json["affectedByGravity"]);\
      Engine::Reflection::_e_saveImpl(linearVelocity, format, json["linearVelocity"]);\
      Engine::Reflection::_e_saveImpl(angularVelocity, format, json["angularVelocity"]);\
      Engine::Reflection::_e_saveImpl(angularDamping, format, json["angularDamping"]);\
      Engine::Reflection::_e_saveImpl(staticFriction, format, json["staticFriction"]);\
      Engine::Reflection::_e_saveImpl(dynamicFriction, format, json["dynamicFriction"]);\
      Engine::Reflection::_e_saveImpl(mass, format, json["mass"]);\
      Engine::Reflection::_e_saveImpl(inertia, format, json["inertia"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(isKinematic, format, json["isKinematic"]);\
      Engine::Reflection::_e_loadImpl(affectedByGravity, format, json["affectedByGravity"]);\
      Engine::Reflection::_e_loadImpl(linearVelocity, format, json["linearVelocity"]);\
      Engine::Reflection::_e_loadImpl(angularVelocity, format, json["angularVelocity"]);\
      Engine::Reflection::_e_loadImpl(angularDamping, format, json["angularDamping"]);\
      Engine::Reflection::_e_loadImpl(staticFriction, format, json["staticFriction"]);\
      Engine::Reflection::_e_loadImpl(dynamicFriction, format, json["dynamicFriction"]);\
      Engine::Reflection::_e_loadImpl(mass, format, json["mass"]);\
      Engine::Reflection::_e_loadImpl(inertia, format, json["inertia"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isKinematic, "IsKinematic", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(affectedByGravity, "AffectedByGravity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(linearVelocity, "LinearVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularVelocity, "AngularVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularDamping, "AngularDamping", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(staticFriction, "StaticFriction", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(dynamicFriction, "DynamicFriction", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(mass, "Mass", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(inertia, "Inertia", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
