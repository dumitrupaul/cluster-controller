#include "LocalSSLClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include "MessageProcessor.hpp"
#include <wiringPi.h>

#define DEBOUNCE_DELAY 1000

namespace ClusterController
{

    LocalSSLClient::LocalSSLClient(boost::asio::io_service &io_service, boost::asio::ssl::context& context, bool aMode) : 
                            autoMode(aMode),
                            sentMessageTimeInMs(0),
                            mSocket(io_service, context),
                            mEndpoint(tcp::v4(), COMMUNICATION_PORT)                            
    {
        if(autoMode){
            std::cout << "Autonomous GPIO Mode engaged. No input necessary.\n";
            DeviceManager::getInstance()->getMyFeatures().printFeatures();
        }

        mSocket.set_verify_mode(boost::asio::ssl::verify_peer);
        mSocket.set_verify_callback(std::bind(&LocalSSLClient::verifyCertificate, this, 
                                              std::placeholders::_1, std::placeholders::_2));
        startConnection();
    }

    bool LocalSSLClient::verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "[SSL]Verifying " << subject_name << "\n";

        return preverified;
    }

    void LocalSSLClient::startConnection()
    {  
        if(autoMode)
            handleGPIO();
        else
            handleInput();

        mSocket.lowest_layer().async_connect(mEndpoint, boost::bind(&LocalSSLClient::onConnect, this, 
                                                                    boost::asio::placeholders::error));
    }

    void LocalSSLClient::onConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            //connection succeeded
            std::cout << "[SSL]Connection succeeded. Starting handshake...\n";
            doHandshake();
        }
        else
        {
            if(error == boost::asio::error::connection_aborted) 
                std::cout << "Device disconnected: ";
            if(error == boost::asio::error::host_unreachable)
                std::cout << "Couldn't connect in time. Make sure batman-adv is properly configured "
                                "and the device is connected to the mesh: ";
            std::cout << error.message() << std::endl;

            shutdownConnection();
        }
    }

    void LocalSSLClient::doHandshake()
    {
        mSocket.async_handshake(boost::asio::ssl::stream_base::client,
            [this](const boost::system::error_code& error)
            {
                if (!error)
                {
                    std::cout << "[SSL]Handshake succeeded. Sending message...\n";
                    writeMessage();
                }
                else
                {
                    std::cout << "[SSL]Handshake failed: " << error.message() << "\n";
                    shutdownConnection();
                }
            });
    }

    void LocalSSLClient::writeMessage()
    {
        boost::asio::async_write(mSocket, mTxBuffer.data(),
                                boost::bind(&LocalSSLClient::onWrite, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void LocalSSLClient::onWrite(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            std::cout << "Message transmitted successfully [size:"<< bytes_transferred << "]" << std::endl;
        }
        else
        {
            std::cout << "Can't write message, reason: " << error.message() << std::endl;
        }

        //close the socket after every message sent
        shutdownConnection();
        
    }

    void LocalSSLClient::shutdownConnection()
    {
        mSocket.async_shutdown([this](const boost::system::error_code& error)
        {
            
            mSocket.lowest_layer().close();
            
            //https://stackoverflow.com/questions/50693708/boost-asio-ssl-not-able-to-receive-data-for-2nd-time-onwards-1st-time-ok
            SSL_clear(mSocket.native_handle());

            startConnection();
        });
        
    }

    void LocalSSLClient::handleInput()
    {
        using namespace std;

        vector<string> names(DeviceManager::getInstance()->getNames());
        uint32_t selectedIdx;
        uint32_t msgType;
        uint32_t printIdx = 0;
        cout << endl << "Select a device from the list:" << endl;

        for (vector<string>::const_iterator i = names.begin(); i != names.end(); ++i, ++printIdx)
        {
            cout << "\t[" << printIdx <<" - " << *i << "]" << endl;
        }

        cin >> selectedIdx;

        //Input validation
        while(selectedIdx < 0 || selectedIdx > names.size() - 1)
        {
            cout << "Invalid selection\n";
            cin >> selectedIdx;
        }
        
        mEndpoint.address(DeviceManager::getInstance()->getIPfromName(names[selectedIdx]));
        cout << endl << "Select the type of message you want to send: \n\t[0 - PING] \n\t[1 - LED]\n";

        cin >> msgType;

        //TODO: Input validation

        if(!MessageProcessor::processSentMessageType(mTxBuffer, msgType))
            handleInput();

    }
    
    void LocalSSLClient::handleGPIO()
    {
        Features& features = DeviceManager::getInstance()->getMyFeatures();
        
        std::vector<Button>& buttons = features.getButtonList();
        
        for(uint32_t buttonIdx = 0; buttonIdx < buttons.size(); ++buttonIdx)
        {        
            pinMode(buttons[buttonIdx].pinNumber, INPUT);
            pullUpDnControl(buttons[buttonIdx].pinNumber, PUD_UP);
        }
        
        bool buttonPressed = false;
        uint32_t currentTimeInMs;
        
        while(!buttonPressed)
        {
            currentTimeInMs = millis();
            if(currentTimeInMs - sentMessageTimeInMs > DEBOUNCE_DELAY)
            {
                bool returnValue = scanButtons(buttons);
                if(returnValue)
                    buttonPressed = true;
            }
        }
        
    }
    
    bool LocalSSLClient::scanButtons(std::vector<Button>& buttons)
    {
        for(uint32_t buttonIdx = 0; buttonIdx < buttons.size(); ++buttonIdx)
        { 
            if(digitalRead(buttons[buttonIdx].pinNumber) == LOW)
            {
                sentMessageTimeInMs = millis();
                
                if(!MessageProcessor::processSentMessagePtr(mTxBuffer, buttons[buttonIdx].sendMsg))
                    return false;
                
                mEndpoint.address(buttons[buttonIdx].conn);
                return true;
            }
        }
        return false;
        
        //if(digitalRead(buttons[1].pinNumber) == LOW)
        //{
            //MessageProcessor::processSentMessagePtr(mTxBuffer, buttons[0].sendMsg);
            //sentMessageTimeInMs = millis();
            //m_ipAddress = buttons[0].conn;
            
            //return true;
        //}
    }
}
