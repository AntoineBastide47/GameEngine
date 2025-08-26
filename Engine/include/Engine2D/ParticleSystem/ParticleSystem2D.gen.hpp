// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_PARTICLESYSTEM2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::ParticleSystem2D>("Engine2D::ParticleSystem2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(renderOrder, format, json["renderOrder"]);\
      Engine::Reflection::_e_saveImpl(renderType, format, json["renderType"]);\
      Engine::Reflection::_e_saveImpl(loop, format, json["loop"]);\
      Engine::Reflection::_e_saveImpl(restart, format, json["restart"]);\
      Engine::Reflection::_e_saveImpl(useGlobalVelocities, format, json["useGlobalVelocities"]);\
      Engine::Reflection::_e_saveImpl(simulateInWorldSpace, format, json["simulateInWorldSpace"]);\
      Engine::Reflection::_e_saveImpl(startDelay, format, json["startDelay"]);\
      Engine::Reflection::_e_saveImpl(startPosition, format, json["startPosition"]);\
      Engine::Reflection::_e_saveImpl(flip, format, json["flip"]);\
      Engine::Reflection::_e_saveImpl(startVelocity, format, json["startVelocity"]);\
      Engine::Reflection::_e_saveImpl(endVelocity, format, json["endVelocity"]);\
      Engine::Reflection::_e_saveImpl(startAngularVelocity, format, json["startAngularVelocity"]);\
      Engine::Reflection::_e_saveImpl(endAngularVelocity, format, json["endAngularVelocity"]);\
      Engine::Reflection::_e_saveImpl(startScale, format, json["startScale"]);\
      Engine::Reflection::_e_saveImpl(endScale, format, json["endScale"]);\
      Engine::Reflection::_e_saveImpl(startColor, format, json["startColor"]);\
      Engine::Reflection::_e_saveImpl(endColor, format, json["endColor"]);\
      Engine::Reflection::_e_saveImpl(simulationSpeed, format, json["simulationSpeed"]);\
      Engine::Reflection::_e_saveImpl(emissionRate, format, json["emissionRate"]);\
      Engine::Reflection::_e_saveImpl(maxStartPositionOffset, format, json["maxStartPositionOffset"]);\
      Engine::Reflection::_e_saveImpl(blendMode, format, json["blendMode"]);\
      Engine::Reflection::_e_saveImpl(duration, format, json["duration"]);\
      Engine::Reflection::_e_saveImpl(particleLifetime, format, json["particleLifetime"]);\
      Engine::Reflection::_e_saveImpl(maxParticles, format, json["maxParticles"]);\
      Engine::Reflection::_e_saveImpl(simulationFinished, format, json["simulationFinished"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json["active"]);\
      Engine::Reflection::_e_loadImpl(renderOrder, format, json["renderOrder"]);\
      Engine::Reflection::_e_loadImpl(renderType, format, json["renderType"]);\
      Engine::Reflection::_e_loadImpl(loop, format, json["loop"]);\
      Engine::Reflection::_e_loadImpl(restart, format, json["restart"]);\
      Engine::Reflection::_e_loadImpl(useGlobalVelocities, format, json["useGlobalVelocities"]);\
      Engine::Reflection::_e_loadImpl(simulateInWorldSpace, format, json["simulateInWorldSpace"]);\
      Engine::Reflection::_e_loadImpl(startDelay, format, json["startDelay"]);\
      Engine::Reflection::_e_loadImpl(startPosition, format, json["startPosition"]);\
      Engine::Reflection::_e_loadImpl(flip, format, json["flip"]);\
      Engine::Reflection::_e_loadImpl(startVelocity, format, json["startVelocity"]);\
      Engine::Reflection::_e_loadImpl(endVelocity, format, json["endVelocity"]);\
      Engine::Reflection::_e_loadImpl(startAngularVelocity, format, json["startAngularVelocity"]);\
      Engine::Reflection::_e_loadImpl(endAngularVelocity, format, json["endAngularVelocity"]);\
      Engine::Reflection::_e_loadImpl(startScale, format, json["startScale"]);\
      Engine::Reflection::_e_loadImpl(endScale, format, json["endScale"]);\
      Engine::Reflection::_e_loadImpl(startColor, format, json["startColor"]);\
      Engine::Reflection::_e_loadImpl(endColor, format, json["endColor"]);\
      Engine::Reflection::_e_loadImpl(simulationSpeed, format, json["simulationSpeed"]);\
      Engine::Reflection::_e_loadImpl(emissionRate, format, json["emissionRate"]);\
      Engine::Reflection::_e_loadImpl(maxStartPositionOffset, format, json["maxStartPositionOffset"]);\
      Engine::Reflection::_e_loadImpl(blendMode, format, json["blendMode"]);\
      Engine::Reflection::_e_loadImpl(duration, format, json["duration"]);\
      Engine::Reflection::_e_loadImpl(particleLifetime, format, json["particleLifetime"]);\
      Engine::Reflection::_e_loadImpl(maxParticles, format, json["maxParticles"]);\
      Engine::Reflection::_e_loadImpl(simulationFinished, format, json["simulationFinished"]);\
    }\
  }\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderOrder, "RenderOrder", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(renderType, "RenderType", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(loop, "Loop", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(restart, "Restart", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(useGlobalVelocities, "UseGlobalVelocities", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(simulateInWorldSpace, "SimulateInWorldSpace", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startDelay, "StartDelay", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startPosition, "StartPosition", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(flip, "Flip", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startVelocity, "StartVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(endVelocity, "EndVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startAngularVelocity, "StartAngularVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(endAngularVelocity, "EndAngularVelocity", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startScale, "StartScale", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(endScale, "EndScale", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(startColor, "StartColor", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(endColor, "EndColor", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(simulationSpeed, "SimulationSpeed", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(emissionRate, "EmissionRate", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(maxStartPositionOffset, "MaxStartPositionOffset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(blendMode, "BlendMode", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(duration, "Duration", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(particleLifetime, "ParticleLifetime", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(maxParticles, "MaxParticles", readOnly);\
    return changed;\
  }\
  private: 
} // namespace Engine::Reflection
