#pragma once
#include <string>
#include "Entity.h"
#include "IQuery.h"

namespace gdi
{
	class QueryCreate : public IQuery
	{
		std::string m_query;
		std::string GetType(const std::string& cType, size_t length)
		{
			std::string sqlType = "";
			if (cType == "int" || cType == "short" || cType == "long" || cType == "int64") sqlType = "INT";
			else if (cType == "float") sqlType = "FLOAT";
			else if (cType == "double") sqlType = "REAL";
			else if (cType == "bool") sqlType = "BOOLEAN";
			else if (cType.find("string") != std::string::npos)
			{
				if(length == 0)
					sqlType = "TEXT";
				else
					sqlType = "VARCHAR(" + std::to_string(length) + ")";
			}
			return sqlType;
		}
	public:
		QueryCreate() = default;
		explicit QueryCreate(const std::string & query) :m_query(query) {}
		std::string GetQuery() const { return m_query; }
		QueryCreate& Database(const std::string& name) { m_query += (" DATABASE " + name); return *this; }
		QueryCreate& TableDrop(const std::string& name) { m_query += (" TABLE " + name); return *this;}
		template<class E>
		QueryCreate& Table(E& entity)
		{
			m_query += (" TABLE " + entity.GetEntityName() + "(");
			E localEntity;
			localEntity = entity;
			size_t entityNameLength = 0;
			entityNameLength = localEntity.GetEntityName().length();
			for (auto& att : attributes_)
			{
				m_query += att.first.substr(entityNameLength) + " " + GetType(std::get<2>(att.second), std::get<3>(att.second)) + ",";
			}

			auto primary = localEntity.GetPrimary();
			if (primary.size() != 0)
			{
				m_query += " PRIMARY KEY (";
				for (auto col = primary.begin(); col != primary.end(); col++)
					m_query += *col + ",";
				m_query.pop_back();
				m_query += ") ";
			}
			m_query.pop_back();
			m_query += ")";
			return *this;
		}
	};
}
