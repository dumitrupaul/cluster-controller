#include "MessageHeader.hpp"
#include "DeviceManager.hpp"

namespace ClusterController
{

    MessageHeader::MessageHeader(MessageType msgType) : 
        mType(msgType),
        mLength(cHeaderLength)
    {
        strcpy(mIpAddress, DeviceManager::getInstance()->getMyIpAddress().c_str());
    }

    MessageHeader::MessageHeader(const MessageHeader& header) :
        mType(header.mType),
        mLength(header.mLength)
    {
        strcpy(mIpAddress, header.getIpAddress());
    }

    bool MessageHeader::decodeHeader(boost::asio::streambuf& rxBuffer)
    {
        assert(rxBuffer.size() < MAX_MSG_SIZE && "Size of the message exceeded");

        std::istream is(&rxBuffer);
        char buf[MAX_MSG_SIZE];

        is.read(buf, sizeof(mIpAddress) + sizeof(mType) + sizeof(mLength));
        if(!is.good()) return false;

        strncpy(mIpAddress, buf, sizeof(mIpAddress));
        mType = *reinterpret_cast<MessageType const*>(buf + sizeof(mIpAddress));
        mLength = *reinterpret_cast<uint32_t const*>(buf + sizeof(mIpAddress) + sizeof(mType));

        CLUSTER_LOG(info) << "Decoded header - ipAddress: " << mIpAddress
                                << " - type:" << mType << " - len: " << mLength;

        return true;
    }

    void MessageHeader::encodeHeader(boost::asio::streambuf& txBuffer)
    {
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(mIpAddress)),
                            boost::asio::buffer(&(mIpAddress), sizeof(mIpAddress))));
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(mType)),
                            boost::asio::buffer(&(mType),sizeof(mType))));
        txBuffer.commit(boost::asio::buffer_copy(txBuffer.prepare(sizeof(mLength)),
                            boost::asio::buffer(&(mLength),sizeof(mLength))));
    }

    MessageType MessageHeader::getMessageType() const
    {
        return mType;
    }

    const char* MessageHeader::getIpAddress() const
    {
        return mIpAddress;
    }

    uint32_t MessageHeader::getLength() const
    {
        return mLength;
    }

    void MessageHeader::setMessageType(MessageType msgType)
    {
        mType = msgType;
    }

    void MessageHeader::setIpAddress(char* msgIpAddress)
    {
        strcpy(mIpAddress, msgIpAddress); 
    }

    void MessageHeader::setLength(uint32_t len)
    {
        mLength = len;
    }

}