#include "Parser.h"

//#################################################
// NODES
//#################################################

/// Since you don't know the type you have to try casting it to all the accepted ones.
void Node::copy(const Node& rhs)
{
	const Argument_Token* a_ptr = dynamic_cast<const Argument_Token*>(rhs.m_token);

	if (a_ptr)
	{
		m_token = new Argument_Token(a_ptr->m_value);
		return;
	}

	const Function_Token* f_ptr = dynamic_cast<const Function_Token*>(rhs.m_token);

	if (f_ptr)
	{
		m_token = new Function_Token(f_ptr->m_name);
		return;
	}

	const Number_Token* n_ptr = dynamic_cast<const Number_Token*>(rhs.m_token);

	if (n_ptr)
	{
		m_token = new Number_Token(n_ptr->m_value);
		return;
	}

	m_token = rhs.m_token ? new Token(rhs.m_token->m_type) : nullptr; // If the input is correct should never be a nullptr.
}

void Node::del()
{
	delete m_token;
	m_token = nullptr;
}

Node::Node(const Token* token)
	: m_token(nullptr)
{
	const Argument_Token* a_ptr = dynamic_cast<const Argument_Token*>(token);

	if (a_ptr)
	{
		m_token = new Argument_Token(a_ptr->m_value);
		return;
	}

	const Function_Token* f_ptr = dynamic_cast<const Function_Token*>(token);

	if (f_ptr)
	{
		m_token = new Function_Token(f_ptr->m_name);
		return;
	}

	const Number_Token* n_ptr = dynamic_cast<const Number_Token*>(token);

	if (n_ptr)
	{
		m_token = new Number_Token(n_ptr->m_value);
		return;
	}

	if (token)
	{
		m_token = new Token(token->m_type);
	}
}

Node::Node(const Node & rhs)
{
	copy(rhs);
}

Node& Node::operator=(const Node& rhs)
{
	if (this != &rhs)
	{
		del();
		copy(rhs);
	}
	return *this;
}

Node::~Node()
{
	del();
}

void Node::print(std::ostream& out) const
{
	m_token->print(out);
}

Node* Node::clone() const
{
	return new Node(*this);
}

Factor_Node::Factor_Node(const Token* token)
	: Node(token)
{ }

Factor_Node::Factor_Node(const Factor_Node & rhs)
	: Node(rhs)
{ }

Factor_Node& Factor_Node::operator=(const Factor_Node & rhs)
{
	Node::operator=(rhs);
	return *this;
}

Factor_Node* Factor_Node::clone() const
{
	return new Factor_Node(*this);
}

Argument_Node::Argument_Node(const Token * token)
	: Node(token)
{ }

Argument_Node::Argument_Node(const Argument_Node & rhs)
	: Node(rhs)
{ }

Argument_Node& Argument_Node::operator=(const Argument_Node & rhs)
{
	Node::operator=(rhs);
	return *this;
}

Argument_Node* Argument_Node::clone() const
{
	return new Argument_Node(*this);
}

Unary_Operation_Node::Unary_Operation_Node(const Token* token, const Node* a)
	: Node(token),
	m_argument(a)
{ }

Unary_Operation_Node::Unary_Operation_Node(const Unary_Operation_Node & rhs)
	: Node(rhs.m_token),
	m_argument(rhs.m_argument->clone())
{ }

Unary_Operation_Node& Unary_Operation_Node::operator=(const Unary_Operation_Node & rhs)
{
	if (this != &rhs)
	{
		delete m_argument;
		m_argument = nullptr;

		Node::operator=(rhs);
		m_argument = rhs.m_argument->clone();
	}
	return *this;
}

Unary_Operation_Node::~Unary_Operation_Node()
{
	delete m_argument;
	m_argument = nullptr;
}

void Unary_Operation_Node::print(std::ostream& out) const
{
	out << '(';
	Node::print(out);
	out << ' ';
	m_argument->print(out);
	out << ')';
}

Unary_Operation_Node* Unary_Operation_Node::clone() const
{
	return new Unary_Operation_Node(*this);
}

