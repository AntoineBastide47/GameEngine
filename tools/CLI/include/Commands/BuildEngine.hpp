//
// Build.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef BUILD_ENGINE_HPP
#define BUILD_ENGINE_HPP

#include "Command.hpp"

class BuildEngine final : public Command {
  public:
    explicit BuildEngine();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //BUILD_ENGINE_HPP
