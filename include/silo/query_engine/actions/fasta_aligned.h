#ifndef SILO_FASTA_ALIGNED_H
#define SILO_FASTA_ALIGNED_H

#include <memory>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "silo/query_engine/actions/action.h"
#include "silo/query_engine/query_result.h"

namespace silo {
namespace query_engine {
struct OperatorResult;
}  // namespace query_engine
struct Database;
}  // namespace silo

namespace silo::query_engine::actions {

class FastaAligned : public Action {
   void validateOrderByFields(const Database& database) const override;

   QueryResult execute(const Database& database, std::vector<OperatorResult> bitmap_filter)
      const override;

  public:
   explicit FastaAligned();
};

// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const nlohmann::json& json, std::unique_ptr<FastaAligned>& action);

}  // namespace silo::query_engine::actions

#endif  // SILO_FASTA_ALIGNED_H
