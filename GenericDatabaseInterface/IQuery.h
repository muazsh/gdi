#pragma once
#include <string>
namespace gdi
{
	class IQuery
	{
	public:
		virtual std::string GetQuery() const = 0;
	};
}