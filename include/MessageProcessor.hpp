#ifndef MESSAGEPROCESSOR_HPP
#define MESSAGEPROCESSOR_HPP
#include <boost/asio.hpp>
#include "MessageHeader.hpp"

namespace ClusterController
{
    class Message_I;

    class MessageProcessor
    {
        public:
            static bool processReceivedMessage(boost::asio::streambuf& rxBuffer);

            static bool processSentMessageType(boost::asio::streambuf& txBuffer, uint32_t msgType);

        private:
            static bool createMessageFromType(MessageType msgType, std::unique_ptr<Message_I>& msg, bool logType);

            MessageProcessor() {}

    };
}
#endif //MESSAGEPROCESSOR_HPP