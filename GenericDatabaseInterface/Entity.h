#pragma once
#ifndef ENTITY_CLASS
#define ENTITY_CLASS
#include <map>
#include <functional>
#include <list>
#include <typeinfo>
#include "StringUtils.h"

namespace gdi
{
    namespace
    {
        typedef std::function<void(void*)> Setter;
        typedef std::function<void* (void)> Getter;

        // [EntityName##ColumnName, <Setter, Getter, Type, Length>]
        // Setter and Getter in the map refer to the last created entity.
        std::map<std::string, std::tuple<Setter, Getter, std::string, size_t>> attributes_;
    }

#define ENTITY(Entity)                           \
class Entity {                                   \
    std::string m_name = #Entity;\
    bool m_null = true;\
    std::list<std::string> m_primary; \
    public:\
       std::string GetEntityName() const { return m_name; }\
       bool Null(){ return m_null; }\
       std::list<std::string> GetPrimary() const { return m_primary;}

#define ADD_VAR(Entity, Attribute, Type, Length)                           \
   private:                                                                         \
    class Attribute##Class {                                                         \
        public:\
           bool m_null = true;\
           Type m_##Attribute##Inner;  \
           Type Get##Attribute##Inner() { return m_##Attribute##Inner; }\
		    Attribute##Class(){\
                                auto key = std::string(#Entity#Attribute);\
                                gdi::ToLower(key);\
                                gdi::attributes_[key] = std::make_tuple(\
                                [&](void * att){m_##Attribute##Inner = *((Type*)att);},\
                                [&]()->Type*{ return &m_##Attribute##Inner;},\
                                typeid(m_##Attribute##Inner).name(),\
                                Length);\
                              }                               \
    };                                                                              \
    Attribute##Class m_##Attribute;                      \
   public:\
           Type Get##Attribute() const { return m_##Attribute.m_##Attribute##Inner; }\
           void Set##Attribute(Type attribute) { m_null = false; m_##Attribute.m_null = false; m_##Attribute.m_##Attribute##Inner = attribute; }\
           void Reset##Attribute() { m_##Attribute.m_null = true; }

#define ADD(Entity, Attribute, Type) ADD_VAR(Entity, Attribute, Type, 0) 

#define PRIMARY(Entity, ...)\
   Entity(){m_primary = {__VA_ARGS__};\
            for(auto& elem : m_primary)\
               gdi::ToLower(elem);\
            }

#define END };
}
#endif