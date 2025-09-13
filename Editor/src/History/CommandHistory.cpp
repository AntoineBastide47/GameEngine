//
// CommandHistory.cpp
// Author: Antoine Bastide
// Date: 17.08.2025
//

#include "History/CommandHistory.hpp"
#include "History/EditorCommand.hpp"

namespace Editor::History {
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

  bool CommandHistory::CanUndo() {
    return currentIndex >= 0;
  }

  bool CommandHistory::CanRedo() {
    return currentIndex < static_cast<int>(commands.size()) - 1;
  }

  bool CommandHistory::HasAffectedScene() {
    return hasAffectedScene;
  }

  void CommandHistory::Clear() {
    commands.clear();
    currentIndex = -1;
  }

  // For debugging
  std::string CommandHistory::History() {
    std::string history;
    const int size = std::distance(commands.begin(), commands.end());
    for (int i = 0; i < size; ++i) {
      std::string entry = commands.at(i)->Name();
      if (i == currentIndex)
        entry += " <-- current";
      history += entry;

      if (i < size - 1)
        history += '\n';
    }
    return history;
  }

  std::string CommandHistory::UndoCmdName() {
    return commands.at(currentIndex)->Name();
  }

  std::string CommandHistory::RedoCmdName() {
    return commands.at(currentIndex + 1)->Name();
  }

  void CommandHistory::execute(std::unique_ptr<EditorCommand> command) {
    // Remove any commands after current index (branching history)
    if (currentIndex < static_cast<int>(commands.size()) - 1)
      commands.erase(commands.begin() + currentIndex + 1, commands.end());

    hasAffectedScene = hasAffectedScene || command->AffectsScene();
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
