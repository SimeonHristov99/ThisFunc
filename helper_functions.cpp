#include "helper_functions.h"

bool is_character(const char c)
{
	return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
}

bool is_digit(const char c)
{
	return c >= '0' && c <= '9';
}
