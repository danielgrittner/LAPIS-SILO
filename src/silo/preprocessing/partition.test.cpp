#include "silo/preprocessing/partition.h"

#include "silo/preprocessing/preprocessing_config.h"

#include <gtest/gtest.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

using silo::preprocessing::Chunk;
using silo::preprocessing::Partition;
using silo::preprocessing::Partitions;

Partitions createSimplePartitionsObject() {
   std::vector<Chunk> chunks1;
   chunks1.emplace_back(std::vector<std::string>{{"A.1", "A.3", "B.1", "A.2"}}, 8);
   chunks1.emplace_back(std::vector<std::string>{{"B.2", "C.3", "C.1", "C.2"}}, 11123);
   Partition partition1(std::move(chunks1));
   std::vector<Chunk> chunks2;
   chunks2.emplace_back(std::vector<std::string>{{"XY.1", "XY.3", "XY.A.A.A.3", "XY.2312"}}, 123);
   chunks2.emplace_back(std::vector<std::string>{{"XT.1", "XT.3", "XTA.A.3", "XT.2312"}}, 512);
   Partition partition2(std::move(chunks2));
   return Partitions{{std::move(partition1), std::move(partition2)}};
}

std::string getExpectedSimplePartitionsDump() {
   return "[\n"
          "    [\n"
          "        {\n"
          "            \"count\": 8,\n"
          "            \"lineages\": [\n"
          "                \"A.1\",\n"
          "                \"A.2\",\n"
          "                \"A.3\",\n"
          "                \"B.1\"\n"
          "            ]\n"
          "        },\n"
          "        {\n"
          "            \"count\": 11123,\n"
          "            \"lineages\": [\n"
          "                \"B.2\",\n"
          "                \"C.1\",\n"
          "                \"C.2\",\n"
          "                \"C.3\"\n"
          "            ]\n"
          "        }\n"
          "    ],\n"
          "    [\n"
          "        {\n"
          "            \"count\": 123,\n"
          "            \"lineages\": [\n"
          "                \"XY.1\",\n"
          "                \"XY.2312\",\n"
          "                \"XY.3\",\n"
          "                \"XY.A.A.A.3\"\n"
          "            ]\n"
          "        },\n"
          "        {\n"
          "            \"count\": 512,\n"
          "            \"lineages\": [\n"
          "                \"XT.1\",\n"
          "                \"XT.2312\",\n"
          "                \"XT.3\",\n"
          "                \"XTA.A.3\"\n"
          "            ]\n"
          "        }\n"
          "    ]\n"
          "]\n";
}

TEST(Partitions, shouldSaveSimpleConfig) {
   const Partitions under_test = createSimplePartitionsObject();
   std::ofstream out_file("output/test.partitions");
   under_test.save(out_file);
   out_file.close();
   const std::ifstream in_file("output/test.partitions");

   std::stringstream buffer;
   buffer << (in_file.rdbuf());
   const std::string file_contents = buffer.str();
   const std::string expected_file_contents = getExpectedSimplePartitionsDump();

   ASSERT_EQ(file_contents, expected_file_contents);
}

TEST(Partitions, shouldSaveAndLoadSimpleConfig) {
   const Partitions partitions = createSimplePartitionsObject();
   std::ofstream out_file("output/test.partitions");
   partitions.save(out_file);
   out_file.close();
   std::ifstream in_file("output/test.partitions");

   const Partitions under_test = Partitions::load(in_file);
   auto partition_chunks = partitions.getPartitionChunks();
   auto partition_chunks_under_test = under_test.getPartitionChunks();
   ASSERT_EQ(partition_chunks.size(), partition_chunks_under_test.size());
   for (size_t i = 0; i < partition_chunks.size(); ++i) {
      ASSERT_EQ(partition_chunks[i], partition_chunks_under_test[i]);
   }
}