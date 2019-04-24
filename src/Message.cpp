#include "Message.hpp"
#include "TcpServer.hpp"
#include "DeviceManager.hpp"
#include <iostream>

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

    bool Message::mouldPacketPayload(boost::asio::streambuf& m_txBuffer)
    {
        //m_txBuffer.consume(m_txBuffer.size());

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

        if(m_txBuffer.size() != m_header.length)
        {
            std::cout << "Couldn't send message: Length Incoherence" << std::endl;
            return false;
        }

        return true;
    }

    // boost::asio::const_buffer Message::getMessageBuffer()
    // {
    //     return boost::asio::buffer(m_txBuffer.data());
    // }
}
