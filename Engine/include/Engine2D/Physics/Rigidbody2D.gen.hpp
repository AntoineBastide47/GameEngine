// Auto-generated – DO NOT EDIT

#pragma once

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
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(isKinematic, format, json.At("isKinematic"));\
      Engine::Reflection::_e_loadImpl(affectedByGravity, format, json.At("affectedByGravity"));\
      Engine::Reflection::_e_loadImpl(linearVelocity, format, json.At("linearVelocity"));\
      Engine::Reflection::_e_loadImpl(angularVelocity, format, json.At("angularVelocity"));\
      Engine::Reflection::_e_loadImpl(angularDamping, format, json.At("angularDamping"));\
      Engine::Reflection::_e_loadImpl(staticFriction, format, json.At("staticFriction"));\
      Engine::Reflection::_e_loadImpl(dynamicFriction, format, json.At("dynamicFriction"));\
      Engine::Reflection::_e_loadImpl(mass, format, json.At("mass"));\
      Engine::Reflection::_e_loadImpl(inertia, format, json.At("inertia"));\
    }\
  }\
  RENDER_RIGIDBODY2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_RIGIDBODY2D
#else
#define RENDER_RIGIDBODY2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(isKinematic, "Is Kinematic", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(affectedByGravity, "Affected By Gravity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(linearVelocity, "Linear Velocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularVelocity, "Angular Velocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularDamping, "Angular Damping", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(staticFriction, "Static Friction", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(dynamicFriction, "Dynamic Friction", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(mass, "Mass", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(inertia, "Inertia", readOnly);\
    return changed;\
  }
#endif