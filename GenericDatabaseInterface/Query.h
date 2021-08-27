#pragma once
#include <list>
#include <string>
#include "Enums.h"
#include "IQuery.h"
namespace gdi
{
	class Query : public IQuery
	{
		std::list<std::string> m_queryAttributes;
		std::string m_query;

		void SetCondition(const std::string& columnsName, const WhereCondition cond, const std::string& rightOperand)
		{
			m_query += cond == WhereCondition::NOT_EQUAL ? std::string(" NOT ") + columnsName : columnsName;
			switch (cond)
			{
			case WhereCondition::NOT_EQUAL:
			case WhereCondition::EQUAL: m_query += std::string(" = "); break;
			case WhereCondition::GREATER_THAN: m_query += std::string(" > "); break;
			case WhereCondition::GREATER_THAN_OR_EQUAL: m_query += std::string(" >= "); break;
			case WhereCondition::LESS_THAN: m_query += std::string(" < "); break;
			case WhereCondition::LESS_THAN_OR_EQUAL: m_query += std::string(" <= "); break;
			case WhereCondition::LIKE: m_query += std::string(" LIKE "); break;
			case WhereCondition::IN: m_query += std::string(" IN "); break;
			}
			m_query += rightOperand;
		}

	public:
		Query() = default;
		explicit Query(const std::string& query) :m_query(query) {}
		void SetQuery(const std::string& query) { m_query = query; }
		const std::list<std::string>& GetQueryAttributes() { return m_queryAttributes; }
		std::string GetQuery() const { return m_query; }
		void AddAttribute(const std::string& attribute) { m_queryAttributes.push_back(attribute); }

		Query& Set(const std::list<std::string>& columns, const std::list<std::string>& values)
		{
			m_query += std::string(" SET ");
			for (auto itCol = columns.begin(), itVal = values.begin(); itCol != columns.end() && itVal != values.end(); itCol++, itVal++)
			{
				m_query += *itCol + " = " + *itVal + ",";
			}
			m_query.pop_back();
			return *this;
		}

		Query& ValuesList(const std::list<std::string>& values)
		{
			m_query += std::string(" VALUES(");
			for (auto& x : values) {
				m_query += x + std::string(",");
			}
			m_query.pop_back();
			m_query += std::string(")");
			return *this;
		}
		template<class...Args>
		Query& Values(Args... args)
		{
			return ValuesList({ args... });
		}

		template<class...Args>
		Query& From(Args... args)
		{
			m_query += std::string(" FROM ");
			for (auto&& x : { args... }) {
				m_query += x + std::string(",");
			}
			m_query.pop_back();
			return *this;
		}
	
		Query& Where(const std::string& columnsName, const WhereCondition cond, const std::string& rightOperand)
		{
			m_query += std::string(" WHERE ");
			SetCondition(columnsName, cond, rightOperand);
			return *this;
		}

		Query& And(const std::string& columnsName, const WhereCondition cond, const std::string& rightOperand)
		{
			m_query += std::string(" AND ");
			SetCondition(columnsName, cond, rightOperand);
			return *this;
		}

		Query& Or(const std::string& columnsName, const WhereCondition cond, const std::string& rightOperand)
		{
			m_query += std::string(" OR ");
			SetCondition(columnsName, cond, rightOperand);
			return *this;
		}

		Query& Distinct()
		{
			m_query += std::string(" DISTINCT ");
			return *this;
		}

		template<class...Args>
		Query& OrderBy(Args... args)
		{
			m_query += std::string(" ORDER BY ");
			for (auto&& x : { args... }) {
				m_query += x + std::string(",");
			}

			m_query.pop_back();
			return *this;
		}

		template<class...Args>
		Query& GroupBy(Args... args)
		{
			m_query += std::string(" GROUP BY ");
			for (auto&& x : { args... }) {
				m_query += x + std::string(",");
			}

			m_query.pop_back();
			return *this;
		}

		Query& Having(const std::string& columnsName, const WhereCondition cond, const std::string& rightOperand)
		{
			m_query += std::string(" HAVING ");
			SetCondition(columnsName, cond, rightOperand);
			return *this;
		}

		Query& Count(const std::string& columnsName)
		{
			if (!IsEmptyOrWhitespaces(m_query))
				m_query += ",";
			m_query += std::string(" COUNT(" + columnsName + ") ");
			return *this;
		}

		Query& Avg(const std::string& columnsName)
		{
			if (!IsEmptyOrWhitespaces(m_query))
				m_query += ",";
			m_query += std::string(" AVG(" + columnsName + ") ");
			return *this;
		}

		Query& Sum(const std::string& columnsName)
		{
			if (!IsEmptyOrWhitespaces(m_query))
				m_query += ",";
			m_query += std::string(" SUM(" + columnsName + ") ");
			return *this;
		}

		Query& Max(const std::string& columnsName)
		{
			if (!IsEmptyOrWhitespaces(m_query))
				m_query += ",";
			m_query += std::string(" MAX(" + columnsName + ") ");
			return *this;
		}

		Query& Min(const std::string& columnsName)
		{
			if (!IsEmptyOrWhitespaces(m_query))
				m_query += ",";
			m_query += std::string(" MIN(" + columnsName + ") ");
			return *this;
		}

		Query& InnerJoin(const std::string& table)
		{
			m_query += std::string(" INNER JOIN ") + table;
			return *this;
		}

		Query& LeftJoin(const std::string& table)
		{
			m_query += std::string(" LEFT JOIN ") + table;
			return *this;
		}

		Query& RightJoin(const std::string& table)
		{
			m_query += std::string(" RIGHT JOIN ") + table;
			return *this;
		}

		Query& FullJoin(const std::string& table)
		{
			m_query += std::string(" FULL JOIN ") + table;
			return *this;
		}

		Query& On(const std::string& col1, WhereCondition cond, const std::string& col2)
		{
			m_query += std::string(" ON ");
			SetCondition(col1, cond, col2);
			return *this;
		}
	};
}
