#include "DeviceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include "rapidxml.hpp"

namespace ClusterController
{
    DeviceManager* DeviceManager::spInstance = NULL;

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
            CLUSTER_LOG(fatal) << "Cannot open file: DeviceManager.xml";
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
            CLUSTER_LOG(fatal) << "Cannot parse XML file: DeviceManager.xml";
            return true;
        }

        rapidxml::xml_node<> *cRoot = root->first_node("devices");

        if(!cRoot)
        {
            CLUSTER_LOG(fatal) << "Cannot parse devices: DeviceManager.xml";
            return true;
        }
        std::cout << "Reading XML configuration file...";
        for(rapidxml::xml_node<> *pNode = cRoot->first_node(); pNode; pNode = pNode->next_sibling())
        {
            std::string name(pNode->first_attribute("name")->value());
            std::string ipAddress(pNode->first_attribute("ip")->value());
            Features feature;
            for(rapidxml::xml_node<> *qNode = pNode->first_node(); qNode; qNode = qNode->next_sibling())
            {
                if(std::string(qNode->name()).compare("led") == 0)
                {
                    feature.insertLed(qNode->first_attribute("pin")->value());
                }
                if(std::string(qNode->name()).compare("button") == 0)
                {
                    feature.insertButton(qNode->first_attribute("pin")->value(), 
                                         qNode->first_node("connection")->value(),
                                         qNode->last_node()->name());
                }
                if(std::string(qNode->name()).compare("currentDevice") == 0)
                {
                    mMyIpAddress = qNode->parent()->first_attribute("ip")->value();
                }
            }

            mDevicesMap.insert(std::make_pair(name, td_device(boost::asio::ip::address::from_string(ipAddress), feature)));
        }
        std::cout << "..............Done." << std::endl;
        //printDeviceMap();
        return false;
    }

    void DeviceManager::printDeviceMap()
    {
        for(std::map<std::string,td_device>::iterator it = mDevicesMap.begin(); it != mDevicesMap.end(); ++it) 
        {
            std::cout << it->first <<" : " << it->second.first.to_string() << std::endl;
            it->second.second.printFeatures();
        }
    }

    std::vector<std::string> DeviceManager::getNames()
    {
        std::vector<std::string> names;

        for(std::map<std::string,td_device>::iterator it = mDevicesMap.begin(); it != mDevicesMap.end(); ++it) 
        {
            names.push_back(it->first);
        }

        return names;
    }

    boost::asio::ip::address DeviceManager::getIPfromName(std::string& name) const
    {
        return mDevicesMap.find(name)->second.first;
    }

    const std::string& DeviceManager::getMyIpAddress() const
    {
        return mMyIpAddress;
    }

}