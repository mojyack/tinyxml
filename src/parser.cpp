#include <stack>

#include "macros/unwrap.hpp"
#include "util/trim.hpp"
#include "xml.hpp"

namespace {
template <class T, class... Args>
static auto contains(const T value, const T key, const Args... args) -> bool {
    if(value == key) {
        return true;
    }
    if constexpr(sizeof...(Args) > 0) {
        return contains(value, args...);
    }
    return false;
};
} // namespace

namespace xml {
struct StringReader {
    size_t           cursor;
    std::string_view str;

    auto peek() const -> std::optional<char> {
        ensure(cursor < str.size());
        return str[cursor];
    }

    auto read() -> std::optional<char> {
        unwrap(c, peek());
        cursor += 1;
        return c;
    }

    auto is_eof() const -> bool {
        return cursor >= str.size();
    }

    template <class... Args>
    auto read_until(const Args... args) -> std::optional<std::string_view> {
        auto begin = cursor;
    loop:
        unwrap(c, read());
        if(contains(c, args...)) {
            cursor -= 1;
            return str.substr(begin, cursor - begin);
        }
        goto loop;
    }

    auto skip_while(const char key) -> void {
        while(peek() == key) {
            cursor += 1;
        }
    }
};

struct ParseElementNodeResult {
    Node node;
    bool leaf;
};

auto parse_element_node(StringReader& reader) -> std::optional<ParseElementNodeResult> {
    ensure(reader.read() == '<');
    auto node = Node();
    {
        auto begin = reader.cursor;
        while(true) {
            const auto c = reader.read();
            if(c == ' ' || c == '>') {
                node.name = reader.str.substr(begin, reader.cursor - begin - 1);
                if(c == '>') {
                    auto leaf = false;
                    if(node.name.back() == '/') {
                        node.name.pop_back();
                        leaf = true;
                    }
                    return ParseElementNodeResult{node, leaf};
                }
                break;
            }
        }
    }
    while(true) {
        reader.skip_while(' ');
        unwrap(key, reader.read_until('=', '>'));
        ensure(reader.read()); // skip '=' or '>'
        if(key == "" || key == "/") {
            return ParseElementNodeResult{node, key == "/"};
        }
        unwrap(dlm, reader.read());
        unwrap(value, reader.read_until(dlm));
        ensure(reader.read()); // skip dlm
        node.attrs.push_back({std::string(key), std::string(value)});
    }
}

auto parse(std::string_view str) -> std::optional<Node> {
    str = trim(str);
    ensure(!str.empty() && str[0] == '<');

    auto root       = Node();
    auto node_stack = std::stack<Node*>();
    auto reader     = StringReader{.str = str};

    node_stack.push(&root);

    while(!reader.is_eof()) {
        auto& parent = *node_stack.top();
        unwrap(next, reader.peek());
        if(next != '<') {
            unwrap(data, reader.read_until('<'));
            parent.data = data;
            continue;
        }

        unwrap(pnr, parse_element_node(reader));
        auto& [node, leaf] = pnr;

        if(node.name.front() == '/') {
            ensure(node_stack.size() >= 2);
            ensure(parent.name == node.name.substr(1), "open close mismatched");
            node_stack.pop();
            continue;
        }
        if(leaf) {
            parent.children.emplace_back(node);
            continue;
        }

        auto& new_node = parent.children.emplace_back(node);
        node_stack.push(&new_node);
    }

    ensure(node_stack.size() == 1, "incomplete");
    return root.children[0];
}
} // namespace xml
