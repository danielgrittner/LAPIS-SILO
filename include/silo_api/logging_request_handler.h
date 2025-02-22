#ifndef SILO_INCLUDE_SILO_API_LOGGING_REQUEST_HANDLER_H_
#define SILO_INCLUDE_SILO_API_LOGGING_REQUEST_HANDLER_H_

#include <memory>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace silo_api {

class LoggingRequestHandler : public Poco::Net::HTTPRequestHandler {
  private:
   std::unique_ptr<Poco::Net::HTTPRequestHandler> wrapped_handler;

  public:
   explicit LoggingRequestHandler(Poco::Net::HTTPRequestHandler* wrapped_handler);

   void handleRequest(
      Poco::Net::HTTPServerRequest& request,
      Poco::Net::HTTPServerResponse& response
   ) override;
};
}  // namespace silo_api

#endif  // SILO_INCLUDE_SILO_API_LOGGING_REQUEST_HANDLER_H_
