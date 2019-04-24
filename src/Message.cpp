#include "Message.hpp"
#include "DeviceManager.hpp"
#include <iostream>
#include <boost/log/trivial.hpp>

namespace ClusterController
{
    Message::Message(const MessageType& msgType)
    {
        m_header.type = msgType;
        strcpy(m_header.ipAddress, DeviceManager::getInstance()->getMyIpAddress().c_str());
    }

    Message::Message()
    {
    }

    Message::Message(const Header& header) : m_header(header)
    {
    }

    bool Message::mouldMessage(boost::asio::streambuf& m_txBuffer)
    {
        m_txBuffer.consume(m_txBuffer.size());

        m_header.length = sizeof(m_header.ipAddress) + sizeof(m_header.type) + 
                          sizeof(m_header.length) + sizeof(END_OF_MESSAGE);

        m_txBuffer.commit(boost::asio::buffer_copy(m_txBuffer.prepare(sizeof(m_header.ipAddress)),
                            boost::asio::buffer(&(m_header.ipAddress),sizeof(m_header.ipAddress))));
        m_txBuffer.commit(boost::asio::buffer_copy(m_txBuffer.prepare(sizeof(m_header.type)),
                            boost::asio::buffer(&(m_header.type),sizeof(m_header.type))));
        m_txBuffer.commit(boost::asio::buffer_copy(m_txBuffer.prepare(sizeof(m_header.length)),
                            boost::asio::buffer(&(m_header.length),sizeof(m_header.length))));
        m_txBuffer.commit(boost::asio::buffer_copy(m_txBuffer.prepare(sizeof(END_OF_MESSAGE)),
                            boost::asio::buffer(&(END_OF_MESSAGE),sizeof(END_OF_MESSAGE))));

        assert(m_txBuffer.size() < MAX_MSG_SIZE && "Size of the message exceeded");
        if(m_txBuffer.size() != m_header.length)
        {
            std::cout << "Couldn't send message: Length Incoherence("<< 
                        m_txBuffer.size() << ":" << m_header.length << ")" << std::endl;
            return false;
        }

        return true;
    }

    bool Message::decomposeMessage(boost::asio::streambuf& m_rxBuffer)
    {
        assert(m_rxBuffer.size() < MAX_MSG_SIZE && "Size of the message exceeded");

        std::istream is(&m_rxBuffer);
        char buf[MAX_MSG_SIZE];

        is.read(buf, sizeof(m_header));

        strncpy(m_header.ipAddress, buf, sizeof(m_header.ipAddress));
        m_header.type = *reinterpret_cast<MessageType const*>(buf + sizeof(m_header.ipAddress));
        m_header.length = *reinterpret_cast<uint32_t const*>(buf + sizeof(m_header.ipAddress) + sizeof(m_header.type));

        BOOST_LOG_TRIVIAL(info) << "Decomposed message - ipAddress: " << m_header.ipAddress 
                                << " - type:" << m_header.type << " - len: " << m_header.length;

        m_rxBuffer.consume(sizeof(END_OF_MESSAGE));

        if(m_rxBuffer.size() != 0)
        {
            BOOST_LOG_TRIVIAL(fatal) << "Deleting the buffer failed";
            return false;
        }

        return true;
    }

    MessageType Message::getMessageType()
    {
        return m_header.type;
    }

}
