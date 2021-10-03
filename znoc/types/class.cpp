#include "class.hpp"

#include "struct.hpp"
#include "type.hpp"
#include "../parsing.hpp"
#include "../macros.hpp"
#include "../main.hpp"
#include <exception>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include "../mangling.hpp"
#include "../constructions/function.hpp"
#include <variant>

llvm::Type* AST::ClassType::codegen(__attribute__((unused)) int template_instance) {
	if (!generated) {

		for (auto &func : functions) {
			func->codegen_prototype();
		}
		for (auto &func : functions) {
			func->codegen();
		}

		std::vector<llvm::Type*> f;
		for (auto &fT : fieldTypes) {
			f.push_back(std::get<AST::type_usage_t>(fT).codegen());
		}
		generated = llvm::StructType::create(*TheContext, llvm::ArrayRef<llvm::Type*>(f));
		generated->setName(full_mangle_name(1, ManglePart::Type, name.c_str()));
	}
	return generated;
}

std::shared_ptr<AST::Type> Parser::parse_class_def(FILE* f) {
	get_next_token(f);
	if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "class name after 'class'");
	std::string name = *std::get_if<std::string>(&currentTokenVal);
	get_next_token(f);

	if (currentToken != '{') throw UNEXPECTED_CHAR(currentToken, "{ after class name");
	get_next_token(f);

	std::vector<std::pair<std::string, AST::type_usage_t>> fields;
	std::vector<std::unique_ptr<AST::Function>> functions;

	if (currentToken != '}') while (1) {
		if (currentToken == tok_func) {
			functions.push_back(Parser::parse_function(f));
			//globalFuncDeclarations.push_back(std::pair<std::unique_ptr<AST::Function>, bool>(std::move(f), false));
		} else {
			if (currentToken != tok_identifier) throw UNEXPECTED_CHAR(currentToken, "identifier for class field");
			std::string fieldName = *std::get_if<std::string>(&currentTokenVal);
			get_next_token(f);
			if (currentToken != ':') throw UNEXPECTED_CHAR(currentToken, ": after class field name");
			get_next_token(f);

			AST::type_usage_t fieldType = parse_type(f);

			fields.push_back(std::pair<std::string, AST::type_usage_t>(fieldName, std::move(fieldType)));
		}

		if (currentToken == '}') break;
		else if (currentToken != ',') throw UNEXPECTED_CHAR(currentToken, ", or } after class field");
		get_next_token(f); // Trim ,
	}
	get_next_token(f); // Trim }

	std::vector<AST::field_type_t> fieldTypes;
	std::map<std::string, int> fieldIndices;

	for (size_t i = 0; i < fields.size(); i++) {
		fieldTypes.push_back(fields[i].second);
		fieldIndices[fields[i].first] = i;
	}

	auto s = std::make_shared<AST::ClassType>(name, std::move(fieldTypes), std::move(fieldIndices), std::move(functions));
	return s;
}