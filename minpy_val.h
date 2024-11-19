#ifndef h_minpy_val
#define h_minpy_val

#include <math.h>
#include <cstdlib>
#include <string>
#include <iostream>

#define VALUE_CHECK_RELATION(op) e_vtype a = GetType();		\
	e_vtype b = val.GetType();								\
	if(a != b) return Value(false);							\
	switch(a) {												\
		case VALUE_TYPE_BOOLEAN:							\
			return Value(GetBool() op val.GetBool());		\
		case VALUE_TYPE_STRING:								\
			return Value(GetString() op val.GetString());	\
		case VALUE_TYPE_CHARACTER:							\
			return Value(GetChar() op val.GetChar());		\
		case VALUE_TYPE_INTEGER:							\
			return Value(GetInteger() op val.GetInteger());	\
		case VALUE_TYPE_REAL:								\
			return Value(GetReal() op val.GetReal());		\
	}

typedef enum _vtype {
	VALUE_TYPE_INTEGER   = 0x0,
	VALUE_TYPE_REAL      = 0x1,
	VALUE_TYPE_STRING    = 0x2,
	VALUE_TYPE_CHARACTER = 0x4,
	VALUE_TYPE_BOOLEAN   = 0x8,
	VALUE_TYPE_ERROR     = 0x10
} e_vtype;

class Value {
	e_vtype vtype;
	void* data;

	void setBool(bool b){ 
		vtype = VALUE_TYPE_BOOLEAN;
		data = malloc(sizeof(bool));
		*((bool*)data) = b;
	}
	void setInteger(int i) {
		vtype = VALUE_TYPE_INTEGER;
		data = malloc(sizeof(int));
		*((int*)data) = i;
	}
	void setReal(double r) {
		vtype = VALUE_TYPE_REAL;
		data = malloc(sizeof(double));
		*((double*)data) = r;
	}
	void setString(char const * cstr) {
		vtype = VALUE_TYPE_STRING;
		data = malloc(sizeof(std::string));
		if(cstr != "") {
			*((std::string*)data) = cstr;
		}
	}
	void setChar(char c) {
		vtype = VALUE_TYPE_CHARACTER;
		data = malloc(sizeof(char));
		*((char*)data) = c;
	}

public:
	/**
	 *	Value Constructors
	 */
	Value() : vtype(VALUE_TYPE_ERROR), data(nullptr) {}
	Value(bool b)            { setBool(b);      }
	Value(int i)             { setInteger(i);   }
	Value(double r)          { setReal(r);      }
	Value(char const * cstr) { setString(cstr); }
	Value(char c)            { setChar(c);      }
	Value(const Value & val) {
		switch(val.GetType()) {
			case VALUE_TYPE_BOOLEAN:
				setBool(val.GetBool());
				break;
			case VALUE_TYPE_INTEGER:
				setInteger(val.GetInteger());
				break;
			case VALUE_TYPE_REAL:
				setReal(val.GetReal());
				break;
			case VALUE_TYPE_STRING:
				setString(val.GetString().c_str());
				break;
			case VALUE_TYPE_CHARACTER:
				setChar(val.GetChar());
				break;
			default:
				data = nullptr;
		}
	}

	~Value() {
	}
	
	/**
	 *	VType Tests
	 */
	bool IsInteger() const { return vtype == VALUE_TYPE_INTEGER;   }
	bool IsReal()    const { return vtype == VALUE_TYPE_REAL;      }
	bool IsString()  const { return vtype == VALUE_TYPE_STRING;    }
	bool IsChar() 	 const { return vtype == VALUE_TYPE_CHARACTER; }
	bool IsBool() 	 const { return vtype == VALUE_TYPE_BOOLEAN;   }
	bool IsError()   const { return vtype == VALUE_TYPE_ERROR;     }
	e_vtype GetType() const { return vtype; }

	/**
	 *	Value Getters
	 */
	int GetInteger() const { 
		if (!IsInteger())
			throw std::runtime_error("Value is not an integer");
		return *((int*)data);
	}
	double GetReal() const { 
		if (!IsReal())
			throw std::runtime_error("Value is not a real");
		return *((double*)data);
	}
	std::string GetString() const { 
		if (!IsString())
			throw std::runtime_error("Value is not a string");
		return *((std::string*)data);
	}
	char GetChar() const { 
		if (!IsChar())
			throw std::runtime_error("Value is not a char");
		return *((char*)data);
	}
	bool GetBool() const { 
		if (!IsBool())
			throw std::runtime_error("Value is not a bool");
		return *((bool*)data);
	}
	
