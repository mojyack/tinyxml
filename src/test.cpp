#include "macros/unwrap.hpp"
#include "util/print.hpp"
#include "xml.hpp"

namespace xml {
auto dump_node(const Node& node, const std::string_view prefix, const bool print_empty_fields) -> void {
    const auto prefix2 = std::string(prefix) + "  ";
    const auto prefix4 = std::string(prefix) + "    ";

    print(prefix, ".name = \"", node.name, "\",");
    print(prefix, ".data = \"", node.data, "\",");
    if(!(node.attrs.empty() && !print_empty_fields)) {
        print(prefix, ".attrs = {");
        for(const auto& a : node.attrs) {
            print(prefix2, "{\"", a.key, "\", \"", a.value, "\"},");
        }
        print(prefix, "},");
    }
    if(!(node.children.empty() && !print_empty_fields)) {
        print(prefix, ".children = {");
        for(const auto& c : node.children) {
            print(prefix2, "Node{");
            dump_node(c, prefix4);
            print(prefix2, "},");
        }
        print(prefix, "},");
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
        print(c.str);
        if(c.expects) {
            unwrap(node, xml::parse(c.str), "parse failed");
            const auto& expect = *c.expects;
            if(node != expect) {
                print("parse result did not match");
                print("expect: ");
                dump_node(expect);
                print("got: ");
                dump_node(node);
                continue;
            }
        } else {
            ensure(!xml::parse(c.str), "parse did not fail");
        }
        print("pass");
    }
    return 0;
}
