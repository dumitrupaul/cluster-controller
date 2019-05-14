#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include "ClusterIncludes.hpp"
#include <boost/asio.hpp>
#include "Message_I.hpp"

namespace ClusterController
{

    class MessagePing : public Message_I
    {
        public:
            MessagePing();

            MessagePing(const MessageHeader& header);

            virtual bool mouldMessage(boost::asio::streambuf& m_txBuffer);

            virtual bool decomposeMessage(MessageHeader msgHeader, boost::asio::streambuf& m_rxBuffer);

            virtual void readAdditionalVariables();

            MessageType getMessageType();

            virtual ~MessagePing() = default;

        private:
            MessageHeader m_header;
    };
}
#endif //MESSAGE_HPP