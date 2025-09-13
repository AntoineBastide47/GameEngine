// Auto-generated – DO NOT EDIT

#pragma once

#define SERIALIZE_CAMERA2D _e_SERIALIZE_RECORD \
  static inline const bool _reg = [] {\
    Engine::Reflection::ReflectionFactory::RegisterType<Engine2D::Rendering::Camera2D>("Engine2D::Rendering::Camera2D");\
    return true;\
  }();\
  void _e_save(const Engine::Reflection::Format format, Engine::JSON &json) const override { \
    if (format == Engine::Reflection::Format::JSON) { \
      json = Engine::JSON::Object();\
      Engine::Reflection::_e_saveImpl(active, format, json["active"]);\
      Engine::Reflection::_e_saveImpl(positionOffset, format, json["positionOffset"]);\
      Engine::Reflection::_e_saveImpl(rotationOffset, format, json["rotationOffset"]);\
      Engine::Reflection::_e_saveImpl(damping, format, json["damping"]);\
      Engine::Reflection::_e_saveImpl(zoomLevel, format, json["zoomLevel"]);\
      Engine::Reflection::_e_saveImpl(shakeCoefficientsX, format, json["shakeCoefficientsX"]);\
      Engine::Reflection::_e_saveImpl(shakeCoefficientsY, format, json["shakeCoefficientsY"]);\
      Engine::Reflection::_e_saveImpl(projection, format, json["projection"]);\
      Engine::Reflection::_e_saveImpl(shakeDuration, format, json["shakeDuration"]);\
      Engine::Reflection::_e_saveImpl(left, format, json["left"]);\
      Engine::Reflection::_e_saveImpl(right, format, json["right"]);\
      Engine::Reflection::_e_saveImpl(bottom, format, json["bottom"]);\
      Engine::Reflection::_e_saveImpl(top, format, json["top"]);\
    }\
  }\
  void _e_load(const Engine::Reflection::Format format, const Engine::JSON &json) override { \
    if (format == Engine::Reflection::Format::JSON) { \
      Engine::Reflection::_e_loadImpl(active, format, json.At("active"));\
      Engine::Reflection::_e_loadImpl(positionOffset, format, json.At("positionOffset"));\
      Engine::Reflection::_e_loadImpl(rotationOffset, format, json.At("rotationOffset"));\
      Engine::Reflection::_e_loadImpl(damping, format, json.At("damping"));\
      Engine::Reflection::_e_loadImpl(zoomLevel, format, json.At("zoomLevel"));\
      Engine::Reflection::_e_loadImpl(shakeCoefficientsX, format, json.At("shakeCoefficientsX"));\
      Engine::Reflection::_e_loadImpl(shakeCoefficientsY, format, json.At("shakeCoefficientsY"));\
      Engine::Reflection::_e_loadImpl(projection, format, json.At("projection"));\
      Engine::Reflection::_e_loadImpl(shakeDuration, format, json.At("shakeDuration"));\
      Engine::Reflection::_e_loadImpl(left, format, json.At("left"));\
      Engine::Reflection::_e_loadImpl(right, format, json.At("right"));\
      Engine::Reflection::_e_loadImpl(bottom, format, json.At("bottom"));\
      Engine::Reflection::_e_loadImpl(top, format, json.At("top"));\
    }\
  }\
  RENDER_CAMERA2D\
  private: 


#if !ENGINE_EDITOR
#define RENDER_CAMERA2D
#else
#define RENDER_CAMERA2D\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "Position Offset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(rotationOffset, "Rotation Offset", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(damping, "Damping", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(zoomLevel, "Zoom Level", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeCoefficientsX, "Shake Coefficients X", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeCoefficientsY, "Shake Coefficients Y", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeDuration, "Shake Duration", readOnly);\
    return changed;\
  }
#endif
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
      Engine::Reflection::_e_loadImpl(amplitude, format, json.At("amplitude"));\
      Engine::Reflection::_e_loadImpl(frequency, format, json.At("frequency"));\
      Engine::Reflection::_e_loadImpl(phase, format, json.At("phase"));\
    }\
  }\
  RENDER_SHAKEWAVE

#if !ENGINE_EDITOR
#define RENDER_SHAKEWAVE
#else
#define RENDER_SHAKEWAVE\
  bool _e_renderInEditor(const bool readOnly) override {\
    bool changed = false;\
    changed |= Engine::Reflection::_e_renderInEditorImpl(amplitude, "Amplitude", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(frequency, "Frequency", readOnly);\
    changed |= Engine::Reflection::_e_renderInEditorImpl(phase, "Phase", readOnly);\
    return changed;\
  }
#endif
