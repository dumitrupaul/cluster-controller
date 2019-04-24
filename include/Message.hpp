#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include <boost/asio.hpp>

#define END_OF_MESSAGE "DEADBEEF"

namespace ClusterController
{
    using boost::asio::ip::tcp;

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

            bool mouldPacketPayload(boost::asio::streambuf& m_txBuffer);

            //boost::asio::const_buffer getMessageBuffer();

        private:
            Message();

            Header m_header;
            //boost::asio::streambuf m_txBuffer;
    };
}
#endif //MESSAGE_HPP