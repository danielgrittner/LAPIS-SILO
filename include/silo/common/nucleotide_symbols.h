#ifndef SILO_H
#define SILO_H

#include <array>
#include <iostream>

#include <spdlog/spdlog.h>

namespace silo {

static constexpr unsigned GENOME_LENGTH = 29903;

// https://www.bioinformatics.org/sms/iupac.html
enum class NUCLEOTIDE_SYMBOL {
   GAP,  // -, GAP
   A,    // Adenine
   C,    // Cytosine
   G,    // Guanine
   T,    // (or U) Thymine (or Uracil)
   R,    // A or G
   Y,    // C or T
   S,    // G or C
   W,    // A or T
   K,    // G or T
   M,    // A or C
   B,    // C or G or T
   D,    // A or G or T
   H,    // A or C or T
   V,    // A or C or G
   N,    // any base
};

static constexpr unsigned SYMBOL_COUNT = static_cast<unsigned>(NUCLEOTIDE_SYMBOL::N) + 1;

static constexpr std::array<char, SYMBOL_COUNT> SYMBOL_REPRESENTATION{
   '-', 'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W', 'K', 'M', 'B', 'D', 'H', 'V', 'N'};

static constexpr std::array<NUCLEOTIDE_SYMBOL, SYMBOL_COUNT> GENOME_SYMBOLS{
   NUCLEOTIDE_SYMBOL::GAP, NUCLEOTIDE_SYMBOL::A, NUCLEOTIDE_SYMBOL::C, NUCLEOTIDE_SYMBOL::G,
   NUCLEOTIDE_SYMBOL::T,   NUCLEOTIDE_SYMBOL::R, NUCLEOTIDE_SYMBOL::Y, NUCLEOTIDE_SYMBOL::S,
   NUCLEOTIDE_SYMBOL::W,   NUCLEOTIDE_SYMBOL::K, NUCLEOTIDE_SYMBOL::M, NUCLEOTIDE_SYMBOL::B,
   NUCLEOTIDE_SYMBOL::D,   NUCLEOTIDE_SYMBOL::H, NUCLEOTIDE_SYMBOL::V, NUCLEOTIDE_SYMBOL::N,
};

inline std::string genomeSymbolRepresentation(NUCLEOTIDE_SYMBOL symbol) {
   return std::string(1, SYMBOL_REPRESENTATION.at(static_cast<unsigned>(symbol)));
}

inline NUCLEOTIDE_SYMBOL toNucleotideSymbol(char character) {
   switch (character) {
      case '.':
      case '-':
         return NUCLEOTIDE_SYMBOL::GAP;
      case 'A':
         return NUCLEOTIDE_SYMBOL::A;
      case 'C':
         return NUCLEOTIDE_SYMBOL::C;
      case 'G':
         return NUCLEOTIDE_SYMBOL::G;
      case 'T':
      case 'U':
         return NUCLEOTIDE_SYMBOL::T;
      case 'R':
         return NUCLEOTIDE_SYMBOL::R;
      case 'Y':
         return NUCLEOTIDE_SYMBOL::Y;
      case 'S':
         return NUCLEOTIDE_SYMBOL::S;
      case 'W':
         return NUCLEOTIDE_SYMBOL::W;
      case 'K':
         return NUCLEOTIDE_SYMBOL::K;
      case 'M':
         return NUCLEOTIDE_SYMBOL::M;
      case 'B':
         return NUCLEOTIDE_SYMBOL::B;
      case 'D':
         return NUCLEOTIDE_SYMBOL::D;
      case 'H':
         return NUCLEOTIDE_SYMBOL::H;
      case 'V':
         return NUCLEOTIDE_SYMBOL::V;
      case 'N':
         return NUCLEOTIDE_SYMBOL::N;
      default:
         SPDLOG_ERROR("unrecognized symbol {}", character);
         return NUCLEOTIDE_SYMBOL::GAP;
   }
}
}  // namespace silo

#endif  // SILO_H
