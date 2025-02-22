#ifndef SILO_INCLUDE_SILO_CONFIG_CONFIG_EXCEPTION_H_
#define SILO_INCLUDE_SILO_CONFIG_CONFIG_EXCEPTION_H_

#include <iostream>
#include <stdexcept>
#include <string>

namespace silo::config {

class ConfigException : public std::runtime_error {
  public:
   explicit ConfigException(const std::string& error_message);
};

}  // namespace silo::config

#endif  // SILO_INCLUDE_SILO_CONFIG_CONFIG_EXCEPTION_H_
