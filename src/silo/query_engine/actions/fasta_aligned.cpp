#include "silo/query_engine/actions/fasta_aligned.h"

#include "silo/query_engine/operator_result.h"
#include "silo/query_engine/query_parse_exception.h"
#include "silo/query_engine/query_result.h"

namespace silo {
struct Database;
}  // namespace silo

namespace silo::query_engine::actions {

FastaAligned::FastaAligned() = default;

void FastaAligned::validateOrderByFields(const Database& /*database*/) const {
   throw QueryParseException(
      "Not implemented: The Ordering of FastaAligned actions is not yet supported"
   );
}

QueryResult FastaAligned::execute(
   const Database& /*database*/,
   std::vector<OperatorResult> /*bitmap_filter*/
) const {
   return {};
}

// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const nlohmann::json& /*json*/, std::unique_ptr<FastaAligned>& action) {
   action = std::make_unique<FastaAligned>();
   throw QueryParseException("Not implemented: The FastAligned action has not been implemented");
}

}  // namespace silo::query_engine::actions
