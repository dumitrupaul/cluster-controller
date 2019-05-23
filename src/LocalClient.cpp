#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "MessageProcessor.hpp"

namespace ClusterController
{

    LocalClient::LocalClient(boost::asio::io_service &io_service, boost::asio::ssl::context& context) : 
                            m_serverPort(COMMUNICATION_PORT),
                            m_socket(io_service, context)
    {
        m_socket.set_verify_mode(boost::asio::ssl::verify_peer);
        m_socket.set_verify_callback(std::bind(&LocalClient::verifyCertificate, this, std::placeholders::_1, std::placeholders::_2));

        startConnection();
    }

    bool LocalClient::verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
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
        std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }

    void LocalClient::startConnection()
    {  
        handleInput();

        tcp::endpoint endPoint(m_ipAddress, m_serverPort);
        m_socket.lowest_layer().async_connect(endPoint, boost::bind(&LocalClient::onConnect, this, boost::asio::placeholders::error));
    }

    void LocalClient::onConnect(const boost::system::error_code &error)
    {
        if (!error)
        {
            //connection succeeded
            std::cout << "Connection from the client succeeded. Starting handshake...\n";
            handshake();
        }
        else
        {
            if(error == boost::asio::error::connection_aborted) std::cout << "Device disconnected: ";
            if(error == boost::asio::error::host_unreachable)
                std::cout << "Couldn't connect in time. Make sure batman-adv is properly configured "
                                "and the device is connected to the mesh: ";
            std::cout << error.message() << std::endl;

            m_socket.lowest_layer().close();
            // m_socket.async_shutdown([this](...){
            //     m_socket.lowest_layer().close();
            // });
            startConnection();
        }
    }

    void LocalClient::handshake()
    {
        m_socket.async_handshake(boost::asio::ssl::stream_base::client,
            [this](const boost::system::error_code& error)
            {
                if (!error)
                {
                    std::cout << "Handshake succeeded. Sending message...\n";
                    writeMessage();
                }
                else
                {
                    std::cout << "Handshake failed: " << error.message() << "\n";
                }
            });
    }

    void LocalClient::writeMessage()
    {
        boost::asio::async_write(m_socket, m_txBuffer.data(),
                                boost::bind(&LocalClient::onWrite, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void LocalClient::onWrite(const boost::system::error_code &error, size_t bytes_transferred)
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
        m_socket.lowest_layer().close();
        // m_socket.async_shutdown([this](...){
        //     m_socket.lowest_layer().close();
        // });
        startConnection();
    }

    void LocalClient::handleInput()
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
        
        m_ipAddress = DeviceManager::getInstance()->getIPfromName(names[selectedIdx]);
        cout << endl << "Select the type of message you want to send: \n\t[0 - PING] \n\t[1 - LED]\n";

        cin >> msgType;

        //TODO: Input validation

        if(!MessageProcessor::processSentMessageType(m_txBuffer, msgType))
            handleInput();

    }
}