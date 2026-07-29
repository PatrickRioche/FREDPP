#include "fred/flow/FlowEngine.hpp"
#include "fred/flow/BufferInputSource.hpp"

#include <memory>
#include <stdexcept>

namespace fred {

FlowEngine::FlowEngine(const BufferManager& buffers, std::size_t maximum_depth)
    : buffers_(&buffers), input_(maximum_depth) {}

std::string FlowEngine::expand_buffer(std::string_view buffer_name) {
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    push_buffer(buffer_name, 0);

    std::string output;
    while (auto character = read_raw()) {
        if (character->value != '\\') {
            output.push_back(character->value);
            continue;
        }

        auto directive = read_raw();
        if (!directive) {
            output.push_back('\\');
            break;
        }

        if (directive->level != character->level) {
            output.push_back('\\');
            output.push_back(directive->value);
            continue;
        }

        if (directive->value == '\\') {
            output.push_back('\\');
            continue;
        }

        if (directive->value == 'B' || directive->value == 'b') {
            const auto name = parse_buffer_name(character->level);
            push_buffer(name, character->level + 1);
            continue;
        }

        output.push_back('\\');
        output.push_back(directive->value);
    }

    return output;
}

std::optional<InputCharacter> FlowEngine::read_raw() {
    return input_.next();
}

std::string FlowEngine::parse_buffer_name(std::size_t directive_level) {
    const auto opening = read_raw();
    if (!opening || opening->level != directive_level || opening->value != '(') {
        throw std::runtime_error("expected '(' after \\B");
    }

    std::string name;
    while (auto character = read_raw()) {
        if (character->level != directive_level) {
            throw std::runtime_error("buffer name crossed an input level");
        }
        if (character->value == ')') {
            if (name.empty()) {
                throw std::runtime_error("empty buffer name in \\B");
            }
            return name;
        }
        if (character->value == '\n') {
            throw std::runtime_error("unterminated buffer name in \\B");
        }
        name.push_back(character->value);
    }

    throw std::runtime_error("unterminated buffer name in \\B");
}

void FlowEngine::push_buffer(std::string_view buffer_name, std::size_t level) {
    const auto& buffer = buffers_->get(buffer_name);
    input_.push(std::make_unique<BufferInputSource>(buffer, level));
}

} // namespace fred