	Value floor() {
		switch(GetType()) {
			case VALUE_TYPE_INTEGER:
				return Value(GetInteger());
			case VALUE_TYPE_REAL:
				return Value((int) GetReal());
		}
		return Value();
	}

	Value sqrt() {
		switch(GetType()) {
			case VALUE_TYPE_INTEGER:
				return Value(sqrtf64((double)GetInteger()));
			case VALUE_TYPE_REAL:
				return Value(sqrtf64(GetReal()));
		}
		return Value();
	}

	Value pow(Value const & other) {
		switch(GetType()) {
			case VALUE_TYPE_BOOLEAN:
				switch(other.GetType()) {
					case VALUE_TYPE_BOOLEAN:
						return Value((int) powl((int) GetBool(), (int) other.GetBool()));
					case VALUE_TYPE_INTEGER:
						return Value((int) powl((int) GetBool(), other.GetInteger()));
					case VALUE_TYPE_REAL:
						return Value((double) powf64((double) GetBool(), other.GetReal()));
				}
				break;
			case VALUE_TYPE_INTEGER:
				switch(other.GetType()) {
					case VALUE_TYPE_BOOLEAN:
						return Value((int) powl((int) GetInteger(), (int) other.GetBool()));
					case VALUE_TYPE_INTEGER:
						return Value((int) powl((int) GetInteger(), other.GetInteger()));
					case VALUE_TYPE_REAL:
						return Value((double) powf64((double) GetInteger(), other.GetReal()));
				}
				break;
			case VALUE_TYPE_REAL:
				switch(other.GetType()) {
					case VALUE_TYPE_BOOLEAN:
						return Value((double) powf64(GetReal(), (double) other.GetBool()));
					case VALUE_TYPE_INTEGER:
						return Value((double) powf64(GetReal(), (double) other.GetInteger()));
					case VALUE_TYPE_REAL:
						return Value((double) powf64(GetReal(), other.GetReal()));
				}
				break;
		}
		return Value();
	}

