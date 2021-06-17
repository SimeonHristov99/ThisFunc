#pragma once

#include "Lexer.h"

//#################################################
// NODES
//#################################################

/// A node for every (accepted) token. Print functions are optional.
struct Node /// Struct because it doesn't do anything special - just stores.
{
	const Token* m_token; /// There is not point in having it non-const.
	
	/// Since this is a polymorphic base class, when creating a new Node, copy not the address of the pointer
	/// but the value of the pointer, hence - the big 4.
	void copy(const Node& rhs);
	void del();

	explicit Node(const Token* token);
	Node(const Node& rhs);
	Node& operator=(const Node& rhs);
	virtual ~Node();

	virtual void print(std::ostream& out) const;

	virtual Node* clone() const; /// Copy the value in the pointer, not the pointer itself.
};

struct Factor_Node :public Node
{
	explicit Factor_Node(const Token* token);
	Factor_Node(const Factor_Node& rhs);
	Factor_Node& operator=(const Factor_Node& rhs);

	Factor_Node* clone() const override;
};

struct Argument_Node :public Node
{
	explicit Argument_Node(const Token* token);
	Argument_Node(const Argument_Node& rhs);
	Argument_Node& operator=(const Argument_Node& rhs);

	Argument_Node* clone() const override;
};

struct Unary_Operation_Node :public Node
{
	const Node* m_argument;

	Unary_Operation_Node(const Token* token, const Node* a);
	Unary_Operation_Node(const Unary_Operation_Node& rhs);
	Unary_Operation_Node& operator=(const Unary_Operation_Node& rhs);
	~Unary_Operation_Node();

	void print(std::ostream& out) const override;

	Unary_Operation_Node* clone() const override;
};

struct Binary_Operation_Node :public Node
{
	const Node* m_left;
	const Node* m_right;

	Binary_Operation_Node(const Token* token, const Node* left, const Node* right);
	Binary_Operation_Node(const Binary_Operation_Node& rhs);
	Binary_Operation_Node& operator=(const Binary_Operation_Node& rhs);
	~Binary_Operation_Node();

	void print(std::ostream& out) const override;

	Binary_Operation_Node* clone() const override;
};

struct If_Opeation_Node :public Node
{
	const Node* m_check;
	const Node* m_left;
	const Node* m_right;

	void copy(const If_Opeation_Node& rhs);
	void del();

	If_Opeation_Node(const Token* token, const Node* check, const Node* left, const Node* right);
	If_Opeation_Node(const If_Opeation_Node& rhs);
	If_Opeation_Node& operator=(const If_Opeation_Node& rhs);
	~If_Opeation_Node();

	void print(std::ostream& out) const override;

	If_Opeation_Node* clone() const override;
};

struct List_Operation_Node :public Node
{
	std::vector<Node*> m_contents; /// Can be superseded with a queue.

	List_Operation_Node(const Token* token, const std::vector<Node*> contents);
	~List_Operation_Node();

	void print(std::ostream& out) const override;
};

struct Map_Operation_Node :public Node
{
	const Node* m_functor;
	const Node* m_list;

	Map_Operation_Node(const Token* token, const Node* functor, const Node* list);
	~Map_Operation_Node();

	void print(std::ostream& out) const override;
};

struct User_Function :public Node
{
	const Node* m_definition;
	std::vector<const Node*> m_arguments;

	void copy(const User_Function& rhs);
	void del();

	User_Function(const Token* token, const Node* definition, const std::vector<const Node*>& arguments);
	User_Function(const User_Function& rhs);
	User_Function& operator=(const User_Function& rhs);
	~User_Function();

	void print(std::ostream& out) const override;

	User_Function* clone() const override;
};

//#################################################
// PARSER
//#################################################

class Parser
{
private:
	/// Unlike the lexer, store the vector.
	std::vector<Token*> m_tokens;

	/// Store the current_type and the current_index because you'll need them a lot.
	Type m_current_type;
	int m_current_index;

	/// Increment the index and get if it is less than the size of the vector get the type.
	void advance();

	/// Returns a factor node if the index is valid and nullptr otherwise.
	Node* factor(std::ostream& out);
	/// An expression is a collection of terms which are function names and factors.
	Node* expr(std::ostream& out);

public:
	/// Copies the vector, sets the index to -1 and calls advance.
	explicit Parser(const std::vector<Token*>& tokens);
	Parser(const Parser& rhs) = delete;
	Parser& operator=(const Parser& rhs) = delete;
	~Parser();

	/// If the index is -1 returns nullptr. Returns expr() otherwise.
	/// Also because of the error checking the ostream is going to have to be passed everywhere.
	Node* parse(std::ostream& out);
};