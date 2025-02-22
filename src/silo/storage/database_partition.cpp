#include "silo/storage/database_partition.h"

#include <tbb/parallel_for.h>

#include "silo/storage/column_group.h"

namespace silo {
namespace preprocessing {
struct Chunk;
}  // namespace preprocessing
namespace storage::column {
class DateColumnPartition;
class FloatColumnPartition;
class IndexedStringColumnPartition;
class IntColumnPartition;
class PangoLineageColumnPartition;
class StringColumnPartition;
}  // namespace storage::column

DatabasePartition::DatabasePartition(std::vector<silo::preprocessing::Chunk> chunks)
    : chunks(std::move(chunks)) {}

void DatabasePartition::flipBitmaps() {
   for (auto& [_, seq_store] : nuc_sequences) {
      auto& positions = seq_store.positions;
      tbb::parallel_for(tbb::blocked_range<uint32_t>(0, positions.size()), [&](const auto& local) {
         for (auto position = local.begin(); position != local.end(); ++position) {
            positions[position].flipMostNumerousBitmap(sequence_count);
         }
      });
   }
   for (auto& [_, seq_store] : aa_sequences) {
      auto& positions = seq_store.positions;
      tbb::parallel_for(tbb::blocked_range<uint32_t>(0, positions.size()), [&](const auto& local) {
         for (auto position = local.begin(); position != local.end(); ++position) {
            positions[position].flipMostNumerousBitmap(sequence_count);
         }
      });
   }
}

const std::vector<preprocessing::Chunk>& DatabasePartition::getChunks() const {
   return chunks;
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::StringColumnPartition& column
) {
   columns.string_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::IndexedStringColumnPartition& column
) {
   columns.indexed_string_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::IntColumnPartition& column
) {
   columns.int_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::DateColumnPartition& column
) {
   columns.date_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::InsertionColumnPartition& column
) {
   columns.insertion_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::PangoLineageColumnPartition& column
) {
   columns.pango_lineage_columns.insert({std::string(name), column});
}

void DatabasePartition::insertColumn(
   const std::string& name,
   storage::column::FloatColumnPartition& column
) {
   columns.float_columns.insert({std::string(name), column});
}

}  // namespace silo
