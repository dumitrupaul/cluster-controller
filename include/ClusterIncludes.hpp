#include <boost/log/trivial.hpp>

#define CLUSTER_LOG(severity) \
  BOOST_LOG_TRIVIAL(severity) << "[" << __FILE__ << ":" << __LINE__ << "] "

#define COMMUNICATION_PORT 44444
#define MAX_MSG_SIZE 256
#define END_OF_MESSAGE "DEADBEEF"