	/**
	 *	Overloaded operators
	 */
	Value operator!() {
		e_vtype a = GetType();
		switch(a) {
			case VALUE_TYPE_BOOLEAN:
				return Value(!GetBool());
			case VALUE_TYPE_CHARACTER:
				return Value(GetChar() != (char)0);
			case VALUE_TYPE_INTEGER:
				return Value(GetInteger() != 0);
			case VALUE_TYPE_REAL:
				return Value(GetReal() != 0.0);
			case VALUE_TYPE_STRING:
				return Value(GetString().length() != 0);
		}
		return Value();
	}
	Value operator+() {
		switch(GetType()) {
			case VALUE_TYPE_BOOLEAN:
				return Value(+(int)GetBool());
			case VALUE_TYPE_REAL:
				return Value(+GetReal());
			case VALUE_TYPE_INTEGER:
				return Value(+GetInteger());
		}
		return Value();
	}
	Value operator-() {
		switch(GetType()) {
			case VALUE_TYPE_BOOLEAN:
				return Value(-(int)GetBool());
			case VALUE_TYPE_REAL:
				return Value(-GetReal());
			case VALUE_TYPE_INTEGER:
				return Value(-GetInteger());
		}
		return Value();
	}
	Value operator+(const Value & val) {
		e_vtype a = GetType();
		e_vtype b = val.GetType();
		switch(a) {
			case VALUE_TYPE_BOOLEAN:
				switch(b) {
					case VALUE_TYPE_BOOLEAN:
						return Value((int) GetBool() + (int) val.GetBool());
					case VALUE_TYPE_INTEGER:
						return Value((int) GetBool() + val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value((int) GetBool() + val.GetReal());
				}
			case VALUE_TYPE_STRING:
				switch(b) {
					case VALUE_TYPE_STRING:
						return Value((GetString() + val.GetString()).c_str());
					case VALUE_TYPE_CHARACTER:
						return Value((GetString() + val.GetChar()).c_str());
				}
				break;
			case VALUE_TYPE_CHARACTER:
				switch(b) {
					case VALUE_TYPE_STRING:
						return Value((GetChar() + val.GetString()).c_str());
					case VALUE_TYPE_CHARACTER:
						return Value(GetChar() + val.GetChar());
				}
				break;
			case VALUE_TYPE_INTEGER:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetInteger() + val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetInteger() + val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetInteger() + (int)val.GetBool());
				}
				break;
			case VALUE_TYPE_REAL:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetReal() + val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetReal() + val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetReal() + (int)val.GetBool());
				}
				break;
		}
		return Value();
	}
	Value operator-(const Value & val) {
		e_vtype b = val.GetType();
		switch(b) {
			case VALUE_TYPE_INTEGER:
			case VALUE_TYPE_REAL:
			case VALUE_TYPE_BOOLEAN:
				Value aVal = *this;
				Value bVal = val;
				return aVal + (-bVal);
		}
		return Value();
	}
	Value operator*(const Value & val) {
		e_vtype a = GetType();
		e_vtype b = val.GetType();
		switch(a) {
			case VALUE_TYPE_INTEGER:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetInteger() * val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetInteger() * val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetInteger() * (int)val.GetBool());
				}
				break;
			case VALUE_TYPE_REAL:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetReal() * val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetReal() * val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetReal() * (int)val.GetBool());
				}
				break;
		}
		return Value();
	}
	Value operator/(const Value & val) {
		e_vtype a = GetType();
		e_vtype b = val.GetType();
		switch(a) {
			case VALUE_TYPE_INTEGER:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetInteger() / val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetInteger() / val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetInteger() / (int)val.GetBool());
				}
				break;
			case VALUE_TYPE_REAL:
				switch(b) {
					case VALUE_TYPE_INTEGER:
						return Value(GetReal() / val.GetInteger());
					case VALUE_TYPE_REAL:
						return Value(GetReal() / val.GetReal());
					case VALUE_TYPE_BOOLEAN:
						return Value(GetReal() / (int)val.GetBool());
				}
				break;
		}
		return Value();
	}
	Value operator==(const Value & val) {
		VALUE_CHECK_RELATION(==)
		return Value();
	}
	Value operator>=(const Value & val) {
		VALUE_CHECK_RELATION(>=)
		return Value();
	}
	Value operator<=(const Value & val) {
		VALUE_CHECK_RELATION(<=)
		return Value();
	}
	Value operator<(const Value & val) {
		VALUE_CHECK_RELATION(<)
		return Value();
	}
	Value operator>(const Value & val) {
		VALUE_CHECK_RELATION(>)
		return Value();
	}
	Value operator&&(const Value & val) {
		// "Pythonic" semantic for conjunction
		e_vtype a = GetType();
		e_vtype b = val.GetType();
		if(a == VALUE_TYPE_ERROR || b == VALUE_TYPE_ERROR) return Value();
		switch(a) {
			case VALUE_TYPE_BOOLEAN:
				if(!GetBool()) return Value(false);
				break;
			case VALUE_TYPE_CHARACTER:
				if(!GetChar()) return Value(false);
				break;
			case VALUE_TYPE_INTEGER:
				if(!GetInteger()) return Value(false);
				break;
			case VALUE_TYPE_REAL:
				if(!GetReal()) return Value(false);
				break;
			case VALUE_TYPE_STRING:
				if(GetString().length() == 0) return Value(false);
				break;
		}
		return Value(val);
	}
	Value operator||(const Value & val) {
		// "Pythonic" semantic for disjunction
		e_vtype a = GetType();
		e_vtype b = val.GetType();
		if(a == VALUE_TYPE_ERROR || b == VALUE_TYPE_ERROR) return Value();
		switch(a) {
			case VALUE_TYPE_BOOLEAN:
				if(GetBool()) return Value(true);
				break;
			case VALUE_TYPE_CHARACTER:
				if(GetChar()) return Value(GetChar());
				break;
			case VALUE_TYPE_INTEGER:
				if(GetInteger()) return Value(GetInteger());
				break;
			case VALUE_TYPE_REAL:
				if(GetReal()) return Value(GetReal());
				break;
			case VALUE_TYPE_STRING:
				if(GetString().length() != 0) return Value(GetString().c_str());
				break;
		}
		return Value(val);
	}


	friend std::ostream& operator<<(std::ostream& out, const Value& val) {
		e_vtype vtype = val.GetType();
		switch(vtype) {
			case VALUE_TYPE_ERROR:
				out << "ERROR";
				break;
			case VALUE_TYPE_BOOLEAN:
				out << (val.GetBool() ? "True" : "False");
				break;
			case VALUE_TYPE_INTEGER:
				out << val.GetInteger();
				break;
			case VALUE_TYPE_REAL:
				out << val.GetReal();
				break;
			case VALUE_TYPE_CHARACTER:
				out << val.GetChar();
				break;
			case VALUE_TYPE_STRING:
				out << val.GetString();
				break;
		}
		return out;
	}
};

#endif