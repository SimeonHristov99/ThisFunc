#include "Interpreter.h"

bool Interpreter::visit(const Node* ast, std::ostream& out)
{
	const Factor_Node* f_ptr = dynamic_cast<const Factor_Node*>(ast);

	if (f_ptr)
	{
		return visit_factor(f_ptr, out);
	}

	const Argument_Node* a_ptr = dynamic_cast<const Argument_Node*>(ast);

	if (a_ptr)
	{
		return visit_argument(a_ptr, out);
	}

	const Unary_Operation_Node* u_ptr = dynamic_cast<const Unary_Operation_Node*>(ast);

	if (u_ptr)
	{
		if (!visit(u_ptr->m_argument, out))
		{
			return false;
		}

		return visit_unary(u_ptr, out);
	}

	const Binary_Operation_Node* b_ptr = dynamic_cast<const Binary_Operation_Node*>(ast);

	if (b_ptr)
	{
		if (dynamic_cast<const Function_Token*>(b_ptr->m_token)->m_name == "concat")
		{
			const List_Operation_Node* l1 = dynamic_cast<const List_Operation_Node*>(b_ptr->m_left);
			const List_Operation_Node* l2 = dynamic_cast<const List_Operation_Node*>(b_ptr->m_right);

			std::vector<Node*> result = l1->m_contents;

			for (Node* a : l2->m_contents)
			{
				result.push_back(a->clone());
			}

			List_Operation_Node* n = new List_Operation_Node(new Token(l1->m_token->m_type), result);
			bool b = visit(n, out);
			delete n;
			for (Node* a : result)
			{
				a = nullptr;
			}
			return b;
		}

		if (!visit(b_ptr->m_left, out) || !visit(b_ptr->m_right, out) || !visit_binary(b_ptr, out))
		{
			return false;
		}

		return true;
	}

	const If_Opeation_Node* i_ptr = dynamic_cast<const If_Opeation_Node*>(ast);

	if (i_ptr)
	{
		return visit_if(i_ptr, out);
	}

	const List_Operation_Node* l_ptr = dynamic_cast<const List_Operation_Node*>(ast);

	if (l_ptr)
	{
		return visit_list(l_ptr, out);
	}

	const Map_Operation_Node* m_ptr = dynamic_cast<const Map_Operation_Node*>(ast);

	if (m_ptr)
	{	
		List_Operation_Node* n = new List_Operation_Node(new Token(m_ptr->m_functor->m_token->m_type), visit_map(m_ptr, out));
		bool b = visit(n, out);
		delete n;
		return b;
	}

	const User_Function* u_f_ptr = dynamic_cast<const User_Function*>(ast);

	if (u_f_ptr)
	{
		if (!visit_user(u_f_ptr, out))
		{
			return false;
		}

		return true;
	}

	Runtime_Error("No matching definition found").print(out);
	return false;
}

bool Interpreter::visit_factor(const Factor_Node* node, std::ostream& out)
{
	const Number_Token* number = dynamic_cast<const Number_Token*>(node->m_token);

	if (number)
	{
		m_results.push(number->m_value);
		return true;
	}

	Runtime_Error("Expected a number").print(out);
	return false;
}

bool Interpreter::visit_argument(const Argument_Node* node, std::ostream& out)
{
	const Argument_Token* current = dynamic_cast<const Argument_Token*>(node->m_token);

	if (current)
	{
		if (current->m_value >= m_arguments.size())
		{
			Runtime_Error("Too few arguments in function call").print(out);
			return false;
		}

		m_results.push(m_arguments[m_offset + current->m_value]);
		return true;
	}

	Runtime_Error("Expected an argument").print(out);
	return false;
}

