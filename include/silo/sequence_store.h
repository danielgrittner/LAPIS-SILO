//
// Created by Alexander Taepper on 26.09.22.
//

#ifndef SILO_SEQUENCE_STORE_H
#define SILO_SEQUENCE_STORE_H

#include "meta_store.h"

namespace silo {

struct Position {
   friend class boost::serialization::access;

   template <class Archive>
   void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
      ar& bitmaps;
   }

   roaring::Roaring bitmaps[symbolCount];
};

struct SequenceStore {
   friend class boost::serialization::access;

   template <class Archive>
   void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
      ar& sequenceCount;

      ar& sid_to_epi;
      ar& epi_to_sid;

      ar& positions;
   }

   Position positions[genomeLength];

   std::vector<uint64_t> sid_to_epi;
   std::unordered_map<uint64_t, uint32_t> epi_to_sid;

   unsigned sequenceCount = 0;

   [[nodiscard]] size_t computeSize() const {
      size_t result = 0;
      for (auto& p : positions) {
         for (auto& b : p.bitmaps) {
            result += b.getSizeInBytes();
         }
      }
      return result;
   }

   /// pos: 1 indexed position of the genome
   [[nodiscard]] const roaring::Roaring* bm(size_t pos, Symbol s) const {
      return &positions[pos - 1].bitmaps[s];
   }

   /// pos: 1 indexed position of the genome
   [[nodiscard]] roaring::Roaring bmr(size_t pos, std::string s) const {
      return positions[pos - 1].bitmaps[to_symbol(s.at(pos - 1))];
   }

   /// pos: 1 indexed position of the genome
   [[nodiscard]] roaring::Roaring ref_mut(size_t pos, std::string s) const {
      Roaring tmp = bmr(pos, std::move(s));
      tmp.flip(0, sequenceCount);
      return tmp;
   }

   /// pos: 1 indexed position of the genome
   [[nodiscard]] roaring::Roaring neg_bm(size_t pos, Symbol s) const {
      Roaring tmp = *bm(pos, s);
      tmp.flip(0, sequenceCount);
      return tmp;
   }

   /// Returns an Roaring-bitmap which has the given residue r at the position pos,
   /// where the residue is interpreted in the _a_pproximate meaning
   /// That means a symbol matches all mixed symbols, which can indicate the residue
   /// pos: 1 indexed position of the genome
   [[nodiscard]] roaring::Roaring bma(size_t pos, Residue r) const;
};

int db_info(const SequenceStore& db, std::ostream& io);

int db_info_detailed(const SequenceStore& db, std::ostream& io);

unsigned save_db(const SequenceStore& db, const std::string& db_filename);

unsigned load_db(SequenceStore& db, const std::string& db_filename);

unsigned runoptimize(SequenceStore& db);

// static void interpret(SequenceStore& db, const vector<string>& genomes);

/// WARNING: This does not look at meta-data nor fill any indices required for queries. Use with caution
void process_raw(SequenceStore& db, std::istream& in);

void interpret(SequenceStore& db, const std::vector<std::string>& genomes);

void process_chunked_on_the_fly(SequenceStore& db, MetaStore& mdb, std::istream& in);

void sort_chunks(const MetaStore& mdb, const std::string& output_prefix_);

void sort_chunk(const MetaStore& mdb, const std::string& file_name, unsigned pid);

} //namespace silo;

#endif //SILO_SEQUENCE_STORE_H
