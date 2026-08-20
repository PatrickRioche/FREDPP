#include "fred/flow/InputStack.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

InputStack::InputStack(std::size_t maximum_depth)
    : maximum_depth_(maximum_depth) {
    if (maximum_depth_ == 0) {
        throw std::invalid_argument("maximum input depth must be positive");
    }
}

void InputStack::push(std::unique_ptr<InputSource> source) {
    if (!source) {
        throw std::invalid_argument("input source must not be null");
    }
    if (sources_.size() >= maximum_depth_) {
        throw std::runtime_error("maximum buffer-flow depth exceeded");
    }

    // back() is the active source, giving injected/nested input normal stack
    // semantics: consume child first, then resume its parent.
    sources_.push_back(std::move(source));
}

std::optional<InputCharacter> InputStack::next() {
    while (!sources_.empty()) {
        if (auto character = sources_.back()->next()) {
            return character;
        }

        // Exhausted sources are destroyed immediately; the next loop iteration
        // resumes the previous caller/source.
        sources_.pop_back();
    }
    return std::nullopt;
}

bool InputStack::empty() const noexcept { return sources_.empty(); }
std::size_t InputStack::depth() const noexcept { return sources_.size(); }
std::size_t InputStack::maximum_depth() const noexcept {
    return maximum_depth_;
}

} // namespace fred
