#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP
#include <map>
#include <boost/asio.hpp>
#include "Features.hpp"

namespace ClusterController
{
    typedef std::pair<boost::asio::ip::address, Features> td_device;
    class DeviceManager
    {
        public:
            static DeviceManager* getInstance();

            bool loadDevices();

            std::vector<std::string> getNames();

            boost::asio::ip::address getIPfromName(std::string& name) const;

            const std::string& getMyIpAddress() const;

        private:
            //useful for debug
            void printDeviceMap();

            DeviceManager() = default;

            ~DeviceManager() = default;

            static DeviceManager* spInstance;
            std::string m_myIpAddress;
            std::map<std::string, td_device> m_devicesMap;  
    };
}

#endif //DEVICEMANAGER_HPP