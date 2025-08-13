//
// BuildEditor.hpp
// Author: Antoine Bastide
// Date: 10.08.2025
//

#ifndef BUILD_EDITOR_HPP
#define BUILD_EDITOR_HPP
#include "Command.hpp"

class BuildEditor final : public Command {
  public:
    BuildEditor();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //BUILD_EDITOR_HPP