bool Interpreter::visit_unary(const Unary_Operation_Node* node, std::ostream& out)
{
	const Function_Token* f_token = dynamic_cast<const Function_Token*>(node->m_token);

	if (f_token->m_name == "sqrt")
	{
		m_results.push(sqrt(m_results.pop()));
		return true;
	}

	if (f_token->m_name == "sin")
	{
		m_results.push(sin(m_results.pop()));
		return true;
	}

	if (f_token->m_name == "cos")
	{
		m_results.push(cos(m_results.pop()));
		return true;
	}

	for (const Node* a : m_user_functions)
	{
		const User_Function* current_ptr = dynamic_cast<const User_Function*>(a);

		if (current_ptr)
		{
			const Function_Token* current_name = dynamic_cast<const Function_Token*>(current_ptr->m_token);

			if (current_name->m_name == dynamic_cast<const Function_Token*>(node->m_token)->m_name)
			{
				size_t size = m_arguments.size();
				size_t diff = size - m_offset;
				m_offset = size == 0 ? 0 : size;

				m_arguments.push_back(m_results.pop());

				if (!visit(current_ptr->m_definition, out))
				{
					return false;
				}

				m_offset -= diff;
				m_arguments.pop_back();
				return true;
			}
		}
	}

	Runtime_Error("No matching function definition found").print(out);
	return false;
}

bool Interpreter::visit_binary(const Binary_Operation_Node* node, std::ostream& out)
{
	double right = m_results.pop();
	double left = m_results.pop();

	const Function_Token* f_token = dynamic_cast<const Function_Token*>(node->m_token);

	if (f_token->m_name == "add")
	{
		m_results.push(left + right);
		return true;
	}
	if (f_token->m_name == "sub")
	{
		m_results.push(left - right);
		return true;
	}
	if (f_token->m_name == "mul")
	{
		m_results.push(left * right);
		return true;
	}
	if (f_token->m_name == "div")
	{
		if (right == 0)
		{
			Runtime_Error("Division by 0").print(out);
			return false;
		}

		m_results.push(left / right);
		return true;
	}
	if (f_token->m_name == "pow")
	{
		m_results.push(pow(left, right));
		return true;
	}
	if (f_token->m_name == "eq")
	{
		m_results.push(left == right);
		return true;
	}
	if (f_token->m_name == "le")
	{
		m_results.push(left < right);
		return true;
	}
	if (f_token->m_name == "nand")
	{
		m_results.push(!left || !right);
		return true;
	}

	for (const Node* a : m_user_functions)
	{
		const User_Function* current_ptr = dynamic_cast<const User_Function*>(a);

		if (current_ptr)
		{
			const Function_Token* current_name = dynamic_cast<const Function_Token*>(current_ptr->m_token);

			if (current_name->m_name == dynamic_cast<const Function_Token*>(node->m_token)->m_name)
			{
				size_t size = m_arguments.size();
				size_t diff = size - m_offset;
				m_offset = size == 0 ? 0 : size;

				m_arguments.push_back(left);
				m_arguments.push_back(right);

				if (!visit(current_ptr->m_definition, out))
				{
					return false;
				}

				m_offset -= diff;
				m_arguments.pop_back();
				m_arguments.pop_back();

				return true;
			}
		}
	}

	Runtime_Error("No matching function definition found").print(out);
	return false;
}

bool Interpreter::visit_if(const If_Opeation_Node* node, std::ostream& out)
{
	if (!visit(node->m_check, out))
	{
		return false;
	}

	if (m_results.pop() == 0)
	{
		return visit(node->m_right, out);
	}

	return visit(node->m_left, out);
}

bool Interpreter::visit_list(const List_Operation_Node* node, std::ostream& out)
{
	out << '[';

	std::vector<Node*>::const_iterator it = node->m_contents.begin();

	while (it + 1 != node->m_contents.end())
	{
		if (!visit(*it, out))
		{
			return false;
		}
		out << m_results.pop() << ", ";

		++it;
	}

	if (!visit(*it, out))
	{
		return false;
	}
	out << m_results.pop() << ']';

	return true;
}

