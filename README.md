# gdi
Generic Database Interface

This library aims to provide C++ with a generic interface to manage databases via entity classes, in order to function it is built up based on 4 parts: Entity, EntityManager, Executor and Query builder.

## Entity

Via using the supported macros an entity can be declared as a class that represents a table in the database like:
```c++
string Name;
string Phone;
int Id;
double Number;

ENTITY(MyEntity)
ADD(MyEntity, Id)
ADD(MyEntity, Number)
ADD(MyEntity, Name)
ADD(MyEntity, Phone)
PRIMARY(MyEntity, "Id")
END 
```

Where `Name`, `Phone`, `Id`, `Number` variables must be global or static, and then `MyEntity` class can be used regularly:
```c++
MyEntity entity;
entity.SetId(1);
entity.SetName("MyName");
entity.SetPhone("12345");
entity.SetNumber(3.14);
```

## Executor

This class is to be implemented for each DBMS differently, here I implemented it to support MySQL databases, it is used to execute the queries and to get the results as entity objects, it should implement 2 methods: `Execute` and `ExecuteQuery`.

## EntityManager

This class needs an Executor as a dependency. EntityManager manages entity objects and is used to look for, update, insert and delete entities:
   -	`Find(Entity e)`: This returns an entity from the database which primary key equals `e` primary key.
   -	`FindBy(Entity e, …)`: This returns a set of entity objects which have the same `e` attributes values where `…` are those attributes names.
   -	`Insert(Entity e)`: Inserts the entity `e` into the database.
   -	`Update(Entity e)`: Updates a row in DB based on the values of `e` attributes.
   -	`UpdateBy(Entity e, …)`: Updates DB rows which have same `e` attributes values where `…` are those attributes names.
   -	`Delete(Entity e)`: Deletes a row from the DB based on the primary key.

## Query

This class contains the query to be executed, and it is used to build the query like:
```c++
Select("Country").Count("CustomerID").
        From("Customers").
        GroupBy("Country").
        Having("COUNT(CustomerID)", WhereCondition::GREATER_THAN, "5").
        OrderBy("COUNT(CustomerID) DESC");
``` 