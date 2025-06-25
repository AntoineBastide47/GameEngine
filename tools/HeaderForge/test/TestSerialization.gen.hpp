// Auto-generated – DO NOT EDIT

#pragma once

namespace Engine::Reflection {
  #define SERIALIZE_BASE _e_SERIALIZE_RECORD \
  inline void _e_save(const Engine::Reflection::Format format, std::ostringstream& os, const bool prettyPrint, const int indent) const { \
    if (format == Engine::Reflection::Format::JSON) { \
      os.put('{'); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"base\":", 7); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->base, format, prettyPrint, 0, &os); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent); \
      os.put('}'); \
    } \
  }\
  private: 

  #define SERIALIZE_DERIVED _e_SERIALIZE_RECORD \
  inline void _e_save(const Engine::Reflection::Format format, std::ostringstream& os, const bool prettyPrint, const int indent) const { \
    if (format == Engine::Reflection::Format::JSON) { \
      os.put('{'); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"base\":", 7); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->base, format, prettyPrint, 0, &os); \
      os.put(','); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"derived\":", 10); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->derived, format, prettyPrint, 0, &os); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent); \
      os.put('}'); \
    } \
  }\
  private: 

  #define SERIALIZE_DERIVED2 _e_SERIALIZE_RECORD \
  inline void _e_save(const Engine::Reflection::Format format, std::ostringstream& os, const bool prettyPrint, const int indent) const { \
    if (format == Engine::Reflection::Format::JSON) { \
      os.put('{'); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"base\":", 7); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->base, format, prettyPrint, 0, &os); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent); \
      os.put('}'); \
    } \
  }\
  private: 

  #define SERIALIZE_DERIVED3 _e_SERIALIZE_RECORD \
  inline void _e_save(const Engine::Reflection::Format format, std::ostringstream& os, const bool prettyPrint, const int indent) const { \
    if (format == Engine::Reflection::Format::JSON) { \
      os.put('{'); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"base\":", 7); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->base, format, prettyPrint, 0, &os); \
      os.put(','); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"t\":", 4); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->t, format, prettyPrint, 0, &os); \
      os.put(','); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent + 1); \
      os.write("\"v\":", 4); \
      if (prettyPrint) os.put(' '); \
      Engine::Reflection::_e_saveImpl(this->v, format, prettyPrint, 0, &os); \
      if (prettyPrint) os.put('\n'); \
      Engine::Reflection::applyIndent(os, prettyPrint, indent); \
      os.put('}'); \
    } \
  }\
  private: 
} // namespace Engine::Reflection
