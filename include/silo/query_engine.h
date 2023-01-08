//
// Created by Alexander Taepper on 27.09.22.
//

#ifndef SILO_QUERY_ENGINE_H
#define SILO_QUERY_ENGINE_H

#include "silo/database.h"
#include <string>

namespace silo {

struct QueryParseException : public std::exception {
   private:
   const char* message;

   public:
   explicit QueryParseException(const std::string& msg) : message(msg.c_str()) {}

   [[nodiscard]] const char* what() const noexcept override {
      return message;
   }
};

struct result_s {
   std::string return_message;
   int64_t parse_time;
   int64_t filter_time;
   int64_t action_time;
};

result_s execute_query(const Database& db, const std::string& query, std::ostream& res_out, std::ostream& perf_out);

} // namespace silo;

#endif //SILO_QUERY_ENGINE_H
