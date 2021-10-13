#ifndef _ZNO_NAMESPACE_H
#define _ZNO_NAMESPACE_H

#include <map>
#include <string>
#include "../types/type.hpp"
#include "function_def.hpp"
#include <stdio.h>
#include <iostream>
//#include "../main.hpp"

namespace AST {
	class Namespace {
		std::map<std::string, std::shared_ptr<AST::TypeBase>> named_types;
		std::map<std::string, std::shared_ptr<AST::Function>> named_functions;
		std::map<std::string, std::unique_ptr<AST::Namespace>> namespaces;
		std::string name;

		public:
		Namespace(std::string name): name(name), named_types(), named_functions(), namespaces() {}
		~Namespace() {}
		virtual std::string get_name() { return name; }

		std::shared_ptr<AST::TypeBase> get_type_by_name(std::string name);
		std::shared_ptr<AST::Function> get_function_by_name(std::string name);
		AST::Namespace* get_namespace_by_name(std::string name);

		Namespace* operator <<(std::shared_ptr<AST::TypeBase> t) {
			named_types.insert({t->get_name(), t});
			return this;
		}

		Namespace* operator <<(std::shared_ptr<AST::Function> f) {
			named_functions.insert({f->get_name(), f});
			return this;
		}

		Namespace* operator <<(std::unique_ptr<AST::Namespace> n) {
			std::cout << "Creating new global namespace called `" << n->get_name() << "`" << std::endl;
			namespaces.insert({n->get_name(), std::move(n)});
			return this;
		}

		void codegen() {
			for (auto &n : namespaces) {
				n.second->codegen();
			}

			for (auto &func : named_functions) {
				func.second->codegen_prototype();
			}

			for (auto &func : named_functions) {
				func.second->codegen();
			}
		}
	};
}

namespace Parser {
	struct NamespaceParseReturn {
		AST::Namespace* parsed_namespace;
		std::string next_identifier;
		int next_token;
	};
	NamespaceParseReturn parse_namespace(FILE* f);
}

#endif