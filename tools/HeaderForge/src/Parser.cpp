//
// Parser.cpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

// Clang AST and Tooling includes:
#include <clang/AST/QualTypeNames.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Type.h>
#include <clang/Tooling/Tooling.h>
#include "clang/AST/DeclTemplate.h"

#include <filesystem>
#include <fstream>
#include <iostream>

// Add the macro definitions
#include "Parser.hpp"
#include "../../../include/Engine/Macros/Serialization.hpp"

using namespace clang;
using namespace Engine::Serialization;
namespace fs = std::filesystem;

// Recursive AST Visitor to find serializable classes and their fields.
class SerializableVisitor final : public RecursiveASTVisitor<SerializableVisitor> {
  public:
    explicit SerializableVisitor(std::vector<Record> &recordsOut)
      : records(recordsOut) {}

    // Visit each CXXRecordDecl (class/struct) in the AST:
    // ReSharper disable once CppDFAConstantFunctionResult
    bool VisitCXXRecordDecl(CXXRecordDecl *decl) const {
      // Skip forward declarations and anonymous classes/structs
      if (!decl->hasDefinition() || !decl->isCompleteDefinition() || !decl->getIdentifier() || decl->isUnion())
        return true;

      // Skip included headers
      SourceManager &SM = decl->getASTContext().getSourceManager();
      if (!SM.isInMainFile(decl->getLocation()))
        return true;

      // Check if the class has a friend declaration of Engine::Serialization::Serializer
      bool canSerializeRecord = false;
      for (auto *friendDecl: decl->friends()) {
        // FriendDecl can name a type or a specific decl (function, etc.)
        if (auto *friendType = friendDecl->getFriendType()) {
          QualType qt = friendType->getType();
          // Get the named decl for the friend type (if it's a record)
          if (const CXXRecordDecl *friendRec = qt->getAsCXXRecordDecl()) {
            if (std::string fName = friendRec->getQualifiedNameAsString();
              fName == "Engine::Serialization::Serializer") {
              canSerializeRecord = true;
              break;
            }
          } else {
            // If not resolved to a record decl, fallback to string comparison
            if (std::string fStr = qt.getAsString();
              fStr.find("Engine::Serialization::Serializer") != std::string::npos) {
              canSerializeRecord = true;
              break;
            }
          }
        } else if (NamedDecl *friendND = friendDecl->getFriendDecl()) {
          // Friend is a named declaration (could be function or class template, etc.)
          if (std::string fName = friendND->getQualifiedNameAsString(); fName == "Engine::Serialization::Serializer") {
            canSerializeRecord = true;
            break;
          }
        }
      }

      if (!canSerializeRecord)
        return true;

      Record rec;
      rec.name = decl->getQualifiedNameAsString();
      rec.filePath = SM.getFilename(decl->getLocation()).str();

      for (FieldDecl *field: decl->fields()) {
        // Skip anonymous, mutable, volatile
        if (field->getName().empty() || field->isMutable() || field->getType().isVolatileQualified()) {
          continue;
        }

        // Check custom annotation attributes on the field
        bool hasSerializeField = false;
        bool isNonSerializable = false;
        bool hasSerializePointer = false;
        for (const Attr *attr: field->attrs()) {
          if (const auto *ann = dyn_cast<AnnotateAttr>(attr)) {
            if (const StringRef annotation = ann->getAnnotation(); annotation == SERIALIZE_FIELD_STRING)
              hasSerializeField = true;
            else if (annotation == NON_SERIALIZABLE_FIELD)
              isNonSerializable = true;
            else if (annotation == SERIALIZE_POINTER_STRING)
              hasSerializePointer = true;
          }
        }

        // If annotated non-serializable, skip regardless of other factors
        if (isNonSerializable)
          continue;

        bool isNonSerializableType = false;
        bool isPointerLike = false;

        // Start recursive check on this field's type
        checkType(field->getType(), isNonSerializableType, isPointerLike);
        if (isNonSerializableType)
          continue;

        if (field->getAccessUnsafe() == AS_public) {
          // Skip public pointers not marked as serializable
          if (isPointerLike && !hasSerializePointer)
            continue;
        }
        // Skip non-public fields not marked as serializable, and same for pointers
        else if ((!isPointerLike && !hasSerializeField) || (isPointerLike && !hasSerializePointer))
          continue;

        // Use Clang to get fully qualified type name (including namespace and template parameters)
        Variable var;
        PrintingPolicy policy(field->getASTContext().getLangOpts());
        policy.SuppressScope = false;
        var.type = TypeName::getFullyQualifiedName(field->getType(), field->getASTContext(), policy);
        var.name = field->getNameAsString();
        rec.fields.push_back(std::move(var));
      }

      records.push_back(std::move(rec));
      return true;
    }

