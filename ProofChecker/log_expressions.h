#include "expressions.h"

class log_expression
{
};

class forall_expression : public log_expression
{
	base_type bt;
	log_expression *p_exp;

public:
	forall_expression(log_expression *p_exp) : p_exp(p_exp) { }
};

class exists_expression : public log_expression
{
	base_type bt;
	log_expression *p_exp;

public:
	exists_expression(log_expression *p_exp) : p_exp(p_exp) { }
};

//class exists1_expression : public log_expression
//{
//	log_expression *p_exp;
//
//public:
//	exists1_expression(log_expression *p_exp) : p_exp(p_exp) { }
//};

class and_expression : public log_expression
{
	log_expression *left, *right;
	type *p_value_type;

public:
};

class or_expression : public log_expression
{
	log_expression *left, *right;

public:
};

class neg_expression : public log_expression
{
	log_expression *p_exp;

public:
};

class equal_expression : public log_expression
{
	expression *left, *right;

public:
	equal_expression(expression *left, expression *right) : left(left), right(right) { }
};

class subset_expression : public log_expression
{
	expression *left, *right;

public:
	subset_expression(expression *left, expression *right) : left(left), right(right) { }
};