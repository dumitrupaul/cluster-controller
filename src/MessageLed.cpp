#include "MessageLed.hpp"
#include <iostream>

namespace ClusterController
{
    MessageLed::MessageLed() : m_header(e_MSG_LED)
    {
        //TODO
        m_action = e_Toggle_Led;
    }

    MessageLed::MessageLed(const MessageHeader& header) : m_header(header)
    {
        //TODO
        m_action = e_Toggle_Led;
    }

    bool MessageLed::mouldMessage(boost::asio::streambuf& txBuffer)
    {
        m_header.setLength(MessageHeader::cHeaderLength + sizeof(m_action) + sizeof(END_OF_MESSAGE));

        txBuffer.consume(txBuffer.size());

        m_header.encodeHeader(txBuffer);

        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(m_action)),
                            boost::asio::buffer(&(m_action),sizeof(m_action))));
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

    bool MessageLed::decomposeMessage(MessageHeader msgHeader, boost::asio::streambuf& rxBuffer)
    {
        m_header = msgHeader;
        
        std::istream is(&rxBuffer);
        char buf[MAX_MSG_SIZE - m_header.getLength()];

        if(!is)
        {
            CLUSTER_LOG(info) << "Could not open stream from buffer";
            rxBuffer.consume(rxBuffer.size());
            return false;
        }

        is.read(buf, sizeof(m_action));
        if(!is) 
        {
            CLUSTER_LOG(info) << "Buffer failure while decomposing message only "
                                    << is.gcount() << " bytes could be read, instead of " << sizeof(m_action);
            rxBuffer.consume(rxBuffer.size());
            return false;
        }

        m_action = *reinterpret_cast<LedAction const*>(buf);

        if(rxBuffer.size() != sizeof(END_OF_MESSAGE))
        {
            CLUSTER_LOG(fatal) << "Unexpected amount of data in the buffer: " << rxBuffer.size();
            rxBuffer.consume(rxBuffer.size());
            return false;
        }

        CLUSTER_LOG(info) << "Decomposed message - type:" << m_header.getMessageType()
                                << " - LedAction:" << m_action;

        rxBuffer.consume(sizeof(END_OF_MESSAGE));

        return true;
    }

    MessageType MessageLed::getMessageType()
    {
        return m_header.getMessageType();
    }

    void MessageLed::setLedAction(LedAction action)
    {
        m_action = action;
    }

    LedAction MessageLed::getLedAction() const
    {
        return m_action;
    }

    void MessageLed::readAdditionalVariables()
    {
        uint32_t action;
        std::cout << "Insert LED Action:\n\t[0 - Toggle]\t[1 - Blink]\n";
        std::cin >> action;
        m_action = static_cast<LedAction>(action);

        if(m_action != e_Toggle_Led && m_action != e_Blink_Led)
        {
            std::cout << "Invalid action entered: " << m_action << "\n";
            readAdditionalVariables();
        }
    }

}
