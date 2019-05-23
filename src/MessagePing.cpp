#include "MessagePing.hpp"
#include "DeviceManager.hpp"
#include <iostream>

namespace ClusterController
{
    MessagePing::MessagePing() : m_header(e_MSG_PING)
    {
    }

    MessagePing::MessagePing(const MessageHeader& header) : m_header(header)
    {
    }

    bool MessagePing::mouldMessage(boost::asio::streambuf& txBuffer)
    {
        m_header.setLength(MessageHeader::cHeaderLength + sizeof(END_OF_MESSAGE));

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

    bool MessagePing::decomposeMessage(MessageHeader msgHeader, boost::asio::streambuf& rxBuffer)
    {
        m_header = msgHeader;
        
        CLUSTER_LOG(info) << "Decomposed message - type:" << m_header.getMessageType() ;

        if(rxBuffer.size() != sizeof(END_OF_MESSAGE))
        {
            CLUSTER_LOG(fatal) << "Unexpected amount of data in the buffer: " << rxBuffer.size();
            rxBuffer.consume(rxBuffer.size());
            return false;
        }

        rxBuffer.consume(sizeof(END_OF_MESSAGE));

        return true;
    }

    MessageType MessagePing::getMessageType()
    {
        return m_header.getMessageType();
    }

    void MessagePing::readAdditionalVariables()
    {
        //nothing here
    }

}
