#ifndef MESSAGELED_HPP
#define MESSAGELED_HPP
#include <boost/asio.hpp>
#include "MessageHeader.hpp"
#include "Message_I.hpp"

namespace ClusterController
{

    enum LedAction
    {
        e_Toggle_Led = 0,
        e_Blink_Led = 1
    };

    class MessageLed : public Message_I
    {
        public:
            MessageLed();

            MessageLed(const MessageHeader& header);

            virtual bool mouldMessage(boost::asio::streambuf& m_txBuffer);

            virtual bool decomposeMessage(boost::asio::streambuf& m_rxBuffer);

            virtual void readAdditionalVariables();

            void setLedAction(LedAction action);

            LedAction getLedAction() const;

            MessageType getMessageType();

            ~MessageLed();

        private:
            MessageHeader m_header;
            LedAction m_action;

    };
}
#endif // MESSAGELED_HPP