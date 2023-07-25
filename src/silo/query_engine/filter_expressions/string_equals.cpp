#include "silo/query_engine/filter_expressions/string_equals.h"

#include <map>
#include <unordered_map>
#include <utility>

#include <nlohmann/json.hpp>
#include <roaring/roaring.hh>

#include "silo/common/string.h"
#include "silo/query_engine/filter_expressions/expression.h"
#include "silo/query_engine/operators/empty.h"
#include "silo/query_engine/operators/index_scan.h"
#include "silo/query_engine/operators/selection.h"
#include "silo/query_engine/query_parse_exception.h"
#include "silo/storage/column/indexed_string_column.h"
#include "silo/storage/column/string_column.h"
#include "silo/storage/column_group.h"
#include "silo/storage/database_partition.h"

namespace silo {
class Database;
namespace query_engine::operators {
class Operator;
}  // namespace query_engine::operators
}  // namespace silo

namespace silo::query_engine::filter_expressions {

StringEquals::StringEquals(std::string column, std::string value)
    : column(std::move(column)),
      value(std::move(value)) {}

std::string StringEquals::toString(const silo::Database& /*database*/) const {
   return column + " = '" + value + "'";
}

std::unique_ptr<silo::query_engine::operators::Operator> StringEquals::compile(
   const silo::Database& /*database*/,
   const silo::DatabasePartition& database_partition,
   Expression::AmbiguityMode /*mode*/
) const {
   if (database_partition.columns.indexed_string_columns.contains(column)) {
      const auto& string_column = database_partition.columns.indexed_string_columns.at(column);
      const roaring::Roaring& bitmap = string_column.filter(value);

      if (bitmap.isEmpty()) {
         return std::make_unique<operators::Empty>(database_partition.sequenceCount);
      }
      return std::make_unique<operators::IndexScan>(
         new roaring::Roaring(bitmap), database_partition.sequenceCount
      );
   }

   if (database_partition.columns.string_columns.contains(column)) {
      const auto& string_column = database_partition.columns.string_columns.at(column);
      const auto& embedded_string = string_column.embedString(value);
      if (embedded_string.has_value()) {
         return std::make_unique<operators::Selection>(
            std::make_unique<operators::CompareToValueSelection<common::SiloString>>(
               string_column.getValues(), operators::Comparator::EQUALS, embedded_string.value()
            ),
            database_partition.sequenceCount
         );
      }
      return std::make_unique<operators::Empty>(database_partition.sequenceCount);
   }

   return std::make_unique<operators::Empty>(database_partition.sequenceCount);
}

// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const nlohmann::json& json, std::unique_ptr<StringEquals>& filter) {
   CHECK_SILO_QUERY(
      json.contains("column"), "The field 'column' is required in an StringEquals expression"
   )
   CHECK_SILO_QUERY(
      json["column"].is_string(),
      "The field 'column' in an StringEquals expression needs to be a string"
   )
   CHECK_SILO_QUERY(
      json.contains("value"), "The field 'value' is required in an StringEquals expression"
   )
   CHECK_SILO_QUERY(
      json["value"].is_string() || json["value"].is_null(),
      "The field 'value' in an StringEquals expression needs to be a string or null"
   )
   const std::string& column = json["column"];
   const std::string& value = json["value"].is_null() ? "" : json["value"].get<std::string>();
   filter = std::make_unique<StringEquals>(column, value);
}

}  // namespace silo::query_engine::filter_expressions