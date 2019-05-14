#include "ClusterIncludes.hpp"
#include "TcpServer.hpp"
#include "LocalClient.hpp"
#include "DeviceManager.hpp"
#include <thread>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/keywords/format.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <wiringPi.h>

static void init_log()
{
  static const std::string COMMON_FMT("[%TimeStamp%][%Severity%]:  %Message%");

  boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

  // Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
  // is capped at 1mb and total is 20mb
  boost::log::add_file_log(
      boost::log::keywords::file_name = "ClusterController_%3N.log",
      boost::log::keywords::rotation_size = 1 * 1024 * 1024,
      boost::log::keywords::max_size = 20 * 1024 * 1024,
      boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
      boost::log::keywords::format = (
      boost::log::expressions::stream
        << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
        << ": [" << boost::log::trivial::severity
        << "] " << boost::log::expressions::smessage
      ),
      boost::log::keywords::auto_flush = true);
  

  boost::log::add_common_attributes();

// Only output message with INFO or higher severity in Release
#ifndef _DEBUG
  boost::log::core::get()->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::debug);
#endif
}

void handleClient(bool autoMode)
{
  try
  {
    boost::asio::io_service io_service;

    ClusterController::LocalClient client(io_service, COMMUNICATION_PORT, autoMode);

    io_service.run();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}

void handleServer()
{
  try
  {
    boost::asio::io_service io_service;

    ClusterController::TcpServer server(io_service, COMMUNICATION_PORT);

    io_service.run();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
}

int main(int argc, char* argv[])
{
  bool autoMode = false;
  
  if(argc > 2)
  {
    std::cerr << "Usage: " << argv[0] << " auto=<true>/<false>\n";
  }
  else if(argc == 2)
  {
    std::string arg(argv[1]);
    if(arg.compare("auto=true") == 0)
      autoMode = true;
    else
      std::cerr << "Usage: " << argv[0] << " auto=<true>/<false>. Mode auto set to false.\n";
  }

  init_log();

  if(ClusterController::DeviceManager::getInstance()->loadDevices())
  {
    return EXIT_FAILURE;
  }
  
  wiringPiSetup();
  
  std::thread ClientThread(&handleClient);
  std::thread ServerThread(&handleServer);
  ServerThread.join();
  ClientThread.join();


  return EXIT_SUCCESS;
}
