

#include <memory>
#include <vector>
#include <optional>
#include <functional>
#include <utility>
#include <iostream>

#include "rnp_networkmanager.h"
#include "rnp_packet.h"
#include "rnp_routingtable.h"
#include "rnp_netman_packets.h"
#include "rnp_default_address.h"

#if defined(ARDUINO)
    #include <Arduino.h>
#endif



RnpNetworkManager::RnpNetworkManager(uint8_t address, NODETYPE nodeType, bool enableLogging):
serviceLookup(1), // initalize single element for NETMAN service
_config({address,nodeType,NOROUTE_ACTION::DUMP,false}),
routingtable(1),
_loggingEnabled(enableLogging)
{
    addInterface(&lo); // add loopback interface
    generateDefaultRoutes();
};


RnpNetworkManager::RnpNetworkManager(RnpNetworkManagerConfig config, bool enableLogging):
serviceLookup(1), // initalize single element for NETMAN service
_config(config),
routingtable(1),
_loggingEnabled(enableLogging)
{
    addInterface(&lo); // add loopback interface
    generateDefaultRoutes();
};


void RnpNetworkManager::update(){
    for (auto iface_ptr : ifaceList){
        if (iface_ptr != nullptr){
            iface_ptr->update(); // call update on all interfaces 
        }
    }
    routePackets();
}


void RnpNetworkManager::reset() 
{
    routingtable = _basetable; // replace the current routing table with the original routing tbale loaded 
    generateDefaultRoutes();
}


void RnpNetworkManager::reconfigure(RnpNetworkManagerConfig config,RoutingTable newroutingtable){
    setRoutingTable(newroutingtable); // load new routing table first so that if there is a mismatch between the new config address and the routing table, we stil can communicate with the node
    loadconfig(config);
};


void RnpNetworkManager::loadconfig(RnpNetworkManagerConfig config){
    setAddress(config.currentAddress); // call set address to clean up old address loopback route
    _config = config; 
};


void RnpNetworkManager::setRoutingTable(RoutingTable newroutingtable){
    routingtable = newroutingtable; //keep base routing table intact incase this screws the config
    generateDefaultRoutes(); // ensure the default routes always exist so we can always communicate with the node
};



void RnpNetworkManager::sendPacket(RnpPacket& packet){

    packet.header.hops += 1; // increment hops

    uint8_t destination = packet.header.destination;
    
    std::optional<Route> route = routingtable.getRoute(destination);
    if (!route){
        //bad entry / no entry returned
        switch (_config.noRouteAction){
            case NOROUTE_ACTION::DUMP:
            {
                return;
            }
            case NOROUTE_ACTION::BROADCAST:
            {
                auto broadcastPacket = [&packet,this](uint8_t ifaceID){
                    if ((ifaceID == packet.header.src_iface) || (ifaceID == static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK)))
                        {
                            return; // dont broadcast packet over the interface it was received
                        }
                        sendByRoute({ifaceID, 0, {}}, packet); // metric is not important and link layer addressing is ignored.
                };

                if (_broadcastList.size() == 0){//broadcast over all avalibale interfaces if no broadcast list is provided
                    for (int i = 0; i < ifaceList.size();i++){broadcastPacket(i);}
                }else{
                    for (uint8_t ifaceID : _broadcastList){broadcastPacket(ifaceID);}
                }     
                
            }
            default:
            {
                return;
            }
        }
    }
    //determine if we are forwarding the packet
    if (packet.header.source != _config.currentAddress){
        if (packet.header.src_iface == route.value().iface){
            return; //avoid forwarding a packet over the interface it was received to prevent packet duplication
        }
    }
    sendByRoute(route.value(),packet);
   
}


void RnpNetworkManager::sendByRoute(const Route& route, RnpPacket& packet) 
{
    //get the corresponding interface index
    uint8_t ifaceID = route.iface;
    if (ifaceID == static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK) && (packet.header.destination != _config.currentAddress)){
        //bad route
        log("[E] bad route, dest and curr address dont match when sending over loopback");
        return;
    }

    std::optional<RnpInterface*> iface_ptr = getInterface(ifaceID);
    if (!iface_ptr){
        //invalid interface entry returned
        log("[E] invalid/non-existant interface");
        return; 
    }

    packet.header.lladdress = route.address;
    iface_ptr.value()->sendPacket(packet);
};


void RnpNetworkManager::setAddress(uint8_t address) 
{

    auto currentRoute = routingtable.getRoute(_config.currentAddress);
    if (currentRoute && (currentRoute.value().iface == static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK))){
        routingtable.deleteRoute(_config.currentAddress); // ensure we dont delete a new route if this is called after a new routing table is assigned
    }
    _config.currentAddress = address;
    generateDefaultRoutes();
   
};


void RnpNetworkManager::setNodeType(NODETYPE nodeType) 
{
    _config.nodeType = nodeType;
};


