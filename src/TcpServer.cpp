#include "TcpServer.hpp"
#include <boost/bind.hpp>
#include <iostream>

namespace ClusterController
{

    TcpServer::TcpServer(boost::asio::io_service &io_service) : 
                        mServerAcceptor(io_service, tcp::endpoint(tcp::v4(), COMMUNICATION_PORT)),
                        mContext(boost::asio::ssl::context::sslv23)
    {
        mContext.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
        mContext.set_password_callback(std::bind(&TcpServer::getPassword, this));
        mContext.use_certificate_chain_file("user.crt");
        mContext.use_private_key_file("user.key", boost::asio::ssl::context::pem);
        mContext.use_tmp_dh_file("dh2048.pem");
        startAccept();
    }

    std::string TcpServer::getPassword() const
    {
        return "jamez";
    }

    void TcpServer::startAccept()
    {
        SSLServerConnection::td_tcpConnPointer newConn = SSLServerConnection::create(mServerAcceptor.get_io_service(), 
                                                                                     mContext);

        mServerAcceptor.async_accept(newConn->getSocket().lowest_layer(),
                                    boost::bind(&TcpServer::onAccept, this, newConn,
                                                boost::asio::placeholders::error));
    }

    void TcpServer::onAccept(SSLServerConnection::td_tcpConnPointer newConn, const boost::system::error_code &error)
    {
        if (!error)
        {
            newConn->startHandshake();
        }
        
        startAccept();
    }

    std::string TcpServer::getIpAddress()
    {
        boost::system::error_code error;
        boost::asio::ip::tcp::endpoint endpoint = mServerAcceptor.local_endpoint(error);
        if (error)
        {
            // An error occurred.
            CLUSTER_LOG(error) << "Can't get local IP Address";
            return nullptr;
        }

        return std::string(endpoint.address().to_string().c_str());
    }
}