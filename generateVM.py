NAME = 0
ARGS = 1
CODE = 2

arg_options = {
    "mso": [
        ("args_16", "stack.arg_at(ip.read<std::uint16_t>())"),
        ("global_16", "globals.get(*string_pointer(resources.at(ip.read<std::uint16_t>()))).peek()"),
        ("local_16", "stack.local_at(ip.read<std::uint16_t>())"),
        ("resource_16", "resources.at(ip.read<std::uint16_t>())"),
        ("args_8", "stack.arg_at(ip.read<std::uint8_t>())"),
        ("global_8", "globals.get(*string_pointer(resources.at(ip.read<std::uint8_t>()))).peek()"),
        ("local_8", "stack.local_at(ip.read<std::uint8_t>())"),
        ("resource_8", "resources.at(ip.read<std::uint8_t>())"),
        ("builtin", "context.builtin(ip.read<bytecode::member::builtin>())")
    ],
    "mdo": [
        ("args_16", "stack.arg_at(ip.read<std::uint16_t>())"),
        ("global_16", "stack.local_at(ip.read<std::uint16_t>())"),
        ("local_16", "stack.local_at(ip.read<std::uint16_t>())"),
        ("args_8", "stack.arg_at(ip.read<std::uint8_t>())"),
        ("global_8", "stack.local_at(ip.read<std::uint8_t>())"),
        ("local_8", "stack.local_at(ip.read<std::uint8_t>())")
    ],
    "co": [
        ("int_8", "ip.read<std::int8_t>()"),
        ("int_16", "ip.read<std::int16_t>()"),
        ("int_32", "ip.read<std::int32_t>()"),
        ("int_64", "ip.read<std::int64_t>()"),
        ("double", "ip.read<double>()"),
        ("test_register", "test_register"),
        ("null", "nullptr")
    ],
    "ro": [
        ("8", "resources.at(ip.read<std::uint8_t>())"),
        ("16", "resources.at(ip.read<std::uint16_t>())"),
        ("32", "resources.at(ip.read<std::uint32_t>())")
    ],
    "oo": [
        ("8", "ip.read<std::int8_t>()"),
        ("16", "ip.read<std::int16_t>()"),
        ("32", "ip.read<std::int32_t>()")
    ]
}
opcodes = [
    ("oc_operator", ("mso", "mso", "mdo"), """const auto left = member_pointer({0});
const auto right = member_pointer({1});
const auto op = ip.read<infix_operator>();
auto& dest = {2};

token->set(dest, left->operation(*right, op));

break;"""),

    ("oc_instantiate", ("co", "mdo"), """const auto constant = {0};

token->template set<bia::member::member>({1}, creator::create(constant));

break;"""),

    ("oc_invoke", ("mso", "mdo"), """const auto from = ip.read<std::uint8_t>();
const auto kwargs = ip.read<std::uint8_t>();
auto sub_view = stack.sub_view(from);

if (kwargs > sub_view.arg_count()) {{
    BIA_IMPLEMENTATION_ERROR("invalid arg count");
}}

auto result = member_pointer({0})->invoke(connector::parameters{{ sub_view, sub_view.arg_count() - kwargs, kwargs }});

stack.drop(sub_view.arg_count() - kwargs);
token->set({1}, std::move(result));

break;"""),

    ("oc_refer", ("mso", "mdo"), """auto& src = {0};

token->set({1}, src);

break;"""),

    ("oc_clone", ("mso", "mdo"), """const auto src = member_pointer({0});

if (src->flags() & flag::flag_clone_is_copy) {{
    token->set({1}, src->copy());
}} else {{
    token->set({1}, src);
}}

break;"""),

    ("oc_copy", ("mso", "mdo"),  """const auto src = member_pointer({0});

token->set({1}, src->copy());

break;"""),

    ("oc_self_operator", ("mso", "mdo"), """const auto op = ip.read<self_operator>();
const auto src = member_pointer({0});
auto& dest = {1};

token->set(dest, src->self_operation(op));

break;"""),

    ("oc_test", ("mso", "mso"), """const auto op = ip.read<test_operator>();
const auto src = member_pointer({0});

test_register = src->test(op, *member_pointer({1}));

break;"""),


    ("oc_get", ("mso", "ro", "mdo"), """const auto src = member_pointer({0});
const auto name = string_pointer({1});

token->set({2}, src->get(*name));

break;"""),

    ("oc_import", ("ro", "mdo"), """auto name = string_pointer({0});

token->set({1}, loader.load(name).get());

break;"""),


    ("oc_jump", ("oo",), """const auto offset = {0};

ip += offset;

break;"""),

    ("oc_jump_true", ("oo",), """const auto offset = {0};

if (test_register) {{
	ip += offset;
}}

break;"""),

    ("oc_jump_false", ("oo",), """const auto offset = {0};

if (!test_register) {{
	ip += offset;
}}

break;"""),

    ("oc_name", ("ro",), """auto& src = stack.local_at(stack.cursor());

token->set(src, make_key_value_pair(string_pointer({0}), member_pointer(src)));

break;"""),


    ("oc_return_void", tuple(), "return;"),
    ("oc_invert", tuple(), """test_register = !test_register;

break;"""),
    ("oc_drop", tuple(), """stack.drop(ip.read<std::uint8_t>());

break;""")
]

for opcode in opcodes:
    args_template = ""

    for i, arg in enumerate(opcode[ARGS]):
        count = "".join("*{}_count".format(j) for j in opcode[ARGS][i+1:])
        args_template += " - {}_{{}}{}".format(arg, count)

    def rec(rest, *args):
        if len(rest) > 0:
            for i in reversed(arg_options[rest[0]]):
                rec(rest[1:], *args, i)
        else:
            print("case ({}{}): {{".format(
                opcode[NAME], args_template.format(*(i[0] for i in args))))
            print(opcode[CODE].format(*(i[1] for i in args)), end="\n}\n")

    rec(opcode[ARGS])
    #case = "case ({}{}):".format(opcode[NAME], args_template.format([i() for i in args]))

print("default: BIA_THROW(exception::opcode, \"invalid opcode\");")
