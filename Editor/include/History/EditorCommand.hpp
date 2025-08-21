//
// SceneCommand.hpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#ifndef SCENE_COMMAND_HPP
#define SCENE_COMMAND_HPP

#include <string>

namespace Editor::History {
  class EditorCommand {
    public:
      virtual ~EditorCommand() = default;
      virtual void Execute() = 0;
      virtual void Undo() = 0;
      virtual std::string Name() const = 0;
  };

  template<typename ExecuteFunc, typename UndoFunc> class LambdaCommand final : public EditorCommand {
    public:
      LambdaCommand(const std::string &name, ExecuteFunc executeFunc, UndoFunc undoFunc)
        : commandName(name), executeFunc(std::move(executeFunc)), undoFunc(std::move(undoFunc)) {}

      void Execute() override {
        executeFunc();
      }

      void Undo() override {
        undoFunc();
      }

      std::string Name() const override {
        return commandName;
      }
    private:
      std::string commandName;
      ExecuteFunc executeFunc;
      UndoFunc undoFunc;
  };

  // Helper function to create lambda commands with type deduction
  template<typename ExecuteFunc, typename UndoFunc>
  auto MakeLambdaCommand(const std::string &name, ExecuteFunc &&executeFunc, UndoFunc &&undoFunc) {
    return std::make_unique<LambdaCommand<std::decay_t<ExecuteFunc>, std::decay_t<UndoFunc>>>(
      name, std::forward<ExecuteFunc>(executeFunc), std::forward<UndoFunc>(undoFunc)
    );
  }
} // Editor::History

#endif //SCENE_COMMAND_HPP
