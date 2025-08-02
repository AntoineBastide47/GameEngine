//
// Parser.cpp
// Author: Antoine Bastide
// Date: 13/06/2025
//

// Clang AST and Tooling includes:
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/QualTypeNames.h>
#include <clang/AST/RecordLayout.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Type.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

// Add the macro definitions
#include "Parser.hpp"
#include "../../../Engine/include/Engine/Reflection/Reflectable.hpp"

using namespace clang;
using namespace Engine::Reflection;
namespace fs = std::filesystem;

// Recursive AST Visitor to find serializable classes and their fields.
class SerializableVisitor final : public RecursiveASTVisitor<SerializableVisitor> {
  public:
    explicit SerializableVisitor(std::vector<Record> &recordsOut)
      : records(recordsOut) {}

    static bool CanSerializeRecord(const CXXRecordDecl *decl, std::unordered_set<const CXXRecordDecl *> &visited) { // NOLINT(*-no-recursion)
      decl = decl->getDefinition();
      if (!decl || !decl->isCompleteDefinition() || !visited.insert(decl).second)
        return false;

      if (decl->getNameAsString().find("Reflectable") != std::string::npos)
        return true;

      for (const auto &B: decl->bases())
        if (const auto *BaseRD = B.getType()->getAsCXXRecordDecl())
          if (CanSerializeRecord(BaseRD->getDefinition(), visited))
            return true;

      return false;
    }

    // Visit each CXXRecordDecl (class/struct) in the AST:
    // ReSharper disable once CppDFAConstantFunctionResult
    bool VisitCXXRecordDecl(CXXRecordDecl *decl) const {
      // Skip forward declarations and anonymous classes/structs
      if (!decl->isThisDeclarationADefinition() || decl->isImplicit() || decl->getLocation().isInvalid() ||
          !decl->hasDefinition() || !decl->isCompleteDefinition() || !decl->getIdentifier() || decl->isUnion())
        return true;

      // Make sure we get the class from its source file
      SourceManager &SM = decl->getASTContext().getSourceManager();
      if (SM.isInSystemHeader(decl->getLocation()))
        return true;

      if (std::unordered_set<const CXXRecordDecl *> visited;
        !CanSerializeRecord(decl, visited) || decl->getNameAsString().find("Reflectable") != std::string::npos)
        return true;

      Record rec;
      rec.name = decl->getQualifiedNameAsString();
      rec.isClass = decl->isClass();

      if (PresumedLoc ploc = SM.getPresumedLoc(decl->getLocation()); ploc.isValid())
        rec.filePath = ploc.getFilename();
      else
        rec.filePath = SM.getFilename(decl->getLocation());

      if (const auto *tmpl = decl->getDescribedClassTemplate())
        for (const TemplateParameterList *params = tmpl->getTemplateParameters(); const NamedDecl *param: *params)
          rec.templateParameters.emplace_back(param->getNameAsString());

      // Extract class fields and inherited fields
      std::vector<const FieldDecl *> fields;
      collectAllFields(decl, fields);

      for (const auto &field: fields) {
        if (!field || !field->getDeclContext() || !field->getASTContext().getTranslationUnitDecl())
          continue;

        // Skip anonymous, mutable, volatile
        if (field->getName().empty() || field->isMutable() || field->getType().isVolatileQualified())
          continue;

        // Check custom annotation attributes on the field
        bool isSerializable = false;
        bool isNonSerializable = false;
        for (const Attr *attr: field->attrs()) {
          if (const auto *ann = dyn_cast<AnnotateAttr>(attr)) {
            if (const StringRef annotation = ann->getAnnotation(); annotation == _e_SERIALIZE_STRING)
              isSerializable = true;
            else if (annotation == _e_NON_SERIALIZABLE_STRING)
              isNonSerializable = true;
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

        // Skip public pointers not marked as serializable
        if ((field->getAccessUnsafe() != AS_public || isPointerLike) && !isSerializable)
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

    static void collectAllFields(const CXXRecordDecl *decl, std::vector<const FieldDecl *> &out) { // NOLINT(*-no-recursion)
      decl = decl->getDefinition();
      if (!decl || !decl->isCompleteDefinition())
        return;

      // Find all inherited fields first
      for (const auto &B: decl->bases())
        if (const auto *BaseRD = B.getType()->getAsCXXRecordDecl())
          collectAllFields(BaseRD->getDefinition(), out);

      // Extract each field
      for (const auto *FD: decl->fields())
        out.emplace_back(FD);
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

      // Skip raw pointers and pointers to members
      if (typePtr->isPointerType() || typePtr->isMemberPointerType())
        isNonSerializableType = true;
        // Template specialization types (e.g., containers or smart pointers)
      else if (const auto *specType = T->getAs<TemplateSpecializationType>()) {
        const TemplateName tmplName = specType->getTemplateName();
        if (const TemplateDecl *td = tmplName.getAsTemplateDecl()) {
          const std::string name = td->getQualifiedNameAsString();
          if (name == "std::function" || name == "std::weak_ptr") {
            isNonSerializableType = true;
            return;
          }
          if (name == "std::shared_ptr" || name == "std::unique_ptr")
            isPointerLike = true;
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
class SerializableAction final : public ASTFrontendAction, public tooling::FrontendActionFactory {
  public:
    explicit SerializableAction(std::vector<Record> &recordsOut)
      : records(recordsOut) {}

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef) override {
      CI.getDiagnostics().setClient(new IgnoringDiagConsumer(), true);
      return std::make_unique<SerializableConsumer>(records);
    }

    std::unique_ptr<FrontendAction> create() override {
      return std::make_unique<SerializableAction>(records);
    }
  private:
    std::vector<Record> &records;
};

namespace Engine::Reflection {
  void Parser::ParseHeader(const std::string &headerPath, std::vector<Record> &records) {
    // Read the header file into a string
    std::ifstream inFile(headerPath);
    if (!inFile.is_open()) {
      std::cerr << "Failed to open file: " << headerPath << std::endl;
      return;
    }
    const std::string code((std::istreambuf_iterator(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::vector<std::string> args{"-stdlib=libc++", "-std=c++20"};

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

    args.emplace_back("-Wno-unused-command-line-argument");

    tooling::FixedCompilationDatabase Compilations(".", args);
    tooling::ClangTool Tool(Compilations, {headerPath});
    SerializableAction factory(records);
    Tool.run(&factory);
  }

  void Parser::PrintRecords(const std::vector<Record> &records) {
    for (const auto &[isClass, name, filePath, fields, templateParameters]: records) {
      std::cout << "Record: " << (isClass ? "class " : "struct ") << name << " (defined in " << filePath << ")\n";
      if (!templateParameters.empty()) {
        std::cout << "  [TemplateParameters: " << templateParameters.size() << "]\n";
        for (const auto &templateParameter: templateParameters)
          std::cout << "    - " << templateParameter << "\n";
      }

      if (fields.empty())
        std::cout << "  [No serializable fields]\n";
      else {
        std::cout << "  [Fields: " << fields.size() << "]\n";
        for (const auto &[type, name]: fields) {
          std::cout << "    - " << type << " " << name << "\n";
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
} // namespace Engine::Reflection
