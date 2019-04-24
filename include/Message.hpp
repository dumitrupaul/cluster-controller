#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <boost/asio.hpp>

#define END_OF_MESSAGE "DEADBEEF"

namespace ClusterController
{
    using boost::asio::ip::tcp;

    static const uint32_t MAX_MSG_SIZE = 256;

    enum MessageType
    {
        e_MSG_PING = 0,
        e_MSG_LED = 1
    };

    struct Header
    {
        char ipAddress[32];
        MessageType type;
        boost::uint32_t length;
    };

    class Message
    {
        public:
            Message(const MessageType& msgType);

            Message(const Header& header);

            bool mouldMessage(boost::asio::streambuf& m_txBuffer);

            bool decomposeMessage(boost::asio::streambuf& m_rxBuffer);

            MessageType getMessageType();

            Message();

        private:
            Header m_header;
    };
}
#endif //MESSAGE_HPP