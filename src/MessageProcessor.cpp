#include "MessageProcessor.hpp"
#include "MessagePing.hpp"
#include "MessageLed.hpp"
#include "DeviceManager.hpp"
#include <boost/log/trivial.hpp>
#include <boost/make_unique.hpp>
#include <iostream>

namespace ClusterController
{
    bool MessageProcessor::processReceivedMessage(boost::asio::streambuf& rxBuffer)
    {
        MessageHeader m_recvMsgHeader;
        std::unique_ptr<Message_I> msg;

        if(m_recvMsgHeader.decodeHeader(rxBuffer))
        {
            if(!createMessageFromType(m_recvMsgHeader.getMessageType(), msg, false))
                return false;
            
            msg->decomposeMessage(rxBuffer);
            
            BOOST_LOG_TRIVIAL(info) << msg->getMessageType();
            DeviceManager::getInstance()->processReceivedMessage(msg);
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
        std::unique_ptr<Message_I> msg;

        if(!createMessageFromType(e_msgType, msg, true))
            return false;

        msg->readAdditionalVariables();
        if(!msg->mouldMessage(txBuffer))
            return false;

        return true;
    }
    
    bool MessageProcessor::processSentMessagePtr(boost::asio::streambuf& txBuffer, std::shared_ptr<Message_I>& msg)
    {
        if(!msg->mouldMessage(txBuffer))
            return false;
        
        return true;
    }

    bool MessageProcessor::createMessageFromType(MessageType msgType, std::unique_ptr<Message_I>& msg, bool logType)
    {
        switch (msgType)
        {
            case e_MSG_PING:
                msg = boost::make_unique<MessagePing>();
                break;
            case e_MSG_LED:
                msg = boost::make_unique<MessageLed>();
                break;
            default:
            {
                if(logType == true)
                    std::cout << "Invalid message type entered.\n";
                else 
                    BOOST_LOG_TRIVIAL(error) << "Invalid message type received. Discarding.";
                return false;
                break;
            }
        }

        return true;
    }

}