void RnpNetworkManager::addInterface(RnpInterface* iface){
    uint8_t ifaceID = iface->getID();
    //check if id is greater than size of iface list
    if (ifaceID >= ifaceList.size()){
        ifaceList.resize(ifaceID+1);
    }
    if ((ifaceList.at(ifaceID) != nullptr) && (ifaceList.at(ifaceID) != iface)){
        //this means iface has a non unique id which is 
        //print some warning here?
        //or just ignore it
        log("[E] iface non unique id");
    }

    ifaceList.at(ifaceID) = iface;
    iface->setPacketBuffer(&packetBuffer);
};


std::optional<RnpInterface*> RnpNetworkManager::getInterface(const uint8_t ifaceID){
    if (ifaceID >= ifaceList.size()){
        log("[E] iface id out of range");
        return {};
    }
    RnpInterface* iface_ptr = ifaceList.at(ifaceID);
    if (iface_ptr == nullptr){
        return {};
    }
    return {iface_ptr};
};


void RnpNetworkManager::removeInterface(const uint8_t ifaceID){
    if (ifaceID >= ifaceList.size()){
        log("[E] iface id out of range");
        return;
    }
    RnpInterface* iface_ptr = ifaceList[ifaceID];
    iface_ptr->setPacketBuffer(nullptr);
    ifaceList.at(ifaceID) = nullptr; // remove pointer from vector
    if (ifaceID == ifaceList.size()-1){
        ifaceList.erase(ifaceList.end()); // erase last element from ifacelist if the index is the final element in the vector
    }
};


void RnpNetworkManager::removeInterface(RnpInterface* iface){
    uint8_t ifaceID = iface->getID();
    removeInterface(ifaceID);
};


const RnpInterfaceInfo* RnpNetworkManager::getInterfaceInfo(const uint8_t ifaceID){
    auto iface_ptr = getInterface(ifaceID);
    if (!iface_ptr){
        return nullptr;
    }
    return iface_ptr.value()->getInfo();
};


void RnpNetworkManager::registerService(const uint8_t serviceID, PacketHandlerCb packetHandler){
    if (serviceID == 0){ // prevent users adding a servivce at id 0
        log("[E] registerService-> Illegal Service ID provided!");
        return;
    }
    if (serviceID >= serviceLookup.size()){
        serviceLookup.resize(serviceID+1);
    }
    serviceLookup.at(serviceID) = packetHandler;
}


void RnpNetworkManager::unregisterService(const uint8_t serviceID){
    if (serviceID == 0){
        log("[E] unregisterService-> Illegal Service ID provided!");
        return;
    }
    if (serviceID >= serviceLookup.size()){
        log("[E] service ID out of range");
        return; // out of bounds
    }
    serviceLookup.at(serviceID) = PacketHandlerCb{};
    if (serviceID == serviceLookup.size()-1){
        serviceLookup.erase(serviceLookup.end());
    }
}


void RnpNetworkManager::setNoRouteAction(const NOROUTE_ACTION action,const std::vector<uint8_t> ifaces) 
{
    _config.noRouteAction = action;
    _broadcastList = ifaces;
}


void RnpNetworkManager::generateDefaultRoutes() 
{
    routingtable.setRoute(_config.currentAddress,Route{0,1,{}}); // loopback route
    routingtable.setRoute(static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG),Route{1,1,{}}); // debug port route
};


void RnpNetworkManager::routePackets(){
    if (packetBuffer.empty()){
        return;
    }
    //process packets on buffer one at a time 
    packetptr_t packet_ptr = std::move(packetBuffer.front()); //packet_ptr is now "owned" within the scope of this function so will be deleted as soon as this goes out of scope
    packetBuffer.pop(); // remove element at front

    if (_config.routeGenEnabled){//route gen only adds a new route if it didnt previously exist
        std::optional<Route> currentRoute = routingtable.getRoute(packet_ptr->header.source); // check if a route exists to the source node
        if (!currentRoute){
            Route newroute{packet_ptr->header.src_iface,packet_ptr->header.hops,packet_ptr->header.lladdress};
            routingtable.setRoute(packet_ptr->header.source,newroute);
        }
    }

    if ((packet_ptr->header.source == static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG)) && (packet_ptr->header.destination == static_cast<uint8_t>(DEFAULT_ADDRESS::NOADDRESS))){
        //need to dump any packets which arent addresed to either no address (0) or the current address
        packet_ptr->header.destination = _config.currentAddress; // process packets addressed to no address on the debug interface as local packets
       
    }

    if (packet_ptr->header.destination != _config.currentAddress){
        forwardPacket(*packet_ptr);
        return;
    }

    if ((packet_ptr->header.source == _config.currentAddress) && (packet_ptr->header.src_iface != static_cast<uint8_t>(DEFAULT_INTERFACES::LOOPBACK))){ //this can happend if a packet is badly addressed and/or a routing table is bad
        return; 
    }

    uint8_t packetService = packet_ptr->header.destination_service;
    switch(packetService){
        case static_cast<uint8_t>(DEFAULT_SERVICES::NOSERVICE): // service 0 debug passthrough -> forwards the packet over the debug interface 
        {
            packet_ptr->header.destination = static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG);
            sendPacket(*packet_ptr);
            break;
        }
        case static_cast<uint8_t>(DEFAULT_SERVICES::NETMAN): // serivce 1 network manager service 
        {
             NetManHandler(std::move(packet_ptr));
             break;
        }
        default: // pass packet to service handler 
        {
            if (packetService >= serviceLookup.size()){
                //out of bounds access
                return;
            }
            PacketHandlerCb callback = serviceLookup.at(packetService);
            if (!callback){
                //empty function pointer
                return;
            }

            callback(std::move(packet_ptr)); // no copy

            break;
        }
    }

};


