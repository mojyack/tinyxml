#include <format>

#include "xml.hpp"

namespace xml {
namespace {
auto deparse(std::string& str, const Node& node) -> void {
    str += std::format("<{}", node.name);
    for(const auto& a : node.attrs) {
        str += std::format(R"( {}="{}")", a.key, a.value);
    }

    const auto leaf = node.data.empty() && node.children.empty();
    if(leaf) {
        str += "/>";
        return;
    }
    str += ">";
    if(!node.data.empty()) {
        str += node.data;
    }
    for(const auto& c : node.children) {
        deparse(str, c);
    }
    str += std::format("</{}>", node.name);
}
} // namespace

auto deparse(const Node& node) -> std::string {
    auto str = std::string();
    deparse(str, node);
    return str;
}
} // namespace xml
