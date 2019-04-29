#include "MessageHeader.hpp"
#include <boost/log/trivial.hpp>

namespace ClusterController
{

    MessageHeader::MessageHeader() : 
        ipAddress("0.0.0.0"),
        type(e_MSG_PING),
        length(sizeof(type) + sizeof(ipAddress) + sizeof(length))
    {
    }

    MessageHeader::~MessageHeader()
    {
    }

    MessageHeader::MessageHeader(const MessageHeader& header)
    {
        strcpy(ipAddress, header.getIpAddress());
        type =  header.getMessageType();
        length = header.getLength();
    }

    bool MessageHeader::decodeHeader(boost::asio::streambuf& rxBuffer)
    {
        assert(rxBuffer.size() < MAX_MSG_SIZE && "Size of the message exceeded");

        std::istream is(&rxBuffer);
        char buf[MAX_MSG_SIZE];

        is.read(buf, sizeof(ipAddress) + sizeof(type) + sizeof(length));
        if(!is.good()) return false;

        strncpy(ipAddress, buf, sizeof(ipAddress));
        type = *reinterpret_cast<MessageType const*>(buf + sizeof(ipAddress));
        length = *reinterpret_cast<uint32_t const*>(buf + sizeof(ipAddress) + sizeof(type));

        BOOST_LOG_TRIVIAL(info) << "Decoded header - ipAddress: " << ipAddress
                                << " - type:" << type << " - len: " << length;

        return true;
    }

    void MessageHeader::encodeHeader(boost::asio::streambuf& txBuffer)
    {
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(ipAddress)),
                            boost::asio::buffer(&(ipAddress), sizeof(ipAddress))));
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(type)),
                            boost::asio::buffer(&(type),sizeof(type))));
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(length)),
                            boost::asio::buffer(&(length),sizeof(length))));
    }

    MessageType MessageHeader::getMessageType() const
    {
        return type;
    }

    const char* MessageHeader::getIpAddress() const
    {
        return ipAddress;
    }

    uint32_t MessageHeader::getLength() const
    {
        return length;
    }

    void MessageHeader::setMessageType(MessageType msgType)
    {
        type = msgType;
    }

    void MessageHeader::setIpAddress(char* msgIpAddress)
    {
        strcpy(ipAddress, msgIpAddress); 
    }

    void MessageHeader::setLength(uint32_t len)
    {
        length = len;
    }

}
