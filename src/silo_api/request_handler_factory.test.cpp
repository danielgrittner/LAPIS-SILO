#include <Poco/Net/HTTPResponse.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "silo/common/data_version.h"
#include "silo/common/nucleotide_symbols.h"
#include "silo/database.h"
#include "silo/database_info.h"
#include "silo/query_engine/query_engine.h"
#include "silo/query_engine/query_result.h"
#include "silo/storage/column/date_column.h"
#include "silo/storage/column/indexed_string_column.h"
#include "silo/storage/column/int_column.h"
#include "silo/storage/column/pango_lineage_column.h"
#include "silo/storage/column/string_column.h"
#include "silo/storage/database_partition.h"
#include "silo_api/manual_poco_mocks.test.h"
#include "silo_api/request_handler_factory.h"

class MockDatabase : public silo::Database {
  public:
   MOCK_METHOD(silo::DatabaseInfo, getDatabaseInfo, (), (const));
   MOCK_METHOD(silo::DetailedDatabaseInfo, detailedDatabaseInfo, (), (const));
   MOCK_METHOD(silo::DataVersion, getDataVersion, (), (const));
};

class MockQueryEngine : public silo::query_engine::QueryEngine {
  public:
   explicit MockQueryEngine(const silo::Database& database)
       : QueryEngine(database) {}

   MOCK_METHOD(silo::query_engine::QueryResult, executeQuery, (const std::string&), (const));
};

class RequestHandlerTestFixture : public ::testing::Test {
  protected:
   MockDatabase mock_database;
   MockQueryEngine mock_query_engine;
   silo_api::test::MockResponse response;
   silo_api::test::MockRequest request;
   silo_api::SiloRequestHandlerFactory under_test;

   RequestHandlerTestFixture()
       : mock_query_engine(MockQueryEngine(mock_database)),
         request(silo_api::test::MockRequest(response)),
         under_test(silo_api::SiloRequestHandlerFactory(mock_database, mock_query_engine)) {}

   void processRequest() {
      std::unique_ptr<Poco::Net::HTTPRequestHandler> request_handler(
         under_test.createRequestHandler(request)
      );
      request_handler->handleRequest(request, response);
   }
};

TEST_F(RequestHandlerTestFixture, handlesGetInfoRequest) {
   EXPECT_CALL(mock_database, getDatabaseInfo)
      .WillRepeatedly(testing::Return(silo::DatabaseInfo{1, 2, 3}));
   EXPECT_CALL(mock_database, getDataVersion)
      .WillRepeatedly(testing::Return(silo::DataVersion("1234")));

   request.setURI("/info");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_OK);
   EXPECT_EQ(response.out_stream.str(), R"({"nBitmapsSize":3,"sequenceCount":1,"totalSize":2})");
   EXPECT_EQ(response.get("data-version"), "1234");
}

TEST_F(RequestHandlerTestFixture, handlesGetInfoRequestDetails) {
   silo::BitmapSizePerSymbol bitmap_size_per_symbol;
   bitmap_size_per_symbol.size_in_bytes[silo::NUCLEOTIDE_SYMBOL::A] =
      1234;  // NOLINT(readability-magic-numbers)

   const silo::BitmapContainerSize bitmap_container_size(
      29903, 4567
   );  // NOLINT(readability-magic-numbers)

   silo::SequenceStoreStatistics stats = {bitmap_size_per_symbol, bitmap_container_size};

   const silo::DetailedDatabaseInfo detailed_database_info = {{{"main", stats}}};

   EXPECT_CALL(mock_database, detailedDatabaseInfo)
      .WillRepeatedly(testing::Return(detailed_database_info));
   EXPECT_CALL(mock_database, getDataVersion)
      .WillRepeatedly(testing::Return(silo::DataVersion("1234")));

   request.setURI("/info?details=true");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_OK);
   EXPECT_EQ(
      response.out_stream.str(),
      R"({"bitmapContainerSizePerGenomeSection":{"bitmapContainerSizeStatistic":{"numberOfArrayContainers":0,"numberOfBitsetContainers":0,"numberOfRunContainers":0,"numberOfValuesStoredInArrayContainers":0,"numberOfValuesStoredInBitsetContainers":0,"numberOfValuesStoredInRunContainers":0,"totalBitmapSizeArrayContainers":0,"totalBitmapSizeBitsetContainers":0,"totalBitmapSizeRunContainers":0},"sectionLength":4567,"sizePerGenomeSymbolAndSection":{"-":[0,0,0,0,0,0,0],"N":[0,0,0,0,0,0,0],"NOT_N_NOT_GAP":[0,0,0,0,0,0,0]},"totalBitmapSizeComputed":0,"totalBitmapSizeFrozen":0},"bitmapSizePerSymbol":{"-":0,"A":1234,"B":0,"C":0,"D":0,"G":0,"H":0,"K":0,"M":0,"N":0,"R":0,"S":0,"T":0,"V":0,"W":0,"Y":0}})"
   );  // NOLINT
   EXPECT_EQ(response.get("data-version"), "1234");
}

TEST_F(RequestHandlerTestFixture, returnsMethodNotAllowedOnPostInfoRequest) {
   request.setMethod("POST");
   request.setURI("/info");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
   EXPECT_EQ(
      response.out_stream.str(),
      R"({"error":"Method not allowed","message":"POST is not allowed on resource /info"})"
   );
}

TEST_F(RequestHandlerTestFixture, handlesPostQueryRequest) {
   std::map<std::string, std::optional<std::variant<std::string, int32_t, double>>> fields{
      // NOLINTNEXTLINE(readability-magic-numbers)
      {"count", 5}};
   const std::vector<silo::query_engine::QueryResultEntry> tmp{{fields}};
   const silo::query_engine::QueryResult query_result{tmp};
   EXPECT_CALL(mock_query_engine, executeQuery).WillRepeatedly(testing::Return(query_result));
   EXPECT_CALL(mock_database, getDataVersion)
      .WillRepeatedly(testing::Return(silo::DataVersion("1234")));

   request.setMethod("POST");
   request.setURI("/query");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_OK);
   EXPECT_EQ(response.out_stream.str(), R"({"queryResult":[{"count":5}]})");
   EXPECT_EQ(response.get("data-version"), "1234");
}

TEST_F(RequestHandlerTestFixture, returnsMethodNotAllowedOnGetQuery) {
   request.setMethod("GET");
   request.setURI("/query");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
   EXPECT_EQ(
      response.out_stream.str(),
      R"({"error":"Method not allowed","message":"GET is not allowed on resource /query"})"
   );
}

TEST_F(RequestHandlerTestFixture, givenRequestToUnknownUrl_thenReturnsNotFound) {
   auto under_test = silo_api::SiloRequestHandlerFactory(mock_database, mock_query_engine);

   request.setURI("/doesNotExist");

   processRequest();

   EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
   EXPECT_EQ(
      response.out_stream.str(),
      R"({"error":"Not found","message":"Resource /doesNotExist does not exist"})"
   );
}
