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
      CommandHistory();

      void Create(std::unique_ptr<EditorCommand> command);
      void Undo();
      void Redo();
      [[nodiscard]] bool CanUndo() const;
      [[nodiscard]] bool CanRedo() const;
      bool HasUnsavedChanges() const;
      void MarkSaved();
      void Clear();
      std::string History() const;

      std::string UndoCmdName() const;
      std::string RedoCmdName() const;
    private:
      std::vector<std::unique_ptr<EditorCommand>> commands;
      std::vector<std::unique_ptr<EditorCommand>> pendingCommands;
      int currentIndex;
      int lastSavedIndex;
      size_t maxHistorySize;

      void execute(std::unique_ptr<EditorCommand> command);
      void executePendingCommands();
  };
}

#endif //COMMAND_HISTORY_HPP
