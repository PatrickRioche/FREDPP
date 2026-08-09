#include "fred/flow/FlowEngine.hpp"
#include "fred/flow/BufferInputSource.hpp"
#include "fred/flow/InputSource.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace fred {
namespace {

class StringInputSource final : public InputSource {
public:
    explicit StringInputSource(std::string_view source, std::size_t level)
        : source_(source), level_(level) {}

    [[nodiscard]] std::optional<InputCharacter> next() override {
        if (position_ >= source_.size()) {
            return std::nullopt;
        }
        return InputCharacter{source_[position_++], level_, false};
    }

    [[nodiscard]] std::string_view description() const noexcept override {
        return "string-input";
    }

    [[nodiscard]] std::size_t level() const noexcept override {
        return level_;
    }

private:
    std::string source_;
    std::size_t level_;
    std::size_t position_{0};
};

} // namespace

FlowEngine::FlowEngine(const BufferManager& buffers, std::size_t maximum_depth)
    : buffers_(&buffers), input_(maximum_depth) {}

std::string FlowEngine::expand_buffer(std::string_view buffer_name) {
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    push_buffer(buffer_name, 0);
    return expand_current_input();
}

std::string FlowEngine::expand_input(std::string_view source) {
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    input_.push(std::make_unique<StringInputSource>(source, 0));
    return expand_current_input();
}

std::string FlowEngine::expand_current_input() {
    std::string output;
    while (auto character = read_raw()) {
        if (character->literal || character->value != '\\') {
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
            const auto name = parse_buffer_name(character->level, 'B');
            push_buffer(name, character->level + 1);
            continue;
        }

        if (directive->value == 'S' || directive->value == 's') {
            const auto name = parse_buffer_name(character->level, 'S');
            push_buffer(name, character->level + 1, false, true);
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

std::string FlowEngine::parse_buffer_name(std::size_t directive_level,
                                          char directive) {
    const std::string prefix = std::string("\\") + directive;
    const auto opening = read_raw();
    if (!opening || opening->level != directive_level || opening->value != '(') {
        throw std::runtime_error("expected '(' after " + prefix);
    }

    std::string name;
    while (auto character = read_raw()) {
        if (character->level != directive_level) {
            throw std::runtime_error("buffer name crossed an input level");
        }
        if (character->value == ')') {
            if (name.empty()) {
                throw std::runtime_error("empty buffer name in " + prefix);
            }
            return name;
        }
        if (character->value == '\n') {
            throw std::runtime_error(
                "unterminated buffer name in " + prefix);
        }
        name.push_back(character->value);
    }

    throw std::runtime_error("unterminated buffer name in " + prefix);
}

void FlowEngine::push_buffer(std::string_view buffer_name,
                             std::size_t level,
                             bool emit_newlines,
                             bool literal) {
    const auto& buffer = buffers_->get(buffer_name);
    input_.push(std::make_unique<BufferInputSource>(
        buffer, level, emit_newlines, literal));
}

} // namespace fred
