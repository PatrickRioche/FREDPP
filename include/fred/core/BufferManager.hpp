#pragma once

#include "fred/core/Buffer.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fred {

class BufferManager {
public:
    BufferManager();

    Buffer& create_or_select(std::string name);
    Buffer& get_or_create(std::string name);
    Buffer& select(std::string_view name);
    const Buffer& get(std::string_view name) const;

    [[nodiscard]] bool contains(std::string_view name) const noexcept;
    void erase(std::string_view name);

    [[nodiscard]] Buffer& current();
    [[nodiscard]] const Buffer& current() const;
    [[nodiscard]] std::vector<std::string> names() const;
    [[nodiscard]] std::vector<std::string> recent_names() const;
    [[nodiscard]] std::vector<std::string> modified_names() const;

private:
    using Storage = std::unordered_map<std::string, std::unique_ptr<Buffer>>;

    void touch(std::string_view name);

    Storage buffers_;
    std::vector<std::string> usage_order_;
    Buffer* current_{nullptr};
};

} // namespace fred
