//
// CommandHistory.cpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#include "History/CommandHistory.hpp"
#include "History/EditorCommand.hpp"

namespace Editor::History {
  CommandHistory::CommandHistory()
    : currentIndex(-1), lastSavedIndex(-1), maxHistorySize(100000) {}

  void CommandHistory::Create(std::unique_ptr<EditorCommand> command) {
    pendingCommands.emplace_back(std::move(command));
  }

  void CommandHistory::Undo() {
    if (CanUndo()) {
      commands.at(currentIndex)->Undo();
      currentIndex--;
    }
  }

  void CommandHistory::Redo() {
    if (CanRedo()) {
      currentIndex++;
      commands.at(currentIndex)->Execute();
    }
  }

  bool CommandHistory::CanUndo() const {
    return currentIndex >= 0;
  }

  bool CommandHistory::CanRedo() const {
    return currentIndex < static_cast<int>(commands.size()) - 1;
  }

  bool CommandHistory::HasUnsavedChanges() const {
    return lastSavedIndex != currentIndex;
  }

  void CommandHistory::MarkSaved() {
    lastSavedIndex = currentIndex;
  }

  void CommandHistory::Clear() {
    commands.clear();
    currentIndex = -1;
    lastSavedIndex = -1;
  }

  // For debugging
  std::string CommandHistory::History() const {
    std::string history;
    for (size_t i = 0; i < commands.size(); ++i) {
      std::string entry = commands.at(i)->Name();
      if (i == currentIndex)
        entry += " <-- current";
      if (i == lastSavedIndex)
        entry += " [saved]";
      history += entry;

      if (i < commands.size() - 1)
        history += '\n';
    }
    return history;
  }

  std::string CommandHistory::UndoCmdName() const {
    return commands.at(currentIndex)->Name();
  }

  std::string CommandHistory::RedoCmdName() const {
    return commands.at(currentIndex + 1)->Name();
  }

  void CommandHistory::execute(std::unique_ptr<EditorCommand> command) {
    // Remove any commands after current index (branching history)
    if (currentIndex < static_cast<int>(commands.size()) - 1) {
      commands.erase(commands.begin() + currentIndex + 1, commands.end());
    }

    command->Execute();
    commands.push_back(std::move(command));
    currentIndex++;

    // Limit history size to prevent memory issues
    if (commands.size() > maxHistorySize) {
      commands.erase(commands.begin());
      currentIndex--;
    }
  }

  void CommandHistory::executePendingCommands() {
    for (auto &command: pendingCommands)
      execute(std::move(command));
    pendingCommands.clear();
  }
}
