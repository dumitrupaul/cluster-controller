#include "SSLServerConnection.hpp"
#include <boost/bind.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MessageHeader.hpp"
#include "MessageProcessor.hpp"

namespace ClusterController
{

    SSLServerConnection::SSLServerConnection(boost::asio::io_service &io_service, boost::asio::ssl::context& context) : 
                                mSocket(io_service, context)
    {
    }

    SSLServerConnection::td_tcpConnPointer SSLServerConnection::create(boost::asio::io_service &io_service, 
                                                                       boost::asio::ssl::context& context)
    {
        return td_tcpConnPointer(new SSLServerConnection(io_service, context));
    }

    SSLServerConnection::td_streamSocket &SSLServerConnection::getSocket()
    {
        return mSocket;
    }

    void SSLServerConnection::startHandshake()
    {
        CLUSTER_LOG(info) << "Starting new SSL handshake";
        auto self(shared_from_this());
        mSocket.async_handshake(boost::asio::ssl::stream_base::server, 
        [this, self](const boost::system::error_code& error)
        {
            if (!error)
            {
            startServerConnection();
            }
            else
            {
            CLUSTER_LOG(error) << "SSL Handshake failed reason: " << error.message();
            }
        });

    }
    void SSLServerConnection::startServerConnection()
    {
        
        CLUSTER_LOG(info) << "[SSL]Connected to: " 
                          << boost::lexical_cast<std::string>(mSocket.lowest_layer().remote_endpoint());

        boost::asio::async_read_until(mSocket, mRxBuffer, END_OF_MESSAGE,
                                boost::bind(&SSLServerConnection::onRead, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void SSLServerConnection::onRead(const boost::system::error_code &error, size_t bytes_transferred)
    {
        if (!error)
        {
            assert(bytes_transferred < mRxBuffer.size() && "Size of the info transferred is more than the buffer size");
            CLUSTER_LOG(info) << "Received message [bytes transferred:" << bytes_transferred << "]";

            if(!MessageProcessor::processReceivedMessage(mRxBuffer))
            {
                //delete the buffer if the process failed
                CLUSTER_LOG(info) << "Message process task failed";
                mRxBuffer.consume(mRxBuffer.size());
            }
            
        }
        else if (error == boost::asio::error::eof)
        {
            //
        }
        else
        {
            CLUSTER_LOG(error) << error.message();
        }
        auto self(shared_from_this());
        mSocket.async_shutdown([this, self](const boost::system::error_code& error)
        {
            mSocket.lowest_layer().close();

            // https://stackoverflow.com/questions/50693708/boost-asio-ssl-not-able-to-receive-data-for-2nd-time-onwards-1st-time-ok
            SSL_clear(mSocket.native_handle());
        });
        
    }

}