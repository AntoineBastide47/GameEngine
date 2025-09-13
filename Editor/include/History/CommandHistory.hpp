//
// CommandHistory.hpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#ifndef COMMAND_HISTORY_HPP
#define COMMAND_HISTORY_HPP

#include <memory>
#include <vector>

#include "EditorCommand.hpp"

namespace Editor {
  class SceneHierarchy;
}

namespace Editor::History {
  class CommandHistory final {
    friend class Editor::SceneHierarchy;
    public:
      /// Creates a new editor command
      /// @note The created command will be registered and executed at the end of the frame
      static void Create(std::unique_ptr<EditorCommand> command) {
        pendingCommands.emplace_back(std::move(command));
      }
      /// Undoes the last executed command
      static void Undo();
      /// Redoes the last undone command
      static void Redo();
      /// @returns True if any registered command can be undone, False if not
      static bool CanUndo();
      /// @returns True if any registered command can be redone, False if not
      static bool CanRedo();
      /// Clears all the existing commands
      /// @warning Does not undo any command, just free's the memory
      static void Clear();
      /// @returns True if at least one command has modified the current scene, False if not
      static bool HasAffectedScene();

      /// @returns A log of each registered command
      static std::string History();
      /// @returns The name of the first command that can be undone
      static std::string UndoCmdName();
      /// @returns The name of the first command that can be redone
      static std::string RedoCmdName();
    private:
      /// The index of the current command
      inline static int currentIndex = -1;
      /// The maximum number of commands that can be stored in memory
      constexpr static size_t maxHistorySize = 1000;
      /// Whether all the current commands have changed the active scene
      inline static bool hasAffectedScene = false;
      /// All the commands stored in memory
      inline static std::vector<std::unique_ptr<EditorCommand>> commands;
      /// All the commands waiting to be stored in memory
      inline static std::vector<std::unique_ptr<EditorCommand>> pendingCommands;

      /// Execute's and register's all the pending commands
      static void executePendingCommands();
      /// Execute's and register's the given command
      static void execute(std::unique_ptr<EditorCommand> command);
  };
}

#endif //COMMAND_HISTORY_HPP
