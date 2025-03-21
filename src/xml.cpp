#include <print>
#include <ranges>
#include <span>

#include "xml.hpp"

namespace xml {
auto Node::get_attrs(AttributeQuery* const queries, const size_t len) const -> bool {
    auto found = 0uz;
    for(auto& q : std::span{queries, len}) {
        for(const auto& a : attrs) {
            if(a.key == q.key) {
                q.value = a.value;
                found += 1;
                break;
            }
        }
    }
    return found == len;
}

auto Node::find_attr(const std::string_view attr) -> std::string* {
    for(auto& a : attrs) {
        if(a.key == attr) {
            return &a.value;
        }
    }
    return nullptr;
}

auto Node::find_attr(const std::string_view attr) const -> const std::string* {
    return const_cast<Node*>(this)->find_attr(attr);
}

auto Node::is_attr_equal(std::string_view key, std::string_view value) const -> bool {
    const auto attr = find_attr(key);
    return attr && (*attr) == value;
}

auto Node::operator==(const Node& o) const -> bool {
    if(name != o.name || data != o.data || attrs.size() != o.attrs.size() || children.size() != o.children.size()) {
        constexpr auto print_mismatch_reason = false;
        if(print_mismatch_reason) {
            std::println("name {} {}", name, o.name);
            std::println("data {} {}", data, o.data);
            std::println("attr.size {} {}", attrs.size(), o.attrs.size());
            std::println("children.size {} {}", children.size(), o.children.size());
        }
        return false;
    }
    for(auto&& [a, b] : std::views::zip(attrs, o.attrs)) {
        if(a.key != b.key || a.value != b.value) {
            return false;
        }
    }
    for(auto&& [a, b] : std::views::zip(children, o.children)) {
        if(a != b) {
            return false;
        }
    }
    return true;
}
} // namespace xml