Binary_Operation_Node::Binary_Operation_Node(const Token* token, const Node* left, const Node* right)
	: Node(token),
	m_left(left),
	m_right(right)
{ }

Binary_Operation_Node::Binary_Operation_Node(const Binary_Operation_Node & rhs)
	: Node(rhs.m_token),
	m_left(rhs.m_left),
	m_right(rhs.m_right)
{ }

Binary_Operation_Node& Binary_Operation_Node::operator=(const Binary_Operation_Node & rhs)
{
	if (this != &rhs)
	{
		delete m_left;
		m_left = nullptr;
		delete m_right;
		m_right = nullptr;

		Node::operator=(rhs);
		m_left = rhs.m_left->clone();
		m_right = rhs.m_right->clone();
	}
	return *this;
}

Binary_Operation_Node::~Binary_Operation_Node()
{
	delete m_left;
	m_left = nullptr;
	delete m_right;
	m_right = nullptr;
}

void Binary_Operation_Node::print(std::ostream& out) const
{
	out << '(';

	m_left->print(out);

	out << ' ';
	
	Node::print(out); // Changed order for debug purposes.

	out << ' ';
	
	m_right->print(out);
	
	out << ')';
}

Binary_Operation_Node* Binary_Operation_Node::clone() const
{
	return new Binary_Operation_Node(*this);
}

void If_Opeation_Node::copy(const If_Opeation_Node& rhs)
{
	m_check = rhs.m_check->clone();
	m_left = rhs.m_left->clone();
	m_right = rhs.m_right->clone();
}

void If_Opeation_Node::del()
{
	delete m_check;
	m_check = nullptr;
	delete m_left;
	m_left = nullptr;
	delete m_right;
	m_right = nullptr;
}

If_Opeation_Node::If_Opeation_Node(const Token* token, const Node* check, const Node* left, const Node* right)
	: Node(token),
	m_check(check),
	m_left(left),
	m_right(right)
{ }

If_Opeation_Node::If_Opeation_Node(const If_Opeation_Node & rhs)
	: Node(rhs.m_token)
{
	copy(rhs);
}

If_Opeation_Node& If_Opeation_Node::operator=(const If_Opeation_Node& rhs)
{
	if (this != &rhs)
	{
		del();
		Node::operator=(rhs);
		copy(rhs);
	}
	return *this;
}

If_Opeation_Node::~If_Opeation_Node()
{
	delete m_check;
	m_check = nullptr;
	delete m_left;
	m_left = nullptr;
	delete m_right;
	m_right = nullptr;
}

void If_Opeation_Node::print(std::ostream& out) const
{
	out << '(';
	
	Node::print(out);
	
	out << ' ';

	m_check->print(out);

	out << ' ';

	m_left->print(out);

	out << ' ';

	m_right->print(out);

	out << ')';
}

If_Opeation_Node* If_Opeation_Node::clone() const
{
	return new If_Opeation_Node(*this);
}

List_Operation_Node::List_Operation_Node(const Token* token, const std::vector<Node*> contents)
	: Node(token),
	m_contents(contents)
{ }

List_Operation_Node::~List_Operation_Node()
{
	for (Node* a : m_contents)
	{
		delete a;
		a = nullptr;
	}
}

void List_Operation_Node::print(std::ostream& out) const
{
	out << '(';
	Node::print(out);
	out << ' ';
	for (Node* a : m_contents)
	{
		a->print(out);
		out << ' ';
	}
	out << ')';
}

Map_Operation_Node::Map_Operation_Node(const Token* token, const Node* functor, const Node* list)
	: Node(token),
	m_functor(functor),
	m_list(list)
{ }

Map_Operation_Node::~Map_Operation_Node()
{
	delete m_functor;
	m_functor = nullptr;
}

void Map_Operation_Node::print(std::ostream& out) const
{
	if (!m_functor || !m_list)
	{
		return;
	}

	out << '(';
	Node::print(out);
	out << ' ';
	m_functor->print(out);
	out << ' ';
	m_list->print(out);
	out << ')';
}

