#pragma once

#include "Parser.h"
#include "Stack.hpp"

class Interpreter
{
private:
	Stack<double> m_results; /// Store the (intermediary) results of the visits.
							/// Predefined functions store their arguments here.
							/// Ex: When add(5, 6) is received 5 and 6 will get stored here.
	std::vector<double> m_arguments; /// Store the arguments of the user defined functions
									/// Ex: When fact(9) is received, 9 will get stored here and not in the stack.

	int m_offset; /// Defines how much of the arguments in the vector are from a previous function call.
				 /// Used for recursion.

	std::vector<const Node*> m_user_functions; /// Stores pointers to the user defined functions.
											 /// Used vector for easy traversal and constant access time by index.

	/// Like the copy of the Node, casts to every possible Node and calls the appropriate visit method.
	bool visit(const Node* ast, std::ostream& out);
	/// Puts the value in the stack. If the pointer is not a number token then outputs an error.
	bool visit_factor(const Factor_Node* node, std::ostream& out);
	/// Transfers the necessary argument from the vector to the stack.
	bool visit_argument(const Argument_Node* node, std::ostream& out);
	/// If predefined function, pushes its result. Else, searches and applies the definition of the user function.
	bool visit_unary(const Unary_Operation_Node* node, std::ostream& out);
	bool visit_binary(const Binary_Operation_Node* node, std::ostream& out);
	bool visit_if(const If_Opeation_Node* node, std::ostream& out);
	/// Visiting a list means printing its contents.
	bool visit_list(const List_Operation_Node* node, std::ostream& out);
	/// Find the functions, pushes the element of the list to the vector and visits the definition of the map function.
	std::vector<Node*> visit_map(const Map_Operation_Node* node, std::ostream& out);
	/// Finds the function by name, transfers all the arguments from the stack to the vector and visits the definition.
	bool visit_user(const User_Function* node, std::ostream& out);

public:
	/// Sets the offset to 0.
	Interpreter();
	Interpreter(const Interpreter& rhs) = delete;
	Interpreter& operator=(const Interpreter& rhs) = delete;
	~Interpreter();

	/// Calls visit on the ast and then outputs a result, an error or does not output, in case of user function declaration/definition.
	void interpret(const Node* ast, std::ostream& out);
};