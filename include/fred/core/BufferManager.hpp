#pragma once

#include "fred/core/Buffer.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fred {

/**
 * @brief Owns, selects and indexes all live FRED buffers.
 *
 * BufferManager owns Buffer objects through std::unique_ptr, tracks one current
 * Buffer and maintains a most-recently-selected name list.
 *
 * @par Bootstrap invariant
 * Construction creates and selects buffer `"0"`, so a newly constructed
 * manager has a current buffer.
 *
 * @par Name validation
 * Creation APIs reject empty names and names longer than
 * limits::max_buffer_name_length (currently 64).
 *
 * @par Transient-buffer cleanup
 * create_or_select() implements FR-0006: when switching away from a non-`0`
 * buffer that is empty, clean and has no associated file, that previous buffer
 * is automatically removed.
 *
 * @par Reference lifetime
 * References to Buffer objects remain valid while that specific Buffer remains
 * owned by the manager. erase(), or the transient cleanup performed by
 * create_or_select(), invalidates references/pointers to the removed Buffer.
 *
 * @par Threading
 * BufferManager provides no internal synchronization.
 */
class BufferManager {
public:
    /**
     * @brief Constructs a manager containing selected buffer `"0"`.
     */
    BufferManager();

    /**
     * @brief Selects an existing buffer or creates and selects it.
     *
     * @param name Buffer name.
     * @return Reference to the selected Buffer.
     * @throws std::invalid_argument for an empty/over-limit name.
     *
     * @post The returned buffer becomes current and most-recently used.
     * @post A previously selected transient empty buffer may be erased.
     */
    Buffer& create_or_select(std::string name);

    /**
     * @brief Returns an existing buffer or creates it without selecting it.
     *
     * @param name Buffer name.
     * @return Reference to the owned Buffer.
     * @throws std::invalid_argument for an empty/over-limit name.
     *
     * @note This does not change current() or the recent-selection order.
     */
    Buffer& get_or_create(std::string name);

    /**
     * @brief Selects an already existing buffer.
     *
     * @param name Existing buffer name.
     * @return Reference to the selected Buffer.
     * @throws std::out_of_range when the name is unknown.
     *
     * @post The selected name moves to the front of recent_names().
     */
    Buffer& select(std::string_view name);

    /**
     * @brief Looks up an existing buffer without selecting it.
     *
     * @param name Existing buffer name.
     * @return Const reference to the Buffer.
     * @throws std::out_of_range with the unknown name when absent.
     */
    const Buffer& get(std::string_view name) const;

    /**
     * @brief Tests whether a name exists.
     *
     * @return true when the manager owns a Buffer under this exact name.
     */
    [[nodiscard]] bool contains(std::string_view name) const noexcept;

    /**
     * @brief Removes an owned buffer.
     *
     * @param name Existing buffer name.
     * @throws std::out_of_range when unknown.
     *
     * @post The erased name is removed from recent_names().
     * @post If this would leave no buffers, buffer `"0"` is recreated/selected.
     * @post If the current buffer is erased while others remain, an arbitrary
     *       remaining unordered-map entry becomes current.
     *
     * @warning References/pointers to the erased Buffer become invalid.
     */
    void erase(std::string_view name);

    /**
     * @return Mutable reference to the current Buffer.
     * @throws std::logic_error only if the internal current-buffer invariant has
     *         been broken.
     */
    [[nodiscard]] Buffer& current();

    /**
     * @return Const reference to the current Buffer.
     * @throws std::logic_error only if the internal current-buffer invariant has
     *         been broken.
     */
    [[nodiscard]] const Buffer& current() const;

    /**
     * @return All owned buffer names sorted lexicographically.
     */
    [[nodiscard]] std::vector<std::string> names() const;

    /**
     * @return Copy of the selection history, most-recent name first.
     *
     * get_or_create() alone does not add/touch a name in this list.
     */
    [[nodiscard]] std::vector<std::string> recent_names() const;

    /**
     * @return Names of all modified buffers sorted lexicographically.
     */
    [[nodiscard]] std::vector<std::string> modified_names() const;

private:
    using Storage =
        std::unordered_map<std::string, std::unique_ptr<Buffer>>;

    /** @brief Moves `name` to the front of the MRU selection list. */
    void touch(std::string_view name);

    Storage buffers_;
    std::vector<std::string> usage_order_;

    /** Non-owning pointer into buffers_; manager owns the pointed Buffer. */
    Buffer* current_{nullptr};
};

} // namespace fred
