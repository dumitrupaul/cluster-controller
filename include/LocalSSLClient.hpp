#ifndef LocalSSLClient_HPP
#define LocalSSLClient_HPP
#include "ClusterIncludes.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <vector>
#include "Features.hpp"

namespace ClusterController
{
    using boost::asio::ip::tcp;

    class LocalSSLClient
    {
    public:
        LocalSSLClient(boost::asio::io_service &io_service, boost::asio::ssl::context& context, bool aMode);

        void startConnection();

    private:
        bool verifyCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);

        void onConnect(const boost::system::error_code &error);

        void doHandshake();

        void shutdownConnection();

        void writeMessage();
        
        void handleGPIO();

        void handleInput();
        
        bool scanButtons(std::vector<Button>& buttons);

        void onWrite(const boost::system::error_code &error, size_t bytes_transferred);
        
        const bool autoMode;
        uint32_t sentMessageTimeInMs;
        boost::asio::ssl::stream<tcp::socket> mSocket;
        boost::asio::streambuf mTxBuffer;
        tcp::endpoint mEndpoint;
    };
}
#endif //LocalSSLClient_HPP
