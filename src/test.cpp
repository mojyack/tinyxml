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
    Result<Node, Error> expects;
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
        Error::NotXML,
    },
    TestCase{
        R"(<node1><node2></node2></node1></node0>)",
        Error::NodeStackUnderFlow,
    },
    TestCase{
        R"(<node1><node2></node1></node2>)",
        Error::OpenCloseMismatch,
    },
    TestCase{
        R"(<node>)",
        Error::Incomplete,
    },
    TestCase{
        R"(<node1><node2><node1/>)",
        Error::Incomplete,
    },
};

auto test() -> bool {
    auto pass_count = 0;
    for(const auto& c : test_cases) {
        print(c.str);
        auto n_r = parse(c.str);
        if(c.expects) {
            if(!n_r) {
                print("parse failed: ", int(n_r.as_error()));
                continue;
            }
            const auto& n      = n_r.as_value();
            const auto& expect = c.expects.as_value();
            if(n != expect) {
                print("parse result did not match");
                print("expect: ");
                dump_node(expect);
                print("got: ");
                dump_node(n);
                continue;
            }
        } else {
            if(n_r) {
                print("parse did not fail");
                continue;
            }
            const auto  e      = n_r.as_error();
            const auto& expect = c.expects.as_error();
            if(e != expect) {
                print("parse error did not match");
                print("expect: Error ", int(expect));
                print("got: Error ", int(e));
                continue;
            }
        }
        print("pass");
        pass_count += 1;
    }
    return pass_count == test_cases.size();
}
} // namespace xml

auto main() -> int {
    return xml::test() ? 0 : 1;
}