void RnpNetworkManager::NetManHandler(packetptr_t packet_ptr){
    switch(static_cast<NETMAN_TYPES>(packet_ptr->header.type)){
        case NETMAN_TYPES::PING_REQ:
        {
            PingPacket pong(*packet_ptr);
            std::swap(pong.header.destination,pong.header.source);
            pong.header.type = (uint8_t)NETMAN_TYPES::PING_RES; // change type from request to response
            pong.header.destination_service = pong.header.source_service; //reply ping to orignator service
            pong.header.source_service = (uint8_t)DEFAULT_SERVICES::NETMAN; // update source service
            sendPacket(pong); 
            log("ping sent");
            break;
        }
        case NETMAN_TYPES::PING_RES:
        {
            // we receive a ping response
            //currently not handling this
            GenericRnpPacket packet(*packet_ptr);
            log("Ping received with systime of " + std::to_string(packet.data));
           
            break;
        }
        case NETMAN_TYPES::SET_ADDRESS:
        {
            GenericRnpPacket packet(*packet_ptr);
            setAddress(static_cast<uint8_t>(packet.data));
            log("Node address is now " + std::to_string(static_cast<uint8_t>(packet.data)));
            break;
        }
        case NETMAN_TYPES::SET_ROUTE:
        {
            SetRoutePacket setroutepacket(*packet_ptr);
            routingtable.setRoute(setroutepacket.destination,setroutepacket.getRoute());
            log("Route for Node " + std::to_string(setroutepacket.destination) + " has been updated");
            break;
        }
        case NETMAN_TYPES::SET_TYPE:
        {
            GenericRnpPacket packet(*packet_ptr);
            _config.nodeType = static_cast<NODETYPE>(packet.data);
            log("Node type is now " + std::to_string(static_cast<uint8_t>(packet.data)));
            break;
        }
        case NETMAN_TYPES::SET_NOROUTEACTION:
        {
            GenericRnpPacket packet(*packet_ptr);
            _config.noRouteAction = static_cast<NOROUTE_ACTION>(packet.data);
            log("Node NoRouteAction is now " + std::to_string(static_cast<uint8_t>(packet.data)));
            break;
        }
        case NETMAN_TYPES::SET_ROUTEGEN:
        {
            GenericRnpPacket packet(*packet_ptr);
            _config.routeGenEnabled = static_cast<bool>(packet.data);
            log("Node RouteGen is now " + std::to_string(static_cast<bool>(packet.data)));
            break;
        }
        case NETMAN_TYPES::SAVE_CONF:
        {
            if (_saveConfigImpl)
            {
                _saveConfigImpl(_config) ? log("Configuration Failed to Save!") : log("Configuration Saved!");   

            }else{
                log("No Save Config Implementation Provided - Not Saved!");
            }
            
            break;
        }
        case NETMAN_TYPES::RESET_NETMAN:
        {
            reset();
            break;
        }
        default:
        {
            break;
        }
    }
};


void RnpNetworkManager::forwardPacket(RnpPacket& packet){

    if (packet.header.source == static_cast<uint8_t>(DEFAULT_ADDRESS::DEBUG) && packet.header.source_service == static_cast<uint8_t>(DEFAULT_SERVICES::NOSERVICE))
    {
        packet.header.source = _config.currentAddress;
    }
    else if (_config.nodeType != NODETYPE::HUB)
    {
        return;
    }; // only forward if node is hub

    sendPacket(packet);
}


void RnpNetworkManager::log(const std::string& msg){
    if (!_loggingEnabled){
        return;
    }
    if (!_logcb){
        //no callback set use default log
        #if defined(ARDUINO)
            Serial.println(msg.c_str());
        #else
            std::cout<<msg<<"\n";
        #endif

        return;
    }
    _logcb(msg);

} ;