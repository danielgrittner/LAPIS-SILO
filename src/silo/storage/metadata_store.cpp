#include "silo/storage/metadata_store.h"

#include <ctime>

#include "silo/config/database_config.h"
#include "silo/preprocessing/metadata.h"
#include "silo/storage/pango_lineage_alias.h"

namespace silo {

std::time_t mapToTime(const std::string& value) {
   struct std::tm time_struct {};
   std::istringstream time_stream(value);
   time_stream >> std::get_time(&time_struct, "%Y-%m-%d");
   return mktime(&time_struct);
}

unsigned MetadataStore::fill(
   const std::filesystem::path& input_file,
   const PangoLineageAliasLookup& alias_key,
   const silo::config::DatabaseConfig& database_config
) {
   auto metadata_reader = silo::preprocessing::MetadataReader::getReader(input_file);

   const auto columns_to_index = std::set<std::string>{"country", "region", "division"};
   initializeColumns(database_config, columns_to_index);

   unsigned sequence_count = 0;

   const auto column_names = metadata_reader.get_col_names();
   for (auto& row : metadata_reader) {
      for (const auto& item : database_config.schema.metadata) {
         const std::string value = row[item.name].get();

         if (item.type == silo::config::DatabaseMetadataType::STRING) {
            if (columns_to_index.contains(item.name)) {
               indexed_string_columns.at(item.name).insert(value);
            } else {
               raw_string_columns.at(item.name).insert(value);
            }
         } else if (item.type == silo::config::DatabaseMetadataType::PANGOLINEAGE) {
            const std::string pango_lineage = alias_key.resolvePangoLineageAlias(value);
            pango_lineage_columns.at(item.name).insert({pango_lineage});
         } else if (item.type == silo::config::DatabaseMetadataType::DATE) {
            date_columns.at(item.name).insert(mapToTime(value));
         }
      }
      ++sequence_count;
   }

   return sequence_count;
}

void MetadataStore::initializeColumns(
   const config::DatabaseConfig& database_config,
   const std::set<std::string>& columns_to_index
) {
   for (const auto& item : database_config.schema.metadata) {
      if (item.type == config::DatabaseMetadataType::STRING) {
         if (columns_to_index.contains(item.name)) {
            this->indexed_string_columns[item.name] = storage::column::IndexedStringColumn();
         } else {
            this->raw_string_columns[item.name] = storage::column::RawStringColumn();
         }
      } else if (item.type == config::DatabaseMetadataType::PANGOLINEAGE) {
         pango_lineage_columns.emplace(item.name, storage::column::PangoLineageColumn());
      } else if (item.type == config::DatabaseMetadataType::DATE) {
         date_columns.emplace(
            item.name,
            // TODO(#114) make this configurable
            item.name == "date" ? storage::column::DateColumn(true)
                                : storage::column::DateColumn(false)
         );
      }
   }
}

}  // namespace silo
