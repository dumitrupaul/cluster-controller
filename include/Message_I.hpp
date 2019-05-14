#ifndef MESSAGE_I_HPP
#define MESSAGE_I_HPP
#include <boost/asio.hpp>

namespace ClusterController
{
    class Message_I
    {
        public:
            virtual bool mouldMessage(boost::asio::streambuf& m_txBuffer) = 0;

            virtual bool decomposeMessage(boost::asio::streambuf& m_rxBuffer) = 0;

            virtual void readAdditionalVariables() = 0;

            virtual ~Message_I() = default;
    };
}
#endif //MESSAGE_I_HPP