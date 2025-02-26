#include "macros/unwrap.hpp"
#include "xml.hpp"

namespace xml {
auto dump_node(const Node& node, const std::string_view prefix, const bool print_empty_fields) -> void {
    const auto prefix2 = std::string(prefix) + "  ";
    const auto prefix4 = std::string(prefix) + "    ";

    std::println(R"({}.name = "{}")", prefix, node.name);
    std::println(R"({}.data = "{}")", prefix, node.data);
    if(!(node.attrs.empty() && !print_empty_fields)) {
        std::println("{}.attrs = {{", prefix);
        for(const auto& a : node.attrs) {
            std::println(R"({}{{"{}"="{}"}},)", prefix2, a.key, a.value);
        }
        std::println("{}}},", prefix);
    }
    if(!(node.children.empty() && !print_empty_fields)) {
        std::println("{}.children = {{", prefix);
        for(const auto& c : node.children) {
            std::println("{}Node{{", prefix);
            dump_node(c, prefix4);
            std::println("{}}},", prefix2);
        }
        std::println("{}}},", prefix);
    }
}

struct TestCase {
    std::string         str;
    std::optional<Node> expects;
};

static const auto test_cases = std::array{
    TestCase{
        R"(<node attr1='value1' attr2="value2"/>)",
        Node{
            .name  = "node",
            .attrs = {{"attr1", "value1"}, {"attr2", "value2"}},
        },
    },
    TestCase{
        R"(<node></node>)",
        Node{
            .name = "node",
        },
    },
    TestCase{
        R"(<node1><node11/><node12/><node13><node131>data</node131></node13></node1>)",
        Node{
            .name     = "node1",
            .children = {
                {
                    Node{.name = "node11"},
                    Node{.name = "node12"},
                    Node{
                        .name     = "node13",
                        .children = {
                            Node{
                                .name = "node131",
                                .data = "data",
                            },
                        },
                    },
                },
            },
        },
    },
    TestCase{
        R"(node></node>)",
        std::nullopt,
    },
    TestCase{
        R"(<node1><node2></node2></node1></node0>)",
        std::nullopt,
    },
    TestCase{
        R"(<node1><node2></node1></node2>)",
        std::nullopt,
    },
    TestCase{
        R"(<node>)",
        std::nullopt,
    },
    TestCase{
        R"(<node1><node2><node1/>)",
        std::nullopt,
    },
};
} // namespace xml

auto main() -> int {
    for(const auto& c : xml::test_cases) {
        std::println("{}", c.str);
        if(c.expects) {
            unwrap(node, xml::parse(c.str), "parse failed");
            const auto& expect = *c.expects;
            if(node != expect) {
                std::println("parse result did not match");
                std::println("expect: ");
                dump_node(expect);
                std::println("got: ");
                dump_node(node);
                continue;
            }
        } else {
            ensure(!xml::parse(c.str), "parse did not fail");
        }
        std::println("pass");
    }
    return 0;
}
