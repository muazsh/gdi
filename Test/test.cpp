#include "pch.h"

namespace gdi
{
    static std::string Name;
    static std::string Phone;
    static int Id;
    static double Number;

    ENTITY(MyEntity)
    ADD(MyEntity, Id)
    ADD(MyEntity, Number)
    ADD(MyEntity, Name)
    ADD_VAR(MyEntity, Phone, 20)
    PRIMARY(MyEntity, "Id")
    END
}

class EntityManagerTest : public ::testing::Test {
    public:
    std::unique_ptr<gdi::EntityManager<gdi::ExecutorMySQL>> m_managerMySQL;
    std::unique_ptr<gdi::EntityManager<gdi::ExecutorPostgres>> m_managerPostgres;

protected:

    virtual void SetUp() {
        auto executorMySQL = std::make_unique<gdi::ExecutorMySQL>("tcp://127.0.0.1:3306", "root", "root");
        auto query = gdi::Create().Database("TestDB");
        executorMySQL->Execute(query);
        executorMySQL->SetSchema("TestDB");
        m_managerMySQL = std::make_unique<gdi::EntityManager<gdi::ExecutorMySQL> >(executorMySQL);
        gdi::MyEntity entity;
        query = gdi::Create().Table<gdi::MyEntity>(entity);
        m_managerMySQL->Execute(query);
        {
            auto executorPostgres = std::make_unique<gdi::ExecutorPostgres>("localhost", "5432", "postgres", "postgres", "root");
            query = gdi::Create().Database("testdb");
            executorPostgres->Execute(query);
        }
        
        auto executorPostgres = std::make_unique<gdi::ExecutorPostgres>("localhost", "5432", "testdb", "postgres", "root");
        m_managerPostgres = std::make_unique<gdi::EntityManager<gdi::ExecutorPostgres> >(executorPostgres);
        query = gdi::Create().Table<gdi::MyEntity>(entity);
        m_managerPostgres->Execute(query);
    }

    virtual void TearDown() {
        auto query = gdi::Drop().TableDrop("MyEntity");
        m_managerMySQL->Execute(query);
        m_managerPostgres->Execute(query);
        query = gdi::Drop().Database("testdb");
        m_managerMySQL->Execute(query);
        {
            auto executorPostgres = std::make_unique<gdi::ExecutorPostgres>("localhost", "5432", "postgres", "postgres", "root");
            m_managerPostgres = std::make_unique<gdi::EntityManager<gdi::ExecutorPostgres> >(executorPostgres);
            query = gdi::Drop().Database("testdb");
            m_managerPostgres->Execute(query);
        }
    }

};

TEST_F(EntityManagerTest, Find) {
    gdi::MyEntity entity;
    entity.SetId(1);
    entity.SetName("Max");
    entity.SetPhone("12345");
    entity.SetNumber(11);
    auto res = m_managerMySQL->Find(entity);
    EXPECT_EQ(0, res.size());
}

TEST_F(EntityManagerTest, Insert) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(5.01);
    m_managerMySQL->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerMySQL->Insert(entity2);

    auto res1 = m_managerMySQL->Find(entity1);
    EXPECT_EQ(1, res1.size());
    EXPECT_EQ(1, res1.front().GetId());
    EXPECT_EQ("Mouaz", res1.front().GetName());
    EXPECT_EQ("12345", res1.front().GetPhone());
    EXPECT_EQ(5.01, res1.front().GetNumber());

    auto res2 = m_managerMySQL->Find(entity2);
    EXPECT_EQ(1, res2.size());
    EXPECT_EQ(2, res2.front().GetId());
    EXPECT_EQ("Max", res2.front().GetName());
    EXPECT_EQ("12345", res2.front().GetPhone());
    EXPECT_EQ(3.14, res2.front().GetNumber());
}

