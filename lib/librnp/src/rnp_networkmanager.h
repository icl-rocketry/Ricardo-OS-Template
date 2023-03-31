#pragma once

//TODO
// Provide implementation form loading from json including the nvs loads
// provide implementaiton in routng tbale from loading and serializing to json


#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <array>
#include <functional>


#include "rnp_packet.h"
#include "rnp_header.h"
#include "rnp_routingtable.h"
#include "rnp_interface.h"
#include "loopback.h"

using packetptr_t = std::unique_ptr<RnpPacketSerialized>;
using packetBuffer_t = std::queue<packetptr_t>;
using PacketHandlerCb = std::function<void(packetptr_t)>;
using LogCb_t = std::function<void(const std::string&)>;



enum class NODETYPE : uint8_t{
            LEAF=0, // only accepts packets addressed to this node and dumps any others. 
            HUB=1, // accepts all packets and forwards to the correct interface (promiscous mode)
};

enum class NOROUTE_ACTION:uint8_t{
            DUMP=0, /*! Dumps the packet !*/
            BROADCAST=1 /*! broadcast the packet to the specifed interfaces NB careful of packet duplication -> no de-duplication service has been written yet !*/
};

enum class DEFAULT_SERVICES:uint8_t{
    /**
     * @brief is reserved for 'special' uses.
     * On the Groundstation, nodes on RNP can send packets to the the command service on the groundstation without being forwarded to connected ip clients. 
     * NOSERVICE on a node which is a hub also allows packets to be forwarded to the debug interface allowing the debug interface to
     * communicate to any node on the network. This only happens if the packet is addressed to a node
     * 
     * 
     * 
     */
    NOSERVICE = 0, //Noservice 
    NETMAN = 1, // Netmanager service
    COMMAND = 2 // Command Processor Service
};

struct RnpNetworkManagerConfig{
    uint8_t currentAddress;
    NODETYPE nodeType;
    NOROUTE_ACTION noRouteAction;
    bool routeGenEnabled;
};

using SaveConfigImpl = std::function<bool(RnpNetworkManagerConfig const & config)>;


class RnpNetworkManager {

    public:
        /**
         * @brief Construct a new Rnp Network Manager object, provides a default constructor to set default settings
         * 
         * @param address 
         * @param nodeType 
         * @param enableLogging 
         */
        RnpNetworkManager(uint8_t address = 0, NODETYPE nodeType = NODETYPE::LEAF,bool enableLogging = false);
        /**
         * @brief Construct a new Rnp Network Manager object using a RnpNetworkManagerConfig struct
         * 
         * @param config 
         * @param enableLogging 
         */
        RnpNetworkManager(RnpNetworkManagerConfig config, bool enableLogging = false);

        /**
         * @brief reconfigure networkmanager 
         * 
         * @param config 
         * @param newroutingtable 
         */
        void reconfigure(RnpNetworkManagerConfig config,RoutingTable newroutingtable);

        /**
         * @brief Runs update routine on all interfaces in the iflist, also calls routePacket command to process any received packets.
         * 
         */
        void update(); 
        /**
         * @brief Resets the network configuration including routing table to the orignal loaded configuration
         * 
         */
        void reset(); //reset routing table to the loaded routing table

        /**
         *
         * @brief Load network manager config taking care to clean up old address entry in routing table
         * 
         * @param config 
         */
        void loadconfig(RnpNetworkManagerConfig config);
        
        /**
         * @brief Set a new routingtable
         * 
         * @param newroutingtable 
         */
        void setRoutingTable(RoutingTable newroutingtable);


        /**
         * @brief Sends a RnpPacket. All routing information is contained within the header of the packet.
         * 
         * @param packet RnpPacket type, typically a class derived from RnpPacket is passed
         */
        void sendPacket(RnpPacket &packet); // if u wwant to send raw data use the actual interface
        /**
         * @brief send packet over a specified route
         * 
         * @param route 
         * @param packet 
         */
        void sendByRoute(const Route& route, RnpPacket&packet);

        /**
         * @brief Set the Address of the node, this removes the previous loopback route and generates a new loopback route aswell so that the loopback route always exists
         * 
         * @param address 
         */
        void setAddress(uint8_t address); 
        /**
         * @brief Get the Address of the node.
         * 
         * @return uint8_t 
         */
        uint8_t getAddress(){return _config.currentAddress;};

