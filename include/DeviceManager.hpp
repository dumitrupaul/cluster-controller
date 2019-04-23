#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP
#include <map>
#include <boost/asio.hpp>
#include "Features.h"

namespace ClusterController
{
    typedef std::pair<boost::asio::ip::address, Features> td_device;
    class DeviceManager
    {
        public:
            static DeviceManager* getInstance();

            bool loadDevices();

            std::vector<std::string> getNames();

            boost::asio::ip::address getIPfromName(std::string& name);

        private:
            void printDeviceMap();
            
            DeviceManager();

            ~DeviceManager();

            static DeviceManager* spInstance;
            std::map<std::string, td_device> m_devicesMap;  
    };
}

#endif //DEVICEMANAGER_HPP