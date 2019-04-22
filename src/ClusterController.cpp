#include "TcpConnection.hpp"
#include "TcpServer.hpp"
#include "LocalClient.hpp"
#include <boost/thread.hpp>
// #include "boost/log/trivial.hpp"
// #include "boost/log/utility/setup.hpp"

#define COMMUNICATION_PORT 44444

// static void init_log()
// {
//     static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]:  %Message%");

//     boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");

//     // Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
//     // is capped at 1mb and total is 20mb
//     boost::log::add_file_log (
//         boost::log::keywords::file_name = "ClusterController_%3N.log",
//         boost::log::keywords::rotation_size = 1 * 1024 * 1024,
//         boost::log::keywords::max_size = 20 * 1024 * 1024,
//         boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
//         boost::log::keywords::format = COMMON_FMT,
//         boost::log::keywords::auto_flush = true
//     );

//     boost::log::add_common_attributes();

//     // Only output message with INFO or higher severity in Release
// #ifndef _DEBUG
//     boost::log::core::get()->set_filter(
//         boost::log::trivial::severity >= boost::log::trivial::debug
//     );
// #endif
// }

void handleClient()
{
  boost::asio::io_service io_service;

  ClusterController::LocalClient client(io_service, COMMUNICATION_PORT);

  io_service.run();
}

int main()
{
  //init_log();

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