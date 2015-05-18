struct expression
{
public:
	virtual type *get_type() const = 0;
};

class function_application : public expression
{
public:
	const base_type * const  p_function;
	const expression * const p_argument;

	function_application(base_type *p_function, expression *p_argument) : p_function(p_function), p_argument(p_argument)
	{
		if (get_type() != p_argument->get_type())
		{ }
	}

	virtual type *get_type() const
	{
		function_type *p_f = dynamic_cast<function_type*>(p_function->get_type());
		return p_f->p_to;
	}
};

class identifier_expression : public expression
{
	base_type *p_identifier;

public:
	identifier_expression(base_type *p_identifier) : p_identifier(p_identifier)
	{}

	virtual type *get_type() const
	{
		return p_identifier->get_type();
	}
};

class binary_operation_application : public expression
{
public:
	const expression * const p_arg1;
	const expression * const p_arg2;

	binary_operation_application(expression *p_arg1, expression *p_arg2) : p_arg1(p_arg1), p_arg2(p_arg2)
	{ }
};