TEST_F(EntityManagerTest, FindBy) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(5.01);
    m_managerMySQL->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerMySQL->Insert(entity2);

    auto res = m_managerMySQL->FindBy(entity1, "Phone");
    EXPECT_EQ(2, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("12345", res.front().GetPhone());
    EXPECT_EQ(5.01, res.front().GetNumber());
    EXPECT_EQ(2, res.back().GetId());
    EXPECT_EQ("Max", res.back().GetName());
    EXPECT_EQ("12345", res.back().GetPhone());
    EXPECT_EQ(3.14, res.back().GetNumber());

    res = m_managerMySQL->FindBy(entity1, "Phone", "Number");
    EXPECT_EQ(1, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("12345", res.front().GetPhone());
    EXPECT_EQ(5.01, res.front().GetNumber());
}

TEST_F(EntityManagerTest, Update) {
    gdi::MyEntity entity;
    entity.SetId(3);
    entity.SetName("Mouaz");
    entity.SetPhone("12345");
    entity.SetNumber(5.01);
    m_managerMySQL->Insert(entity);

    entity.SetPhone("54321");
    entity.SetNumber(3.14);
    m_managerMySQL->Update(entity);

    auto res = m_managerMySQL->Find(entity);
    EXPECT_EQ(1, res.size());
    EXPECT_EQ(3, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("54321", res.front().GetPhone());
    EXPECT_EQ(3.14, res.front().GetNumber());
}

TEST_F(EntityManagerTest, UpdateBy) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(3.14);
    m_managerMySQL->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerMySQL->Insert(entity2);

    entity1.SetId(3);
    entity1.SetName("Tom");
    entity1.SetNumber(3.14);

    m_managerMySQL->UpdateBy(entity1, "Phone", "Number");
    auto res = m_managerMySQL->FindBy(entity1, "Phone");
    EXPECT_EQ(2, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Tom", res.front().GetName());
    EXPECT_EQ(2, res.back().GetId());
    EXPECT_EQ("Tom", res.back().GetName());
}

TEST_F(EntityManagerTest, Delete) {
    gdi::MyEntity entity;
    entity.SetId(4);
    entity.SetName("Mouaz");
    entity.SetPhone("12345");
    entity.SetNumber(5.01);
    m_managerMySQL->Insert(entity);
    m_managerMySQL->Delete(entity);
    auto res = m_managerMySQL->Find(entity);
    EXPECT_EQ(0, res.size());
}

TEST_F(EntityManagerTest, Find_Postgres) {
    gdi::MyEntity entity;
    entity.SetId(1);
    entity.SetName("Max");
    entity.SetPhone("12345");
    entity.SetNumber(11);
    auto res = m_managerPostgres->Find(entity);
    EXPECT_EQ(0, res.size());
}

TEST_F(EntityManagerTest, Insert_Postgres) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(5.01);
    m_managerPostgres->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerPostgres->Insert(entity2);

    auto res1 = m_managerPostgres->Find(entity1);
    EXPECT_EQ(1, res1.size());
    EXPECT_EQ(1, res1.front().GetId());
    EXPECT_EQ("Mouaz", res1.front().GetName());
    EXPECT_EQ("12345", res1.front().GetPhone());
    EXPECT_EQ(5.01, res1.front().GetNumber());

    auto res2 = m_managerPostgres->Find(entity2);
    EXPECT_EQ(1, res2.size());
    EXPECT_EQ(2, res2.front().GetId());
    EXPECT_EQ("Max", res2.front().GetName());
    EXPECT_EQ("12345", res2.front().GetPhone());
    EXPECT_EQ(3.14, res2.front().GetNumber());
}

TEST_F(EntityManagerTest, FindBy_Postgres) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(5.01);
    m_managerPostgres->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerPostgres->Insert(entity2);

    auto res = m_managerPostgres->FindBy(entity1, "Phone");
    EXPECT_EQ(2, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("12345", res.front().GetPhone());
    EXPECT_EQ(5.01, res.front().GetNumber());
    EXPECT_EQ(2, res.back().GetId());
    EXPECT_EQ("Max", res.back().GetName());
    EXPECT_EQ("12345", res.back().GetPhone());
    EXPECT_EQ(3.14, res.back().GetNumber());

    res = m_managerPostgres->FindBy(entity1, "Phone", "Number");
    EXPECT_EQ(1, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("12345", res.front().GetPhone());
    EXPECT_EQ(5.01, res.front().GetNumber());
}

TEST_F(EntityManagerTest, Update_Postgres) {
    gdi::MyEntity entity;
    entity.SetId(3);
    entity.SetName("Mouaz");
    entity.SetPhone("12345");
    entity.SetNumber(5.01);
    m_managerPostgres->Insert(entity);

    entity.SetPhone("54321");
    entity.SetNumber(3.14);
    m_managerPostgres->Update(entity);

    auto res = m_managerPostgres->Find(entity);
    EXPECT_EQ(1, res.size());
    EXPECT_EQ(3, res.front().GetId());
    EXPECT_EQ("Mouaz", res.front().GetName());
    EXPECT_EQ("54321", res.front().GetPhone());
    EXPECT_EQ(3.14, res.front().GetNumber());
}

TEST_F(EntityManagerTest, UpdateBy_Postgres) {
    gdi::MyEntity entity1;
    entity1.SetId(1);
    entity1.SetName("Mouaz");
    entity1.SetPhone("12345");
    entity1.SetNumber(3.14);
    m_managerPostgres->Insert(entity1);

    gdi::MyEntity entity2;
    entity2.SetId(2);
    entity2.SetName("Max");
    entity2.SetPhone("12345");
    entity2.SetNumber(3.14);
    m_managerPostgres->Insert(entity2);

    entity1.SetId(3);
    entity1.SetName("Tom");
    entity1.SetNumber(3.14);

    m_managerPostgres->UpdateBy(entity1, "Phone", "Number");
    auto res = m_managerPostgres->FindBy(entity1, "Phone");
    EXPECT_EQ(2, res.size());
    EXPECT_EQ(1, res.front().GetId());
    EXPECT_EQ("Tom", res.front().GetName());
    EXPECT_EQ(2, res.back().GetId());
    EXPECT_EQ("Tom", res.back().GetName());
}

TEST_F(EntityManagerTest, Delete_Postgres) {
    gdi::MyEntity entity;
    entity.SetId(4);
    entity.SetName("Mouaz");
    entity.SetPhone("12345");
    entity.SetNumber(5.01);
    m_managerPostgres->Insert(entity);
    m_managerPostgres->Delete(entity);
    auto res = m_managerPostgres->Find(entity);
    EXPECT_EQ(0, res.size());
}

TEST(QueryBuilder, Join)
{
    auto queryString = "SELECT Orders.OrderID " 
        "FROM Orders "
        "INNER JOIN Customers "
        "ON Orders.CustomerID = Customers.CustomerID";
    auto query = gdi::Select("Orders.OrderID").
        From("Orders").
        InnerJoin("Customers").
        On("Orders.CustomerID", gdi::WhereCondition::EQUAL, "Customers.CustomerID");
    EXPECT_EQ(queryString, query.GetQuery());
}

TEST(QueryBuilder, GroupBy)
{
    auto queryString = "SELECT Country, COUNT(CustomerID)  "
        "FROM Customers "
        "GROUP BY Country "
        "HAVING COUNT(CustomerID) > 5 "
        "ORDER BY COUNT(CustomerID) DESC";
    auto query = gdi::Select("Country").Count("CustomerID").
        From("Customers").
        GroupBy("Country").
        Having("COUNT(CustomerID)", gdi::WhereCondition::GREATER_THAN, "5").
        OrderBy("COUNT(CustomerID) DESC");
    EXPECT_EQ(queryString, query.GetQuery());
}

TEST(QueryBuilder, InsertInto)
{
    auto queryString = "INSERT INTO Customers(Name,Street,Number,City,PostalCode,Country) "
        "VALUES('Tom','Kaiser','Berlin',67663,'Germany')";
    auto query = gdi::InsertInto("Customers", "Name", "Street", "Number", "City", "PostalCode", "Country").
        Values("'Tom'", "'Kaiser'" , "'Berlin'", "67663", "'Germany'");
    EXPECT_EQ(queryString, query.GetQuery());
}

TEST(QueryBuilder, Update)
{
    auto queryString = "UPDATE Customers "
        "SET Name = 'Tom',City = 'Berlin' "
        "WHERE CustomerID = 1";
    auto query = gdi::Update("Customers").
        Set({"Name","City"}, { "'Tom'", "'Berlin'"}).
        Where("CustomerID",gdi::WhereCondition::EQUAL,"1");
    EXPECT_EQ(queryString, query.GetQuery());
}

TEST(QueryBuilder, Delete)
{
    auto queryString = "DELETE  FROM Customers WHERE CustomerID = 1";
    auto query = gdi::Delete().
        From("Customers").
        Where("CustomerID", gdi::WhereCondition::EQUAL, "1");
    EXPECT_EQ(queryString, query.GetQuery());
}