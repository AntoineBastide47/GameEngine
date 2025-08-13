//
// Help.hpp
// Author: Antoine Bastide
// Date: 27/03/2025
//

#ifndef HELP_HPP
#define HELP_HPP

#include "Command.hpp"

class Help final : public Command {
  public:
    Help();
    void Run(
      const std::vector<std::string> &args, const OrderedMap<std::string, std::unique_ptr<Command>> &commands
    ) override;
  private:
    static inline void displayCommandInfo(const std::string &name, const std::unique_ptr<Command> &command);
};

#endif //HELP_HPP
