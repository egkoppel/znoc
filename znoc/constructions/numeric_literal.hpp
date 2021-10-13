#ifndef _NUMERICAL_LITERAL_H
#define _NUMERICAL_LITERAL_H

#include "expression.hpp"
#include "../types/builtins.hpp"
#include "../main.hpp"
#include <llvm/IR/Constant.h>
#include <llvm/ADT/APFloat.h>
#include <cfloat>

namespace AST {
	class NumericLiteral: public AST::Expression {
		union {
			uint64_t i;
			double f;
		} value;
		bool is_int_val;

		static bool is_int(double v) {
			uint64_t vi = v;
			double vc = v - vi;
			return vc == 0;
		}

		public:
		NumericLiteral(double v): Expression(is_int(v) ? (v > UINT32_MAX ? AST::get_fundamental_type("i64") : AST::get_fundamental_type("i32")) : (v > FLT_MAX ? AST::get_fundamental_type("double") : AST::get_fundamental_type("float"))) {
			is_int_val = is_int(v);
			if (is_int_val) value.i = v;
			else value.f = v;
		}
		virtual llvm::Value* codegen(__attribute__((unused)) llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name = "");
	};
}

#endif