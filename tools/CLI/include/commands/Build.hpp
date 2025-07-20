//
// Build.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef BUILD_HPP
#define BUILD_HPP

#include "Command.hpp"

class Build final : public Command {
  public:
    Build();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;

    static int GetCoreCount();
    static bool HasNinja();
};

#endif //BUILD_HPP
