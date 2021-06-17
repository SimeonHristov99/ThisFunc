#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "helper_functions.h"

///#################################################
/// ERRORS
///#################################################

struct Error
{
	std::string m_name; /// Three different types of errors: "Illegal Character" from the Lexer, "Illegal Syntax" from the Parser and "Runtime" from the Interpreter
	std::string m_details;

	Error(const std::string& name, const std::string& details);
	virtual ~Error() = default;

	virtual void print(std::ostream& out) const;
};

/// Takes the input and the index of the wrong character and "points" to it.
class Illegal_Character :public Error
{
private:
	std::string m_input;
	unsigned m_index;

public:
	/// Passes /// Passes the string "Illegal Character" to the base class.
	Illegal_Character(const std::string& c, const std::string& input, const unsigned index);

	void print(std::ostream& out) const override;
};

/// Passes the string "Illegal Syntax" to the base class.
struct Illegal_Syntax :public Error
{
	Illegal_Syntax(const std::string& details);
};

struct Runtime_Error :public Error
{
	Runtime_Error(const std::string& details);
};

//#################################################
// TOKENS
//#################################################

/// All the valid types.
enum class Type
{
	FUNCTION_NAME,

	ARROW,

	NUMBER,
	ARGUMENT,

	OPENING_BRACKET,
	COMMA,
	CLOSING_BRACKET,
};

/// A struct for every accepted type.
struct Token
{
	Type m_type;

	explicit Token(const Type type);

	/// Debug function.
	virtual void print(std::ostream& out) const;
};

struct Function_Token :public Token
{
	std::string m_name; /// COMPRISED ONLY OF LETTERS (no other symbols - digits, _, etc. are allowed!)

	explicit Function_Token(const std::string& name);

	/// Debug function.
	void print(std::ostream& out) const override;
};

struct Number_Token :public Token
{
	double m_value;

	explicit Number_Token(const double value);

	/// Debug function.
	void print(std::ostream& out) const override;
};

struct Argument_Token :public Token
{
	unsigned m_value;

	explicit Argument_Token(const unsigned value);

	/// Debug function.
	void print(std::ostream& out) const override;
};

//#################################################
// LEXER
//#################################################

/// Go through the input character by character
/// and fill in a vector with tokens
/// if there is an unaccepted symbol, dump the vector and return error
class Lexer
{
private:
	std::string m_input; /// Hold the input in a string for easier work.

public:
	explicit Lexer(const std::string& input);

	Lexer(const Lexer& rhs) = delete;
	Lexer& operator=(const Lexer& rhs) = delete;

	/// Use vector because of its constant access time.
	bool make_tokens(std::vector<Token*>& tokens, std::ostream& error_output);
};

//#################################################
// RUN
//#################################################

/// The main function that does uses all the classes.
void run(std::istream& in, std::ostream& out);