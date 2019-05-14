#include "MessageProcessor.hpp"
#include "MessagePing.hpp"
#include "MessageLed.hpp"
#include "DeviceManager.hpp"
#include <iostream>

namespace ClusterController
{
    bool MessageProcessor::processReceivedMessage(boost::asio::streambuf& rxBuffer)
    {
        MessageHeader recvMsgHeader;
        
        if(recvMsgHeader.decodeHeader(rxBuffer))
        {
            std::unique_ptr<Message_I> msg = createMessageFromType(recvMsgHeader.getMessageType(), false);
            if(!msg)
                return false;
            
            if(!msg->decomposeMessage(recvMsgHeader, rxBuffer))
                return false;
            
            DeviceManager::getInstance()->processReceivedMessage(std::move(msg));
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
        if(!msg->mouldMessage(txBuffer))
            return false;

        return true;
    }
    
    bool MessageProcessor::processSentMessagePtr(boost::asio::streambuf& txBuffer, std::shared_ptr<Message_I> msg)
    {
        if(!msg->mouldMessage(txBuffer))
            return false;
        
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
                    CLUSTER_LOG(error) << "Invalid message type received. Discarding.";
                return nullptr;
                break;
            }
        }

        return msg;
    }

}