void User_Function::copy(const User_Function& rhs)
{
	m_definition = rhs.m_definition->clone();

	for (const Node* a : rhs.m_arguments)
	{
		m_arguments.push_back(a->clone());
	}
}

void User_Function::del()
{
	for (const Node* a : m_arguments)
	{
		delete a;
		a = nullptr;
	}
}

User_Function::User_Function(const Token* token, const Node* definition, const std::vector<const Node*>& arguments)
	: Node(token),
	m_definition(definition),
	m_arguments(arguments)
{ }

User_Function::User_Function(const User_Function & rhs)
	: Node(rhs.m_token)
{
	copy(rhs);
}

User_Function& User_Function::operator=(const User_Function& rhs)
{
	if (this != &rhs)
	{
		del();
		Node::operator=(rhs);
		copy(rhs);
	}
	return *this;
}

User_Function::~User_Function()
{
	delete m_definition;
	m_definition = nullptr;
}

void User_Function::print(std::ostream& out) const
{
	out << '(';
	Node::print(out);
	out << ' ';
	if (m_definition)
	{
		m_definition->print(out);
	}
	else
	{
		out << "()";
	}
	if (m_arguments.size())
	{
		out << ' ';
		for (const Node* const a : m_arguments)
		{
			a->print(out);
			out << ' ';
		}
	}
	out << ')';
}

User_Function* User_Function::clone() const
{
	return new User_Function(*this);
}

//#################################################
// PARSER
//#################################################

void Parser::advance()
{
	if (m_current_index + 1 < m_tokens.size())
	{
		++m_current_index;
		m_current_type = m_tokens[m_current_index]->m_type;
	}
	else
	{
		m_current_index = -1;
	}
}

Node* Parser::factor(std::ostream& out)
{
	if (m_current_index != -1)
	{
		Factor_Node* n = new Factor_Node(m_tokens[m_current_index]);
		advance();
		return n;
	}

	Illegal_Syntax("Expected a number").print(out);
	return nullptr;
}

