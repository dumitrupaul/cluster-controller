#include "MessagePing.hpp"
#include "DeviceManager.hpp"
#include <iostream>
#include <boost/log/trivial.hpp>

namespace ClusterController
{
    MessagePing::MessagePing()
    {
        m_header.setMessageType(e_MSG_PING);
        char ipAddress[32];
        strcpy(ipAddress, DeviceManager::getInstance()->getMyIpAddress().c_str());
        m_header.setIpAddress(ipAddress);
    }

    MessagePing::MessagePing(const MessageHeader& header) : m_header(header)
    {
    }

    MessagePing::~MessagePing()
    {
    }

    bool MessagePing::mouldMessage(boost::asio::streambuf& txBuffer)
    {
        m_header.setLength(m_header.headerLength + sizeof(END_OF_MESSAGE));

        txBuffer.consume(txBuffer.size());

        m_header.encodeHeader(txBuffer);
        
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(END_OF_MESSAGE)),
                            boost::asio::buffer(&(END_OF_MESSAGE),sizeof(END_OF_MESSAGE))));

        assert(txBuffer.size() < MAX_MSG_SIZE && "Size of the message exceeded");
        if(txBuffer.size() != m_header.getLength())
        {
            std::cout << "Couldn't mould message: Length Incoherence("<< 
                        txBuffer.size() << ":" << m_header.getLength() << ")" << std::endl;
            txBuffer.consume(txBuffer.size());
            return false;
        }

        return true;
    }

    bool MessagePing::decomposeMessage(boost::asio::streambuf& rxBuffer)
    {
        BOOST_LOG_TRIVIAL(info) << "Decomposed message - type:" << m_header.getMessageType() ;

        if(rxBuffer.size() != sizeof(END_OF_MESSAGE))
        {
            BOOST_LOG_TRIVIAL(fatal) << "Unexpected amount of data in the buffer";
            rxBuffer.consume(rxBuffer.size());
            return false;
        }

        rxBuffer.consume(sizeof(END_OF_MESSAGE));

        return true;
    }

    MessageType MessagePing::getMessageType() const
    {
        return m_header.getMessageType();
    }

    void MessagePing::readAdditionalVariables()
    {
        //nothing here
    }

}
