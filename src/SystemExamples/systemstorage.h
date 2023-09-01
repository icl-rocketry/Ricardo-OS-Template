#pragma once

#include <libriccore/riccoresystem.h>

#include <memory>

#include "Config/systemflags_config.h"
#include "Config/commands_config.h"
#include "Config/pinmap_config.h"

#include "Commands/commands.h"

#include "Storage/sdfat_store.h"
#include "Storage/sdfat_file.h"

#include <SPI.h>
class SystemStorage : public RicCoreSystem<System,SYSTEM_FLAG,Commands::ID>
{
    public:

        SystemStorage();
        
        void systemSetup();

        void systemUpdate();

    private:

        SPIClass vspi;

        SdFat_Store filestore;

        std::unique_ptr<WrappedFile> file_ptr;

        uint32_t prevtime;

        std::vector<uint8_t> data;

        

        


};