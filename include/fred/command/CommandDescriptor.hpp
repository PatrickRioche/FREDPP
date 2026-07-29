#pragma once

#include "fred/ast/CommandNode.hpp"

#include <functional>
#include <memory>
#include <string>

namespace fred {

using CommandParseFunction = std::function<std::unique_ptr<CommandNode>(
    std::unique_ptr<AddressNode>, SourceLocation)>;

struct CommandDescriptor {
    char mnemonic{};
    std::string name;
    CommandParseFunction parse;
};

} // namespace fred
