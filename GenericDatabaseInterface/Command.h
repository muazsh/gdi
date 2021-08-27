#pragma once
#include "Query.h"
#include "QueryCreate.h"

namespace gdi
{
	QueryCreate Create()
	{
		return QueryCreate(std::string("CREATE "));
	}

	QueryCreate Drop()
	{
		return QueryCreate(std::string("DROP "));
	}

	Query SelectOfList(const std::list<std::string>& columns)
	{
		Query query;
		std::string queryString = "";
		for (auto& x : columns) {
			query.AddAttribute(x);
			queryString += x + std::string(",");
		}
		queryString = std::string("SELECT ") + queryString;
		queryString.pop_back();
		query.SetQuery(queryString);
		return query;
	}

	template<class...Args>
	Query Select(Args... args)
	{
		return SelectOfList({args...});
	}

	Query InsertIntoOfList(const std::string& table, std::list<std::string>& columns)
	{
		Query query;
		std::string queryString = "(";
		for (auto& x : columns) {
			query.AddAttribute(x);
			queryString += x + std::string(",");
		}
		queryString.pop_back();
		queryString += ")";
		queryString = std::string("INSERT INTO ") + table + queryString;
		query.SetQuery(queryString);
		return query;
	}

	template<class...Args>
	Query InsertInto(const std::string& table, Args... args)
	{
		return InsertIntoOfList(table, std::list<std::string>({args...}));
	}
	
	Query Update(const std::string& table)
	{
		return Query(std::string("UPDATE ") + table);
	}

	Query Delete()
	{
		return Query(std::string("DELETE "));
	}
}