std::vector<Node*> Interpreter::visit_map(const Map_Operation_Node* node, std::ostream& out)
{
	const Function_Token* f_name = dynamic_cast<const Function_Token*>(node->m_list->m_token);
	const Function_Token* l_name = dynamic_cast<const Function_Token*>(node->m_functor->m_token);

	if (!f_name || !l_name)
	{
		Runtime_Error("Function could not be deduced").print(out);
		return {};
	}

	// Go and find their pointers.

	const User_Function* map_ptr = nullptr;
	const List_Operation_Node* list_ptr = nullptr;

	size_t j = 0;

	for (const Node* a : m_user_functions)
	{
		const User_Function* current_ptr = dynamic_cast<const User_Function*>(a);

		if (current_ptr)
		{
			const Function_Token* current_name = dynamic_cast<const Function_Token*>(current_ptr->m_token);

			if (current_name->m_name == f_name->m_name || current_name->m_name == l_name->m_name)
			{				
				if (current_name->m_name == f_name->m_name)
				{
					map_ptr = dynamic_cast<const User_Function*>(current_ptr);
					++j;
				}
				else
				{
					list_ptr = dynamic_cast<const List_Operation_Node*>(current_ptr->m_definition);
					++j;
				}
			}
		}
		if (j == 2)
		{
			break;
		}
	}

	if (j < 2)
	{
		Runtime_Error("No mathing function definition found").print(out);
		return {};
	}

	std::vector<Node*> new_contents;

	if (list_ptr && map_ptr)
	{
		for (const Node* a : list_ptr->m_contents)
		{
			if (!visit(a, out))
			{
				return {};
			}

			m_arguments.push_back(m_results.pop());

			if (!visit(map_ptr->m_definition, out))
			{
				return {};
			}

			++m_offset;

			new_contents.push_back(new Factor_Node(new Number_Token(m_results.pop())));

			--m_offset;
			m_arguments.pop_back();
		}
	}

	return new_contents;
}

bool Interpreter::visit_user(const User_Function* node, std::ostream& out)
{
	for (const Node* a : m_user_functions)
	{
		const User_Function* current_ptr = dynamic_cast<const User_Function*>(a);

		if (current_ptr)
		{
			const Function_Token* current_name = dynamic_cast<const Function_Token*>(current_ptr->m_token);

			if (current_name->m_name == dynamic_cast<const Function_Token*>(node->m_token)->m_name)
			{
				if (node->m_definition)
				{
					Runtime_Error("A function with the same name already exists").print(out);
					return false;
				}

				if (node->m_arguments.size() == 0)
				{
					return visit(current_ptr->m_definition, out);
				}

				for (const Node* a : node->m_arguments)
				{
					if (!visit(a, out))
					{
						return false;
					}
				}

				size_t size = m_arguments.size();
				size_t diff = size - m_offset;
				m_offset = size == 0 ? 0 : size;

				size = m_results.size() + m_offset;

				if (size > m_arguments.size())
				{
					m_arguments.resize(size);
				}

				while (!m_results.is_empty() && size)
				{
					m_arguments[--size] = m_results.pop();
				}

				if (!visit(current_ptr->m_definition, out))
				{
					return false;
				}

				m_offset -= diff;

				m_arguments.clear();

				return true;
			}
		}
	}

	if (!node->m_definition)
	{
		Runtime_Error("Expected \"<-\"").print(out);
		return false;
	}

	const Function_Token* function_name = dynamic_cast<const Function_Token*>(node->m_token);
	const Function_Token* its_definition = dynamic_cast<const Function_Token*>(node->m_definition->m_token);

	if(function_name && its_definition && function_name->m_name == its_definition->m_name)
	{
		Runtime_Error("Function will cause stack overflow and hence will not be created").print(out);
		return false;
	}

	m_user_functions.push_back(node);
	return true;
}

Interpreter::Interpreter()
	: m_offset(0)
{ }

Interpreter::~Interpreter()
{
	for (const Node* a : m_user_functions)
	{
		delete a;
		a = nullptr;
	}
}

void Interpreter::interpret(const Node* ast, std::ostream& out)
{
	if (!visit(ast, out))
	{
		m_offset = 0;
		m_arguments.clear();
		while (!m_results.is_empty())
		{
			m_results.pop();
		}
		return;
	}

	if (!m_results.is_empty())
	{
		out << m_results.pop();
	}

	if (!m_results.is_empty() || !m_arguments.empty() || m_offset != 0)
	{
		Runtime_Error("Unexpected argument").print(out);
	}
}