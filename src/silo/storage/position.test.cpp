#include "silo/storage/sequence_store.h"

#include <fstream>
#include <iostream>

#include <gtest/gtest.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

void serializeToFile(const std::string& filename, const silo::Position& position) {
   std::ofstream output_file(filename.c_str(), std::ios::binary);
   ::boost::archive::binary_oarchive output_archive(output_file);
   output_archive << position;
   output_file.close();
}

void deserializeFromFile(const std::string& filename, silo::Position& position) {
   std::ifstream input_file(filename, std::ios::binary);
   ::boost::archive::binary_iarchive input_archive(input_file);
   input_archive >> position;
   input_file.close();
}

TEST(position, should_serialize_and_deserialize_postions_with_unset_optional) {
   const std::string test_file = "test.bin";

   silo::Position const position_with_unset_optional;
   ASSERT_NO_THROW(serializeToFile(test_file, position_with_unset_optional));

   silo::Position deserialized_position;
   ASSERT_NO_THROW(deserializeFromFile(test_file, deserialized_position));

   EXPECT_FALSE(position_with_unset_optional.symbol_whose_bitmap_is_flipped.has_value());
   EXPECT_FALSE(deserialized_position.symbol_whose_bitmap_is_flipped.has_value());

   ASSERT_NO_THROW(std::remove(test_file.c_str()));
}

TEST(position, should_serialize_and_deserialize_postions_with_set_optional) {
   const std::string test_file = "test.bin";

   silo::Position position_with_set_optional;
   position_with_set_optional.symbol_whose_bitmap_is_flipped = silo::NUCLEOTIDE_SYMBOL::A;
   ASSERT_NO_THROW(serializeToFile(test_file, position_with_set_optional));

   silo::Position deserialized_position;
   ASSERT_NO_THROW(deserializeFromFile(test_file, deserialized_position));

   EXPECT_TRUE(deserialized_position.symbol_whose_bitmap_is_flipped.has_value());
   ASSERT_EQ(
      position_with_set_optional.symbol_whose_bitmap_is_flipped.value(),
      deserialized_position.symbol_whose_bitmap_is_flipped.value()
   );

   ASSERT_NO_THROW(std::remove(test_file.c_str()));
}