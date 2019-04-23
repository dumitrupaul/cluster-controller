#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP
#include <map>
#include <boost/asio.hpp>

namespace ClusterController
{
    struct Led
    {
        Led(char* pin):pinNumber(atoi(pin)){}
        int pinNumber;
    };
    
    struct Button
    {
        Button(char* pin):pinNumber(atoi(pin)){}
        int pinNumber;
    };

    class Features
    {
        public:
            void insertLed(char* value)
            {
                return ledList.push_back(Led(value));
            }
            void insertButton(char* value)
            {
                return buttonList.push_back(Button(value));
            }

        private:
            std::vector<Button> buttonList;
            std::vector<Led> ledList;

    };
    typedef std::pair<boost::asio::ip::address, Features> td_device;
    class DeviceManager
    {
        public:
            static DeviceManager* getInstance();
            bool loadDevices();

        private:
            DeviceManager();

            ~DeviceManager();

            static DeviceManager* spInstance;
            std::map<std::string, td_device> m_devicesMap;  
    };
}

#endif //DEVICEMANAGER_HPP