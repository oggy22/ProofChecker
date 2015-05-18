#include <vector>

class type
{
public:
	virtual bool is_same(type* p_type) { return false; }
	virtual bool is_function() { return false; }
	virtual bool is_binary_operation() { return false; }
	virtual bool is_binary_relation() { return false; }
	virtual wstring to_string() const = 0;
};

// This class represents either a set or is just a slot to store a pointer to the type
class base_type : public type
{
	// If nullptr this class represents a set
	type* ptr;

public:
	base_type(type* ptr = nullptr) : ptr(ptr) { }

	virtual bool is_same(type* p_type)
	{
		// This method should be called 
		_ASSERT(dynamic_cast<base_type*>(p_type) != nullptr);
		_ASSERT(ptr == nullptr);

		// They represent the same type only if this is the same object.
		return this == ptr;
	}

	virtual wstring to_string() const
	{
		return wstring(L"id");
	}

	type* get_type() const
	{
		return ptr;
	}

	void set_type(type* t)
	{
		ptr = t;
	}
};

class cartesian_product_type : public type
{
	type *p_set1, *p_set2;

public:
	cartesian_product_type(type *p_set1, type *p_set2) : p_set1(p_set1), p_set2(p_set2) { }
	virtual bool is_same(type* p_type)
	{
		cartesian_product_type *ptr = dynamic_cast<cartesian_product_type*>(p_type);
		if (ptr == nullptr)
			return false;

		return p_set1->is_same(ptr->p_set1) && p_set2->is_same(ptr->p_set2);
	}

	virtual wstring to_string() const
	{
		return p_set1->to_string() + L'x' + p_set2->to_string();
	}
	type *get1() { return p_set1; }
	type *get2() { return p_set2; }
};

class power_set_type : public type
{
	type *p_base;

public:
	power_set_type(type *p_base) : p_base(p_base)
	{ }

	virtual bool is_same(type* p_type)
	{
		power_set_type* ptr = dynamic_cast<power_set_type*>(p_type);
		if (ptr == nullptr)
			return false;

		return p_base->is_same(ptr->p_base);
	}

	virtual bool is_binary_relation(type* p_type)
	{
		power_set_type* p_power = dynamic_cast<power_set_type*>(p_type);
		if (!p_power)
			return false;

		//cartesian_product_type
		cartesian_product_type* p_decart = dynamic_cast<cartesian_product_type*>(p_power);
		if (!p_decart)
			return false;

		return true;
	}

	virtual wstring to_string() const
	{
		return L"P(" + p_base->to_string() + L")";
	}
};

class function_type : public type
{
public:
	type * const p_from;
	type * const p_to;

	function_type(type *from, type *to) : p_from(from), p_to(to) { }

	virtual bool is_same(type* p_type)
	{
		function_type* ptr = dynamic_cast<function_type*>(p_type);
		if (ptr == nullptr)
			return false;

		return p_from->is_same(ptr->p_from) && p_to->is_same(ptr->p_to);
	}

	virtual bool is_function()
	{
		return true;
	}

	virtual bool is_binary_operation()
	{
		cartesian_product_type *ptr = dynamic_cast<cartesian_product_type*>(p_from);
		if (ptr == nullptr)
			return false;

		return p_to->is_same(ptr->get1()) && p_to->is_same(ptr->get2());
	}

	virtual wstring to_string() const
	{
		return p_to->to_string() + L"^" + p_from->to_string();
	}
};

class tuple_type : public type
{
};

class log_expression;

class theory : public tuple_type
{
	base_type this_type;
	std::vector<base_type> vars;

public:
	std::string name;
	std::vector<log_expression*> axioms;
	std::vector<log_expression*> theorems;

	int size() const
	{
		return vars.size();
	}

	theory(int arity, std::string name) : vars(arity), name(name) {}

	base_type& operator[](int i)
	{
		return vars[i];
	}

	virtual wstring to_string() const
	{
		std::wstringstream ss;
		std::for_each(vars.cbegin(), vars.cend(), [&ss](const base_type &bt)
		{
			auto ws = bt.get_type()->to_string();
			ss << ws << L",";
		});
		return ss.str();
	}
};