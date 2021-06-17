#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

///#################################################
/// ERRORS
///#################################################

Error::Error(const std::string& name, const std::string& details)
	: m_name(name),
	m_details(details)
{ }

void Error::print(std::ostream & out) const
{
	out << m_name << ": " << m_details << "\n\n";
}

Illegal_Character::Illegal_Character(const std::string& c, const std::string& input, const unsigned index)
	: Error("Illegal Character", "'" + c + "'"),
	m_input(input),
	m_index(index)
{ }

void Illegal_Character::print(std::ostream & out) const
{
	out << m_input << '\n';

	for (size_t i = 0; i < m_index; ++i)
	{
		out << ' ';
	}

	out << "^\n";

	Error::print(out);
}

Illegal_Syntax::Illegal_Syntax(const std::string & details)
	: Error("Illegal Syntax", details)
{ }

Runtime_Error::Runtime_Error(const std::string& details)
	: Error("Runtime Error", details)
{ }

///#################################################
/// TOKENS
///#################################################

Token::Token(const Type type)
	: m_type(type)
{ }

void Token::print(std::ostream & out) const
{
	switch (m_type)
	{
	case Type::ARGUMENT:
	{
		out << "ARGUMENT";
		break;
	}
	case Type::ARROW:
	{
		out << "ARROW";
		break;
	}
	case Type::OPENING_BRACKET:
	{
		out << "OPENING_BRACKET";
		break;
	}
	case Type::CLOSING_BRACKET:
	{
		out << "CLOSING_BRACKET";
		break;
	}
	case Type::COMMA:
	{
		out << "COMMA";
		break;
	}
	case Type::FUNCTION_NAME:
	{
		out << "FUNCTION_NAME";
		break;
	}
	case Type::NUMBER:
	{
		out << "NUMBER";
		break;
	}
	default:
		out << "UNKNOWN";
	}
}

Function_Token::Function_Token(const std::string& name)
	: Token(Type::FUNCTION_NAME),
	m_name(name)
{ }

void Function_Token::print(std::ostream& out) const
{
	Token::print(out);
	out << ':' << m_name;
}

Number_Token::Number_Token(const double value)
	: Token(Type::NUMBER),
	m_value(value)
{ }

void Number_Token::print(std::ostream& out) const
{
	Token::print(out);
	out << ':' << m_value;
}

Argument_Token::Argument_Token(const unsigned value)
	: Token(Type::ARGUMENT),
	m_value(value)
{ }

void Argument_Token::print(std::ostream& out) const
{
	Token::print(out);
	out << ':' << m_value;
}

//#################################################
// LEXER
//#################################################

Lexer::Lexer(const std::string& input)
	: m_input(input)
{ }

bool Lexer::make_tokens(std::vector<Token*>& tokens, std::ostream& error_output)
{
	int bracket_count = 0; // Increment if '('. Decrement if ')'. It must not go below 0 and must be 0 at the end.

	// Don't use a for_each because the iterator must be moved by hand is some situations.
	for (std::string::iterator it = m_input.begin(); it != m_input.end(); ++it)
	{
		// Spaces or tabs should have no impact.
		while (*it == ' ' || *it == '\t')
		{
			++it;
		}

		// Again: Names of functions (lists) must contain only letters (no digits allowed!).
		if (is_character(*it))
		{
			std::string name;

			do
			{
				name += *it;
				++it;
			} while (it != m_input.end() && is_character(*it));

			tokens.push_back(new Function_Token(name));
		}
		else if (is_digit(*it) || *it == '-') // Form a double number (can be negative and/or a fraction).
		{
			bool negative = false;

			if (*it == '-')
			{
				negative = true;
				++it;
			}

			double number = 0;

			bool dot = false;

			double power_10 = 1;

			do
			{
				if (*it == '.')
				{
					if (dot == true) // There cannot be two dots in a number. 
					{
						Illegal_Character(std::string() += *it, m_input, it - m_input.begin()).print(error_output);
						tokens.clear();
						return false;
					}
					dot = true;
				}
				else
				{
					number = number * 10 + *it - '0';

					if (dot == true)
					{
						power_10 *= 10;
					}
				}

				++it;
			} while (it != m_input.end() && (is_digit(*it) || *it == '.'));

			tokens.push_back(new Number_Token(negative == true ? (number / power_10) * (-1) : number / power_10));
		}

		if (it == m_input.end())
		{
			break;
		}

		while (*it == ' ' || *it == '\t')
		{
			++it;
		}
		
		switch (*it)
		{
		case '(':
		{
			++bracket_count;
			tokens.push_back(new Token(Type::OPENING_BRACKET));
			break;
		}
		case ')':
		{
			if (bracket_count == 0)
			{
				Illegal_Character(std::string() += *it, m_input, it - m_input.begin()).print(error_output);
				tokens.clear();
				return false;
			}

			--bracket_count;

			tokens.push_back(new Token(Type::CLOSING_BRACKET));
			break;
		}
		case ',':
		{
			tokens.push_back(new Token(Type::COMMA));
			break;
		}
		case '<':
		{
			++it;

			if (*it != '-')
			{
				Illegal_Character(std::string() += *it, m_input, it - m_input.begin()).print(error_output);
				tokens.clear();
				return false;
			}

			tokens.push_back(new Token(Type::ARROW));
			break;
		}
		case '#':
		{
			++it;

			if (it == m_input.end() || !is_digit(*it))
			{
				Illegal_Character(std::string() += *it, m_input, it - m_input.begin()).print(error_output);
				tokens.clear();
				return false;
			}

			unsigned argument = 0; // The argument must be a non-negative integer, i.e. not be a fraction and/or a negative number).

			do
			{
				argument = argument * 10 + *it - '0';
				++it;				
			} while (it != m_input.end() && is_digit(*it));

			tokens.push_back(new Argument_Token(argument));
			
			--it; // So as not to skip the next character.
			break;
		}
		default: // If nothing catches the character then it is not accepted in our language.
		{
			Illegal_Character(std::string() += *it, m_input, it - m_input.begin()).print(error_output);
			tokens.clear();
			return false;
		}
		}
	}

	if (bracket_count > 0)
	{
		Error("Lexical error", "Expected ')'").print(error_output);
		tokens.clear();
		return false;
	}

	return true;
}

//#################################################
// RUN
//#################################################

void run(std::istream& in, std::ostream& out)
{
	std::string input;

	Interpreter i; // It has to be active during the loop so as to store the user declared functions.

	while (true)
	{
		out << "thisfunc > ";

		getline(in, input);

		if (input == "e0")
		{
			break;
		}

		Lexer l(input);

		std::vector<Token*> tokens; // Use an outside vector so as to have no contact between the lexer and the parser.
									// Also, you could print the vector before passing it to the parser (for debug purposes/in another function).
									// Also, also, you could use the vector for other purposes e.g. optimization or additional error checking.

		if (!l.make_tokens(tokens, out))
		{
			continue; // If the input is not acceptable, start the loop all over again.
		}

		Parser p(tokens);

		for (const Token* a : tokens)
		{
			a = nullptr;
		}

		Node* a = p.parse(out);

		if (!a)
		{
			continue; // If the abstract syntax tree is not acceptable, there is not point in interpreting it.
		}

		i.interpret(a, out); // Needn't check for corrections, since nothing happens after the interpretation.
							// If there is an error, just print it and output nothing.

		out << '\n';
	}

	out << "\n\n\n";
}