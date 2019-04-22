#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "LocalClient.hpp"
#include <boost/thread.hpp>

#define COMMUNICATION_PORT 44444

void handleClient()
{
  boost::asio::io_service io_service;

  ClusterController::LocalClient client(io_service, COMMUNICATION_PORT);

  io_service.run();
}

int main()
{
  try
  {
    // We need to create a server object to accept incoming client connections.
    boost::asio::io_service io_service_server;

    ClusterController::TcpServer server(io_service_server, COMMUNICATION_PORT);

    boost::thread th(&handleClient);
    boost::thread th1(boost::bind(&boost::asio::io_service::run, &io_service_server));
    th1.join();
    th.join();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}