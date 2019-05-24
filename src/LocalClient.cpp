#include "LocalClient.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include "DeviceManager.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "MessageProcessor.hpp"

namespace ClusterController
{
    const char LocalClient::closeBuffer[1] = "";

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
            doHandshake();
        }
        else
        {
            if(error == boost::asio::error::connection_aborted) std::cout << "Device disconnected: ";
            if(error == boost::asio::error::host_unreachable)
                std::cout << "Couldn't connect in time. Make sure batman-adv is properly configured "
                                "and the device is connected to the mesh: ";
            std::cout << error.message() << std::endl;

            shutdownConnection();
        }
    }

    void LocalClient::doHandshake()
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
                    shutdownConnection();
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
        shutdownConnection();
        
    }

    void LocalClient::shutdownConnection()
    {
        // m_socket.async_shutdown([this](...)
        // {
        //     std::cout << "CRAPAT";
        //     boost::asio::async_write(m_socket, boost::asio::buffer(closeBuffer, 1), 
        //                             [this](const boost::system::error_code& error,
        //                                     size_t bytes_transferred)
        //     {
        //         std::cout << "CRAPAT";
        //         if ((error.category() == boost::asio::error::get_ssl_category())
        //             && (SSL_R_PROTOCOL_IS_SHUTDOWN == ERR_GET_REASON(error.value())))
        //         {
        //             std::cout << "CRAPAT";
        //             m_socket.lowest_layer().close();
        //             SSL_clear(m_socket.native_handle());
        //             startConnection();
        //         }
        //         std::cout << "CRAPAT";
        //     });
        // });
        m_socket.async_shutdown([this](const boost::system::error_code& error)
        {
            
            m_socket.lowest_layer().close();
            
            //https://stackoverflow.com/questions/50693708/boost-asio-ssl-not-able-to-receive-data-for-2nd-time-onwards-1st-time-ok
            SSL_clear(m_socket.native_handle());

            startConnection();
        });
        
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