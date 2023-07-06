
#ifndef SILO_SEQUENCE_STORE_H
#define SILO_SEQUENCE_STORE_H

#include <array>
#include <deque>
#include <optional>

#include <silo/common/fasta_reader.h>
#include <spdlog/spdlog.h>
#include <boost/serialization/array.hpp>
#include <roaring/roaring.hh>

#include "silo/common/nucleotide_symbols.h"
#include "silo/common/zstdfasta_reader.h"
#include "silo/roaring/roaring_serialize.h"
#include "silo/storage/serialize_optional.h"

namespace silo {

struct NucPosition {
   friend class boost::serialization::access;

   template <class Archive>
   void serialize(Archive& archive, [[maybe_unused]] const unsigned int version) {
      // clang-format off
      archive& symbol_whose_bitmap_is_flipped;
      archive& bitmaps;
      // clang-format on
   }

   std::array<roaring::Roaring, NUC_SYMBOL_COUNT> bitmaps;
   std::optional<NUCLEOTIDE_SYMBOL> symbol_whose_bitmap_is_flipped = std::nullopt;
};

struct SequenceStoreInfo {
   uint32_t sequence_count;
   uint64_t size;
   size_t n_bitmaps_size;
};

class SequenceStorePartition {
   friend class boost::serialization::access;

  private:
   template <class Archive>
   void serialize(Archive& archive, [[maybe_unused]] const unsigned int version) {
      // clang-format off
      archive& sequence_count;
      archive& positions;
      archive& nucleotide_symbol_n_bitmaps;
      // clang-format on
   }

   void fillIndexes(const std::vector<std::string>& genomes);

   void fillNBitmaps(const std::vector<std::string>& genomes);

  public:
   explicit SequenceStorePartition(const std::string& reference_genome);

   const std::string& reference_genome;
   std::vector<NucPosition> positions;
   std::vector<roaring::Roaring> nucleotide_symbol_n_bitmaps;
   uint32_t sequence_count = 0;

   [[nodiscard]] size_t computeSize() const;

   [[nodiscard]] const roaring::Roaring* getBitmap(size_t position, NUCLEOTIDE_SYMBOL symbol) const;

   SequenceStoreInfo getInfo() const;

   size_t fill(silo::ZstdFastaReader& input_file);

   void interpret(const std::vector<std::string>& genomes);

   size_t runOptimize();

   size_t shrinkToFit();
};

class SequenceStore {
  public:
   std::string reference_genome;
   std::deque<SequenceStorePartition> partitions;

   explicit SequenceStore(std::string reference_genome);

   SequenceStorePartition& createPartition();
};

}  // namespace silo

template <>
struct [[maybe_unused]] fmt::formatter<silo::SequenceStoreInfo> : fmt::formatter<std::string> {
   [[maybe_unused]] static auto format(
      silo::SequenceStoreInfo sequence_store_info,
      format_context& ctx
   ) -> decltype(ctx.out());
};

#endif  // SILO_SEQUENCE_STORE_H