        /**
         * @brief Set the Node Type
         * 
         * @param nodeType 
         */
        void setNodeType(NODETYPE nodeType); // hub or leaf
        /**
         * @brief Get the Node Type
         * 
         * @return NODETYPE 
         */
        NODETYPE getNodeType(){return _config.nodeType;};

        /**
         * @brief Add interface to iface list, places the interface at the id speicifed in the interface object
         * 
         * @param iface Pointer to interface object
         */
        void addInterface(RnpInterface* iface);
        /**
         * @brief Get the Interface object from ifacelist
         * 
         * @param ifaceID id of interface
         * @return std::optional<RnpInterface*> 
         */
        std::optional<RnpInterface*> getInterface(const uint8_t ifaceID); 
        /**
         * @brief Remove interface at iface id
         * 
         * @param ifaceID 
         */
        void removeInterface(const uint8_t ifaceID); 
        /**
         * @brief Remove interface by iface pointer 
         * 
         * @param iface 
         */
        void removeInterface(RnpInterface* iface); 

        /**
         * @brief Get Interface info
         * 
         * @param ifaceID 
         * @return RnpInterfaceInfo* 
         */
        const RnpInterfaceInfo* getInterfaceInfo(const uint8_t ifaceID);
        /**
         * @brief Get List of interfaces
         * 
         * @return std::string 
         */
        std::string getInterfaceList(){return "";}; //not implemented yet

        /**
         * @brief Register a packet callback handler for a specifed service id. Thread saftey is up to the implmentation of the callback.
         * check that the expect packet length and recieved packet length match
         * 
         * @param serviceID 
         * @param packetHandler 
         */
        void registerService(const uint8_t serviceID, PacketHandlerCb packetHandler);
        /**
         * @brief remove callback by id, ensure this is called if the callback passed gets destructed otherwise there will be UB
         * 
         * @param serviceID 
         */
        void unregisterService(const uint8_t serviceID); 

        /**
         * @brief Pass a logging function pointer to allow logging of errors from network manager
         * 
         * @param logcb 
         */
        void setLogCb(LogCb_t logcb){_logcb = logcb;};

        /**
         * @brief Enable the learning of routes by analyzing packets comming from previously unkown sources
         * 
         * @param setting 
         */
        void enableAutoRouteGen(bool setting){_config.routeGenEnabled = setting;};
        
        /**
         * @brief Set action if no route is found in the routing table.
         * 
         * @param action 
         * @param ifaces list of interface ids to broadcast the packet on
         */
        void setNoRouteAction(const NOROUTE_ACTION action,const std::vector<uint8_t> ifaces = {});

        /**
         * @brief Generates the loopback route aswell as the debug port route
         * 
         */
        void generateDefaultRoutes();
        
        /**
         * @brief updates the _basetable to the current routing table
         * 
         */
        void updateBaseTable(){_basetable = routingtable;};

        /**
         * @brief Set the save config implementation to save the current config in memory
         * 
         * @param saveConfigImpl 
         */
       void setSaveConfigImpl(SaveConfigImpl saveConfigImpl){_saveConfigImpl = saveConfigImpl;};

        
    private:

        /**
         * @brief Processes any recieved packets 
         * 
         */
        void routePackets();
            
        /**
         * @brief packet forwarding logic
         * 
         */
        void forwardPacket(RnpPacket& packet);
        /**
         * @brief Internal network management packets handler
         * 
         * @param packet_ptr 
         */
        void NetManHandler(packetptr_t packet_ptr); //internal network packet handler

        packetBuffer_t packetBuffer;

        std::vector<PacketHandlerCb> serviceLookup;
        
        Loopback lo; // loopback is by default owned by the network manager
    private:
        std::vector<RnpInterface*> ifaceList;
        /**
         * @brief Which interfaces to broadcast packets if we dont have an entry. If empty, packet will be broadcasted to all interfaces
         * 
         */
        std::vector<uint8_t> _broadcastList;

        RnpNetworkManagerConfig _config;
        SaveConfigImpl _saveConfigImpl;


        RoutingTable routingtable;
        RoutingTable _basetable; //copy of the intial routing table read in from config
       


        const bool _loggingEnabled;
        LogCb_t _logcb; // callback to logging function
        void log(const std::string& msg);




};