Node* Parser::expr(std::ostream& out)
{
	if (m_current_type != Type::NUMBER)
	{
		if (m_current_type != Type::FUNCTION_NAME)
		{
			if (m_current_type == Type::CLOSING_BRACKET)
			{
				advance();
				return nullptr;
			}
			Illegal_Syntax("Expected a function name. Received: ").print(out);
			m_tokens[m_current_index]->print(out);
			out << "\n\n";
			return nullptr;
		}

		Token* const& operation = m_tokens[m_current_index]; // This is the parent token which is a term. It may have n children.

		advance();

		Function_Token* f_ptr = dynamic_cast<Function_Token*>(operation); // It gets special attention if it is a list or map function.

		if (f_ptr && f_ptr->m_name == "list")
		{
			if (m_current_index == -1 || m_current_type != Type::OPENING_BRACKET)
			{
				Illegal_Syntax("Expected '('").print(out);
				return nullptr;
			}

			advance();

			std::vector<Node*> arguments;

			do
			{
				if (m_current_index == -1)
				{
					Illegal_Syntax("Unexpected end of input").print(out);
					return nullptr;
				}
				else if (m_current_type == Type::COMMA)
				{
					advance();
				}

				arguments.push_back(expr(out));

				size_t size = m_tokens.size();

				if (m_current_type == Type::CLOSING_BRACKET && (m_current_index + 1 >= size || m_tokens[m_current_index + 1]->m_type == Type::CLOSING_BRACKET || m_current_index + 2 >= size || m_tokens[m_current_index + 2]->m_type == Type::FUNCTION_NAME))
				{
					if (m_current_index + 1 < size && m_tokens[m_current_index + 1]->m_type == Type::CLOSING_BRACKET)
					{
						advance();
					}
					break;
				}

				advance();

				if (m_current_index != -1 && m_current_type == Type::CLOSING_BRACKET)
				{
					advance();
					if (m_current_index == -1)
					{
						break;
					}
					else if (m_current_type == Type::COMMA)
					{
						advance();
					}
				}
			} while (m_current_index != -1 && (m_current_type == Type::FUNCTION_NAME || m_current_type == Type::COMMA || m_current_type == Type::NUMBER));

			return new List_Operation_Node(operation, arguments);
		}
		else if (f_ptr && f_ptr->m_name == "map")
		{
			advance();
			return new Map_Operation_Node(operation, expr(out), expr(out));
		}

		if (m_current_index == -1 || m_current_type != Type::OPENING_BRACKET)
		{
			if (m_current_type == Type::ARROW)
			{
				advance();
				return new User_Function(operation, expr(out), {});
			}

			if (m_current_type == Type::COMMA || m_current_type == Type::CLOSING_BRACKET)
			{
				advance();
				return new User_Function(operation, nullptr, {});
			}

			if (m_current_index != -1)
			{
				Illegal_Syntax("Expected '(' or list. Received: ").print(out);
				m_tokens[m_current_index]->print(out);
				out << "\n\n";
				return nullptr;
			}

			return new User_Function(operation, nullptr, {});
		}

		advance();

		if (m_current_index == -1)
		{
			Illegal_Syntax("Unexpected end of input").print(out);
			return nullptr;
		}

		Node* left;

		switch (m_current_type)
		{
		case Type::FUNCTION_NAME:
		{
			left = expr(out);
			advance();
			break;
		}
		case Type::NUMBER:
		{
			left = factor(out);
			break;
		}
		case Type::ARGUMENT:
		{
			left = new Argument_Node(m_tokens[m_current_index]);
			advance();
			break;
		}
		default:
			return new User_Function(operation, expr(out), {});
		}

		if (m_current_index == -1)
		{
			Illegal_Syntax("Expected ','").print(out);
			return nullptr;
		}

		if (m_current_type != Type::COMMA)
		{
			if (m_current_type == Type::CLOSING_BRACKET)
			{
				return new Unary_Operation_Node(operation, left);
			}

			Illegal_Syntax("Expected ','. Received: ").print(out);
			m_tokens[m_current_index]->print(out);
			out << "\n\n";
			return nullptr;
		}

		advance();

		if (m_current_index == -1)
		{
			Illegal_Syntax("Unexpected end of input").print(out);
			return nullptr;
		}

		Node* right;

		switch (m_current_type)
		{
		case Type::FUNCTION_NAME:
		{
			right = expr(out);
			advance();
			break;
		}
		case Type::ARGUMENT:
		{
			right = new Argument_Node(m_tokens[m_current_index]);
			advance();
			break;
		}
		default:
			right = factor(out);
		}

		if (m_current_index != -1 && m_current_type != Type::CLOSING_BRACKET)
		{
			if (m_current_type == Type::COMMA)
			{
				Function_Token* f_ptr = dynamic_cast<Function_Token*>(operation);
				if (f_ptr && f_ptr->m_name == "if")
				{
					advance();
					return new If_Opeation_Node(f_ptr, left, right, expr(out));
				}
				else
				{
					advance();
					std::vector<const Node*> m_arguments;
					m_arguments.push_back(left);
					m_arguments.push_back(right);
					Node* n = expr(out);
					while (n)
					{
						m_arguments.push_back(n);
						advance();
						if (m_current_type == Type::COMMA)
						{
							advance();
						}
						n = expr(out);
					}
					return new User_Function(operation, nullptr, m_arguments);
				}
			}

			Illegal_Syntax("Expected ')'. Received: ").print(out);
			m_tokens[m_current_index]->print(out);
			out << "\n\n";
			return nullptr;
		}

		return new Binary_Operation_Node(operation, left, right);
	}

	return factor(out);
}

Parser::Parser(const std::vector<Token*>& tokens)
	: m_tokens(tokens),
	m_current_index(-1)
{
	advance();
}

Parser::~Parser()
{
	for (auto a : m_tokens)
	{
		delete a;
		a = nullptr;
	}
}

Node* Parser::parse(std::ostream& out)
{
	if (m_current_index == -1)
	{
		return nullptr;
	}

	return expr(out);
}