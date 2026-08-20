#pragma once

#include "fred/flow/InputCharacter.hpp"

#include <optional>
#include <string_view>

namespace fred {

/**
 * @brief Abstract producer consumed by InputStack.
 *
 * An InputSource is a stateful, forward-only stream of InputCharacter values.
 * InputStack owns concrete sources and automatically removes one when next()
 * returns std::nullopt.
 */
class InputSource {
public:
    virtual ~InputSource() = default;

    /**
     * @brief Produces the next character from this source.
     *
     * @return Next InputCharacter, or std::nullopt when permanently exhausted.
     *
     * @note Implementations advance internal cursor state.
     */
    [[nodiscard]] virtual std::optional<InputCharacter> next() = 0;

    /**
     * @return Non-owning textual description of the source.
     *
     * The returned view must remain valid for the lifetime required by callers.
     */
    [[nodiscard]] virtual std::string_view
    description() const noexcept = 0;

    /** @return Flow/input nesting level associated with this source. */
    [[nodiscard]] virtual std::size_t level() const noexcept = 0;
};

} // namespace fred