    static void checkType(const QualType T, bool &isNonSerializableType, bool &isPointerLike) { // NOLINT(*-no-recursion)
      if (T.isNull() || isNonSerializableType)
        return;

      const Type *typePtr = T.getTypePtr();
      if (!typePtr)
        return;

      if (typePtr->isReferenceType()) {
        isNonSerializableType = true;
        return;
      }

      // Raw pointer types (T*)
      if (typePtr->isPointerType()) {
        if (const QualType pointee = typePtr->getPointeeType(); !pointee.isNull()) {
          if (pointee->isFunctionProtoType() || pointee->isFunctionNoProtoType()) {
            isNonSerializableType = true;
            return;
          }
          // Pointer to data/object
          isPointerLike = true;
          checkType(pointee, isNonSerializableType, isPointerLike);
        }
      }
      // Pointer-to-member types (X T::*)
      else if (typePtr->isMemberPointerType()) {
        if (typePtr->isMemberFunctionPointerType()) {
          // Pointer to member function
          isNonSerializableType = true;
          return;
        }
        // Pointer to member data
        isPointerLike = true;
        if (auto *mpt = llvm::dyn_cast<MemberPointerType>(typePtr))
          checkType(mpt->getPointeeType(), isNonSerializableType, isPointerLike);
      }
      // Template specialization types (e.g., containers or smart pointers)
      else if (const auto *specType = T->getAs<TemplateSpecializationType>()) {
        const TemplateName tmplName = specType->getTemplateName();
        if (const TemplateDecl *td = tmplName.getAsTemplateDecl()) {
          const std::string name = td->getQualifiedNameAsString();
          if (name == "std::function") {
            isNonSerializableType = true;
            return;
          }
          if (name == "std::shared_ptr" || name == "std::unique_ptr" || name == "std::weak_ptr") {
            if (name == "std::weak_ptr") {
              isNonSerializableType = true;
              return;
            }
            isPointerLike = true;
          }
        }

        // Recursively check all template type arguments directly
        for (const TemplateArgument &arg: specType->template_arguments()) {
          if (isNonSerializableType)
            break;

          if (arg.getKind() == TemplateArgument::Type)
            checkType(arg.getAsType(), isNonSerializableType, isPointerLike);
        }
      }
      // Array types: check element type
      else if (auto *arrType = llvm::dyn_cast<ArrayType>(typePtr))
        checkType(arrType->getElementType(), isNonSerializableType, isPointerLike);
    }
  private:
    std::vector<Record> &records;
};

// ASTConsumer that uses the visitor to traverse the AST
class SerializableConsumer final : public ASTConsumer {
  public:
    explicit SerializableConsumer(std::vector<Record> &recordsOut)
      : visitor(recordsOut) {}

    void HandleTranslationUnit(ASTContext &context) override {
      // Traverse the entire translation unit AST
      visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
  private:
    SerializableVisitor visitor;
};

// FrontendAction to run our ASTConsumer
class SerializableAction final : public ASTFrontendAction {
  public:
    explicit SerializableAction(std::vector<Record> &recordsOut)
      : records(recordsOut) {}

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) override {
      return std::make_unique<SerializableConsumer>(records);
    }
  private:
    std::vector<Record> &records;
};

namespace Engine::Serialization {
  std::vector<Record> Parser::ParseHeader(const std::string &headerPath) {
    std::vector<Record> result;
    // Read the header file into a string
    std::ifstream inFile(headerPath);
    if (!inFile.is_open()) {
      std::cerr << "Failed to open file: " << headerPath << std::endl;
      return result;
    }
    const std::string code((std::istreambuf_iterator(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    // Prepare compiler arguments for parsing (C++20 standard)
    std::vector<std::string> args{"-std=c++20", "-stdlib=libc++"};

    #if __APPLE__
    args.emplace_back("-isysroot");
    args.emplace_back(getMacOSSDKPath());
    args.emplace_back("-I" + (getMacOSSDKPath() + "/usr/include"));
    args.emplace_back("-I" + (getMacOSSDKPath() + "/usr/include/c++/v1"));
    #endif

    // Clang resource dir
    args.emplace_back("-resource-dir");
    args.emplace_back(getClangResourceDir());
    const fs::path path(getClangResourceDir());
    args.emplace_back("-I" + (path.parent_path().parent_path().parent_path() / "include").string());
    args.emplace_back("-I" + (getClangResourceDir() + "include/c++/v1"));

    // Run the Clang tool on the code with our custom FrontendAction
    const bool success = tooling::runToolOnCodeWithArgs(
      std::make_unique<SerializableAction>(result), code, args, headerPath
    );
    if (!success) {
      std::cerr << "Clang parsing failed for: " << headerPath << std::endl;
    }
    return result;
  }

  void Parser::PrintRecords(const std::vector<Record> &records) {
    for (const auto &[name, filePath, fields]: records) {
      std::cout << "Record: " << name << " (defined in " << filePath << ")\n";
      if (fields.empty()) {
        std::cout << "  [No serializable fields]\n";
      } else {
        for (const auto &[type, name]: fields) {
          std::cout << "  - " << type << " " << name << "\n";
        }
      }
      std::cout << "\n";
    }
  }

  std::string Parser::getClangResourceDir() {
    std::array<char, 128> buffer{};
    static std::string result;
    if (!result.empty())
      return result;

    FILE *pipe = popen("clang -print-resource-dir", "r");
    if (!pipe)
      throw std::runtime_error("popen failed");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
      result += buffer.data();
    }
    if (const auto rc = pclose(pipe); rc != 0)
      throw std::runtime_error("clang command failed");
    if (result.ends_with("\n"))
      result = result.substr(0, result.size() - 1);
    return result;
  }

  std::string Parser::getMacOSSDKPath() {
    std::array<char, 128> buffer{};
    static std::string result;
    if (!result.empty())
      return result;

    FILE *pipe = popen("xcrun --sdk macosx --show-sdk-path", "r");
    if (!pipe)
      throw std::runtime_error("popen failed");
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
      result += buffer.data();
    }
    if (const auto rc = pclose(pipe); rc != 0)
      throw std::runtime_error("xcrun command failed");
    result.erase(std::ranges::remove(result, '\n').begin(), result.end());
    return result;
  }
} // namespace Engine::Serialization
