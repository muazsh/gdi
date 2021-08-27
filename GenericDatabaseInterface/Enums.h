#pragma once
namespace gdi
{
	enum class WhereCondition { 
		EQUAL, 
		GREATER_THAN, 
		GREATER_THAN_OR_EQUAL, 
		LESS_THAN, 
		LESS_THAN_OR_EQUAL, 
		NOT_EQUAL, 
		LIKE, 
		IN 
	};

	enum class PrimitiveType {
		INT,
		FLOAT,
		DOUBLE,
		STRING,
	};
}
