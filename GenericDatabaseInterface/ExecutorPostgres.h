#pragma once

#include <libpq-fe.h>
#include <stdlib.h>
#include "Query.h"

namespace gdi
{
	class ExecutorPostgres
	{
		PGconn* m_con;
	public:
		ExecutorPostgres(const std::string& host, const std::string& port, const std::string& dbName, const std::string& user, const std::string& password)
		{
			char* pgoptions, * pgtty;
			pgoptions = NULL;
			pgtty = NULL;
			m_con = PQsetdbLogin(host.c_str(), port.c_str(), pgoptions, pgtty, dbName.c_str(), user.c_str(), password.c_str());
		}

		template<class E>
		std::list<E> ExecuteQuery(gdi::Query& query) const
		{
			std::list<E> results;
			auto res = PQexec(m_con, "BEGIN");
			PQclear(res);
			auto queryString = "DECLARE executorCursor CURSOR FOR " + query.GetQuery();
			res = PQexec(m_con, queryString.c_str());
			PQclear(res);
			res = PQexec(m_con, "FETCH ALL in executorCursor");
			for (int i = 0; i < PQntuples(res); i++) 
			{
				E entity;
				auto& attributes = query.GetQueryAttributes();
				auto itAtt = attributes.begin();
				for (size_t j = 0; j < attributes.size(); j++)
				{
					auto key = entity.GetEntityName() + *(itAtt++);
					ToLower(key);
					std::string type = std::get<2>(attributes_[key]);
					auto value = PQgetvalue(res, i, j);
					if (type == "int" || type == "short")
					{
						auto val = atoi(value);
						std::get<0>(attributes_[key])(&val);
					}
					else if (type == "long" || type == "int64")
					{
						auto val = atol(value);
						std::get<0>(attributes_[key])(&val);
					}
					else if (type == "double" || type == "float")
					{
						auto val = atof(value);
						std::get<0>(attributes_[key])(&val);
					}
					else if (type == "bool")
					{
						bool val = value[0] == 't' || value[0] == 'T'
							|| value[0] == 'y' || value[0] == 'Y'
							|| value[0] == '1';
						std::get<0>(attributes_[key])(&val);
					}
					else if (type.find("string") != std::string::npos)
					{
						auto val = std::string(value);
						std::get<0>(attributes_[key])(&val);
					}
				}
				results.push_back(entity);
			}
			PQclear(res);
			res = PQexec(m_con, "CLOSE executorCursor");
			PQclear(res);
			res = PQexec(m_con, "END");
			PQclear(res);
			return results;
		}

		void Execute(const gdi::IQuery& query) const
		{
			auto res = PQexec(m_con, query.GetQuery().c_str());
		}

		~ExecutorPostgres()
		{
			PQfinish(m_con);
		}

	};
}
