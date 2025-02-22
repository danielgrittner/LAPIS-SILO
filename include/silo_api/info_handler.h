#ifndef SILO_INFOHANDLER_H
#define SILO_INFOHANDLER_H

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "silo_api/rest_resource.h"

namespace silo {
class Database;
}

namespace silo_api {

class InfoHandler : public RestResource {
  private:
   const silo::Database& database;

   void returnDetailedDatabaseInfo(Poco::Net::HTTPServerResponse& response);
   void returnSimpleDatabaseInfo(Poco::Net::HTTPServerResponse& response);

  public:
   explicit InfoHandler(const silo::Database& database);

   void get(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
      override;
};
}  // namespace silo_api

#endif  // SILO_INFOHANDLER_H
