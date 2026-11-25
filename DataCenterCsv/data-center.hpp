#pragma once

#include <string>

class DataEntry
{
public:
    std::string level;
    std::string speed;	// 
    std::string index;
    std::string time0;
    std::string duration;
    std::string length;
    std::string error;
    std::string device;
    std::string endpoint;
    std::string record;
    std::string data;
    std::string summary;
    std::string ASCII;
};

class DataCenter
{
public:
    DataCenter();
    ~DataCenter();
    bool LoadCsv(char* filename);
    
    bool CheckFilter(class DataEntry entry);
    void SetFilter(int disable);  
    void Print();  

    int disable_endpoint;    

};

