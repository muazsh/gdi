#pragma once
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "Query.h"

namespace gdi
{	
	class ExecutorMySQL
	{
		sql::Driver* m_driver;
		sql::Connection* m_con;
	public:
		ExecutorMySQL(const std::string& host, const std::string& user, const std::string& password)
		{
			m_driver = get_driver_instance();
			m_con = m_driver->connect(host, user, password);
		}

		void SetSchema(const std::string& database)
		{
			m_con->setSchema(database);
		}

		template<class E>
		std::list<E> ExecuteQuery(gdi::Query& query) const
		{
				std::list<E> results;
				auto stmt = m_con->createStatement();
				auto queryRes = stmt->executeQuery(query.GetQuery());
				while (queryRes->next()) {
					E entity;
					for (auto att : query.GetQueryAttributes())
					{
						auto key = entity.GetEntityName() + att;
						ToLower(key);
						std::string type = std::get<2>(attributes_[key]);
						if (type == "int" || type == "short")
						{
							auto value = queryRes->getInt(att);
							std::get<0>(attributes_[key])(&value);
						}
						else if (type == "long" || type == "int64")
						{
							auto value = queryRes->getInt64(att);
							std::get<0>(attributes_[key])(&value);
						}
						else if (type == "double" || type == "float")
						{
							auto value = queryRes->getDouble(att);
							std::get<0>(attributes_[key])(&value);
						}
						else if (type == "bool")
						{
							auto value = queryRes->getBoolean(att);
							std::get<0>(attributes_[key])(&value);
						}
						else if (type.find("string") != std::string::npos)
						{
							auto value = queryRes->getString(att);
							std::get<0>(attributes_[key])(&value);
						}
					}
					results.push_back(entity);
				}
				delete queryRes;
				delete stmt;
				return results;
			}

		void Execute(const gdi::IQuery& query) const 
		{
			auto stmt = m_con->createStatement();
			stmt->executeUpdate(query.GetQuery());
			delete stmt;
		}

		~ExecutorMySQL()
		{
			m_con->close();
			delete m_con;
		}

	};
}
