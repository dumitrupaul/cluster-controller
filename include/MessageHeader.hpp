#ifndef MESSAGEHEADER_HPP
#define MESSAGEHEADER_HPP
#include <boost/asio.hpp>

#define MAX_MSG_SIZE 256
#define END_OF_MESSAGE "DEADBEEF"

namespace ClusterController
{
    using boost::asio::ip::tcp;

    enum MessageType
    {
        e_MSG_PING = 0,
        e_MSG_LED = 1
    };

    class MessageHeader
    {
        public:
            const uint32_t headerLength = 40U;
            
            MessageHeader();

            MessageHeader(const MessageHeader& header);

            bool decodeHeader(boost::asio::streambuf& rxBuffer);

            void encodeHeader(boost::asio::streambuf& txBuffer);

            MessageType getMessageType() const;

            const char* getIpAddress() const;

            uint32_t getLength() const;

            void setMessageType(MessageType msgType);

            void setIpAddress(char* msgIpAddress);

            void setLength(uint32_t len);

            ~MessageHeader();

        private:
            char ipAddress[32];
            MessageType type;
            uint32_t length;
    };
}

#endif //MESSAGEHEADER_HPP
