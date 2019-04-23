#include "DeviceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "rapidxml.hpp"
#include <boost/log/trivial.hpp>

namespace ClusterController
{
    //using namespace boost::property_tree::detail;

    DeviceManager* DeviceManager::spInstance = NULL;

    DeviceManager::DeviceManager()
    {
    }

    DeviceManager* DeviceManager::getInstance()
    {
        if(!spInstance)
        {
            spInstance = new DeviceManager();
        }
        return spInstance;
    }

    bool DeviceManager::loadDevices()
    {
        rapidxml::xml_document<> doc;
        std::ifstream file("DeviceManager.xml");

        if(!file)
        {
            BOOST_LOG_TRIVIAL(fatal) << "Cannot open file: DeviceManager.xml";
            return true;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string content(buffer.str());
        doc.parse<0>(&content[0]);
        rapidxml::xml_node<> *root(doc.first_node());

        if(!root->first_node())
        {
            BOOST_LOG_TRIVIAL(fatal) << "Cannot parse XML file: DeviceManager.xml";
            return true;
        }

        rapidxml::xml_node<> *cRoot = root->first_node("devices");

        if(!cRoot)
        {
            BOOST_LOG_TRIVIAL(fatal) << "Cannot parse devices: DeviceManager.xml";
            return true;
        }

        for(rapidxml::xml_node<> *pNode = cRoot->first_node(); pNode; pNode = pNode->next_sibling())
        {
            std::string name(pNode->first_attribute("name")->value());
            std::string ipAddress(pNode->first_attribute("ip")->value());
            Features feature;
            for(rapidxml::xml_node<> *qNode = pNode->first_node(); qNode; qNode = qNode->next_sibling())
            {
                if(std::string(pNode->first_node()->name()).compare("led") == 0)
                {
                    feature.insertLed(qNode->first_attribute("pin")->value());
                }
                if(std::string(pNode->first_node()->name()).compare("button") == 0)
                {
                    feature.insertButton(qNode->first_attribute("pin")->value());
                }
            }

            m_devicesMap.insert(std::make_pair(name, td_device(boost::asio::ip::address::from_string(ipAddress), feature)));
        }

        return false;
    }

    DeviceManager::~DeviceManager()
    {
    }
}