#ifndef SILO_STRING_EQUALS_H
#define SILO_STRING_EQUALS_H

#include <memory>
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "silo/query_engine/filter_expressions/expression.h"

namespace silo {
class Database;
class DatabasePartition;
namespace query_engine {
namespace operators {
class Operator;
}  // namespace operators
}  // namespace query_engine
}  // namespace silo

namespace silo::query_engine::filter_expressions {

struct StringEquals : public Expression {
  private:
   std::string column;
   std::string value;

  public:
   explicit StringEquals(std::string column, std::string value);

   std::string toString(const Database& database) const override;

   [[nodiscard]] std::unique_ptr<silo::query_engine::operators::Operator> compile(
      const Database& database,
      const DatabasePartition& database_partition,
      AmbiguityMode mode
   ) const override;
};

// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const nlohmann::json& json, std::unique_ptr<StringEquals>& filter);

}  // namespace silo::query_engine::filter_expressions

#endif  // SILO_STRING_EQUALS_H
