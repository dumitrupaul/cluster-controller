#include "MessageProcessor.hpp"
#include "MessagePing.hpp"
#include "MessageLed.hpp"
#include <boost/log/trivial.hpp>
#include <iostream>

namespace ClusterController
{
    bool MessageProcessor::processReceivedMessage(boost::asio::streambuf& rxBuffer)
    {
        MessageHeader m_recvMsgHeader;
        std::unique_ptr<Message_I> msg = createMessageFromType(m_recvMsgHeader.getMessageType(), false);

        if(m_recvMsgHeader.decodeHeader(rxBuffer))
        {
            if(!msg)
                return false;
            
            msg->decomposeMessage(rxBuffer);
        }
        else
        {
            //failed to decode header
            return false;
        }
        return true;
    }

    bool MessageProcessor::processSentMessageType(boost::asio::streambuf& txBuffer, uint32_t msgType)
    {
        MessageType e_msgType = static_cast<MessageType>(msgType);
        std::unique_ptr<Message_I> msg = createMessageFromType(e_msgType, true);

        if(!msg)
            return false;

        msg->readAdditionalVariables();
        msg->mouldMessage(txBuffer);

        return true;
    }

    std::unique_ptr<Message_I> MessageProcessor::createMessageFromType(MessageType msgType, bool logType)
    {
        std::unique_ptr<Message_I> msg;
        switch (msgType)
        {
            case e_MSG_PING:
                msg = std::unique_ptr<Message_I>(new MessagePing());
                break;
            case e_MSG_LED:
                msg = std::unique_ptr<Message_I>(new MessageLed());
                break;
            default:
            {
                if(logType == true)
                    std::cout << "Invalid message type entered.\n";
                else 
                    BOOST_LOG_TRIVIAL(error) << "Invalid message type received. Discarding.";
                return nullptr;
                break;
            }
        }

        return msg;
    }

}