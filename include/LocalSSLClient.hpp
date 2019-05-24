#ifndef LocalSSLClient_HPP
#define LocalSSLClient_HPP
#include "ClusterIncludes.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace ClusterController
{
    using boost::asio::ip::tcp;
    
    class LocalSSLClient
    {
        public:
            LocalSSLClient(boost::asio::io_service &io_service, boost::asio::ssl::context& context);

            void startConnection();

        private:
            bool verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);

            void onConnect(const boost::system::error_code &error);

            void doHandshake();

            void shutdownConnection();

            void writeMessage();

            void handleInput();

            void onWrite(const boost::system::error_code &error, size_t bytes_transferred);
            
            boost::asio::ssl::stream<tcp::socket> mSocket;
            boost::asio::streambuf mTxBuffer;
            tcp::endpoint mEndpoint;
    };
}
#endif //LocalSSLClient_HPP