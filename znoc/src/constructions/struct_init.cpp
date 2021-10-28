#include "struct_init.hpp"
#include "construction_parse.hpp"
#include "codeblock.hpp"
#include "binary_op.hpp"
#include "../types/type.hpp"
#include "../parsing.hpp"
#include "../memory/variable.hpp"
#include "../memory/memory_location.hpp"
#include "../memory/memory_ref.hpp"
#include "../memory/gep.hpp"
#include <memory>

std::unique_ptr<AST::Expression> Parser::parse_struct_init(FILE* f, AST::TypeInstance to_init) {
	std::vector<std::unique_ptr<AST::Expression>> cb_exprs;
	auto struct_var = AST::Variable::create_in_scope("struct_init", to_init);
	LIST('{', ',', '}', {
		EXPECT('.', "struct field reference");
		auto field_name = EXPECT_IDENTIFIER("field name");
		EXPECT('=', "before value");
		auto val = parse_binary_expression(f);
		auto field_info = to_init.get_field_info_by_name(field_name);
		auto field_gep = std::make_shared<AST::GEP>(struct_var, field_info.index);
		auto field_ref = std::make_unique<AST::MemoryRef>(field_gep);
		auto assign_expr = std::make_unique<AST::BinaryExpression>(AST::OpType::op_assign, std::move(field_ref), std::move(val));
		cb_exprs.push_back(std::move(assign_expr));
	}, "struct fields");
	cb_exprs.push_back(std::make_unique<AST::MemoryRef>(struct_var));
	return std::make_unique<AST::CodeBlock>(std::move(cb_exprs));
}