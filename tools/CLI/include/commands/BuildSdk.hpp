//
// BuildSdk.hpp
// Author: Antoine Bastide
// Date: 01.08.2025
//

#ifndef BUILD_SDK_HPP
#define BUILD_SDK_HPP

#include "Command.hpp"

class BuildSDK final : public Command {
  public:
    BuildSDK();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
};

#endif //BUILD_SDK_HPP
