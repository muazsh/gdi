#pragma once
#include "Command.h"
#include "Query.h"
#include "ExecutorMySQL.h"
#include <memory>

namespace gdi
{
	template<class Ex>
	class EntityManager
	{
		std::unique_ptr<Ex> p_executor;
		std::string GetEntityAttributeValue( std::string attribute )
		{
			ToLower(attribute);
			void* val = std::get<1>(attributes_[attribute])();
			std::string opt = "";
			auto type = std::get<2>(attributes_[attribute]);

			if (type == "int") opt = std::to_string(*(int*)val);
			else if (type == "short") opt = std::to_string(*(short*)val);
			else if (type == "long") opt = std::to_string(*(long*)val);
			else if (type == "int64") opt = std::to_string(*(long long*)val);
			else if (type == "float") opt = std::string("'") + std::to_string(*(float*)val) + std::string("'");
			else if (type == "double") opt = std::string("'") + std::to_string(*(double*)val) + std::string("'");
			else if (type == "bool") opt = std::to_string(*(bool*)val);
			else if (type.find("string") != std::string::npos) opt = std::string("'") + *(std::string*)val + std::string("'");

			return opt;
		}

		std::list<std::string> GetColumnNames(const std::string& entityName)
		{
			std::list<std::string> columns;
			auto entityNameLength = entityName.length();
			for (auto& att : attributes_)
			{
				columns.push_back(att.first.substr(entityNameLength));
			}
			return columns;
		}

		std::list<std::string> GetColumnNamesWithoutPrimary(const std::string& entityName, const std::list<std::string>& primary)
		{
			std::list<std::string> columns;
			auto entityNameLength = entityName.length();
			for (auto& att : attributes_)
			{
				auto name = att.first.substr(entityNameLength);
				if(std::find_if(primary.begin(), primary.end(), [&](std::string elem) { return elem == name; }) == primary.end())
					columns.push_back(name);
			}
			return columns;
		}

		void SetWhereCondition(Query& query, const std::string& entityName, const std::list<std::string>& columns)
		{
			if (columns.size() > 0)
			{
				auto att = GetEntityAttributeValue(entityName + columns.front());
				query.Where(columns.front(), WhereCondition::EQUAL, att);
				auto it = columns.begin();
				while (++it != columns.end())
				{
					att = GetEntityAttributeValue(entityName + *it);
					query.And(*it, WhereCondition::EQUAL, att);
				}
			}
		}

		template <class E, class L>
		void UpdateWithCondition(E& entity, L condition)
		{
			auto columns = GetColumnNamesWithoutPrimary(entity.GetEntityName(), entity.GetPrimary());
			std::list<std::string> values;
			for (auto& col : columns)
			{
				values.push_back(GetEntityAttributeValue(entity.GetEntityName() + col));
			}
			auto q = gdi::Update(entity.GetEntityName()).Set(columns, values);
			condition(q);
			p_executor->Execute(q);
		}

	public:
		EntityManager(std::unique_ptr<Ex>& executor) : p_executor(std::move(executor)) {}

		template <class E>
		std::list<E> Find(E entity)
		{
			if (!entity.Null())
			{
				E localEntity; // This instantiation is necessary so the Getter and Setter in arrtibutes_ map will refere to the localEntity.
				localEntity = entity;
				auto q = gdi::Select(GetColumnNames(localEntity.GetEntityName())).From(localEntity.GetEntityName());
				SetWhereCondition(q, localEntity.GetEntityName(), localEntity.GetPrimary());
				return p_executor->ExecuteQuery<E>(q);
			}
			return std::list<E>();
		}

		template <class E, class... C>
		std::list<E> FindBy(E entity, C... cols)
		{
			if (!entity.Null())
			{
				E localEntity;
				localEntity = entity;
				auto q = gdi::Select(GetColumnNames(localEntity.GetEntityName())).From(localEntity.GetEntityName());
				SetWhereCondition(q, localEntity.GetEntityName(), std::list<std::string>{cols...});
				return p_executor->ExecuteQuery<E>(q);
			}
			return std::list<E>();
		}

		template <class E>
		void Update(E entity)
		{
			if (!entity.Null())
			{
				E localEntity;
				localEntity = entity;
				UpdateWithCondition(localEntity, [&](Query& q) {SetWhereCondition(q, localEntity.GetEntityName(), localEntity.GetPrimary()); });
			}
		}

		template <class E, class... C>
		void UpdateBy(E entity, C... cols)
		{
			if (!entity.Null())
			{
				E localEntity;
				localEntity = entity;
				UpdateWithCondition(localEntity, [&](Query& q) {SetWhereCondition(q, localEntity.GetEntityName(), std::list<std::string>{cols...}); });
			}
		}

		template <class E>
		void Insert(E entity)
		{
			if (!entity.Null())
			{
				E localEntity;
				localEntity = entity;
				std::list<std::string> columns = GetColumnNames(localEntity.GetEntityName());
				std::list<std::string> values;
				for (auto& col : columns)
				{
					values.push_back(GetEntityAttributeValue(localEntity.GetEntityName() + col));
				}
				auto q = gdi::InsertIntoOfList(localEntity.GetEntityName(), columns).ValuesList(values);
				p_executor->Execute(q);
			}
		}

		template <class E>
		void Delete(E entity)
		{
			if (!entity.Null())
			{
				E localEntity;
				localEntity = entity;
				auto q = gdi::Delete().From(localEntity.GetEntityName());
				SetWhereCondition(q, localEntity.GetEntityName(), localEntity.GetPrimary());
				p_executor->Execute(q);
			}
		}

		template<class E>
		std::list<E> ExecuteQuery(gdi::Query& query) const
		{
			return p_executor->ExecuteQuery(query);
		}

		void Execute(const gdi::IQuery& query) const
		{
			p_executor->Execute(query);
		}
	};
}