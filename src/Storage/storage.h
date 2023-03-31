#pragma once
#include <unordered_map>

class freertos_mutex;


class Base_Storage{

void take(task_handle_t task);
void give(task_handle_t task);
void mount();


private:    
    freertos_mutex storage_mutex;



};


class StorageController{



std::unordered_map<uint8_t,Storage *> storage_list;


};



class Relog{
    Relog(std::string filename,size_t file_size_hint,size_t buffersize)

    /**
     * @brief Writes data to internal buffer and flushes when buffer size is met
     * 
     * @param data 
     */
    void write(std::vector<uint8_t> data); 

    /** 
     * @brief force flush 
     * 
     */
    void flush();


    std::vector<Storage&> storage_of_storages;

}