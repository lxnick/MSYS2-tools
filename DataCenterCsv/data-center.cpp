#include "data-center.h"
#include "data-center.hpp"

#include <stdio.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>

#define MAX_OUTPUT_LINE 1000
#define MAX_ENDPOINT_COUNT  8

class DataCenter data_center;
static int output_line = 0;

static const std::unordered_set<std::string> chirp_string_table = 
 {
    "<Reset> / <Chirp K> / <Tiny K>",
    "<Reset> / <Chirp J> / <Tiny J>",
    "<Chirp K>",
    "<Chirp J>", 
   "<Chirp J>", 
};

static std::unordered_set<std::string> disable_endpint_table ; 


bool datacenter_load_csv(char* filename)
{
    printf("%s\n", filename);

    return data_center.LoadCsv(filename);
}

void datacenter_set_filter(int disable_endpint)
{
    data_center.SetFilter(disable_endpint);
}

void datacenter_print()
{
    printf("%s\n", __FUNCTION__);

}

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

static std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::string token;
    std::istringstream ss(line);

    while (std::getline(ss, token, ',')) {
        result.push_back(token);
    }
    return result;
}

static std::vector<std::string> find_csv_header(std::ifstream& in) {
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        if (line[0] == '#') {
            line.erase(0, 1);
            line = trim(line);
            if (line.rfind("Level,Sp,Index", 0) == 0) {
                return split_csv_line(line);
            }
        }
    }
    throw std::runtime_error("CSV header not found");
}

bool is_sof_record(std::string string)
{
    int sof = 0 ;

    sscanf(string.c_str(), "[%d SOF]", &sof);

    if (sof > 0)
        return true;
    else
        return false;
}

bool is_chirp_record(std::string string)
{
    if (chirp_string_table.find(string) != chirp_string_table.end()) 
        return  true;

    int j_k_pairs = 0 ;        
    sscanf(string.c_str(), "[%d Chirp K-J pairs]", &j_k_pairs);
    if (j_k_pairs > 0)
        return true;

    return false;
}

bool is_disable_endpoint(std::string string)
{
    if (disable_endpint_table.find(string) != disable_endpint_table.end()) 
        return  true;
    return  false;
}

class DataEntry ParseLine(std::vector<std::string> fields)
{
    class DataEntry entry;

    entry.level = fields[0];
    entry.speed = fields[1];
    entry.index = fields[2];
    entry.time0 = fields[3];
    entry.duration = fields[4];
    entry.length = fields[5];
    entry.error = fields[6];

    entry.device = fields[7];
    entry.endpoint = fields[8];
    entry.record = fields[9];
    entry.data = fields[10];
    entry.summary = fields[11];
    entry.ASCII = fields[11];

    return  entry;

}

DataCenter::DataCenter()
{
}

DataCenter::~DataCenter()
{
}

bool DataCenter::LoadCsv(char* filename)
{
    std::ifstream in(filename);
    if (!in) {
        fprintf(stderr, "ERROR: Load CSV file failed : '%s'\n", filename);
        return 1;
    }

    // 1. 找 header
    auto header = find_csv_header(in);

    // header name -> index
    std::unordered_map<std::string, size_t> col;
    for (size_t i = 0; i < header.size(); ++i) {
        col[trim(header[i])] = i;
    }

    // Should Check header here ?
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        auto fields = split_csv_line(line);

        class DataEntry entry = ParseLine(fields);

        if ( CheckFilter(entry))
        {
                 printf("[Time %s] Index =%s, Level=%s,EP = %s, Record=%s\n", 
                    entry.time0.c_str(),
                    entry.index.c_str(), 
                    entry.level.c_str(), 
                    entry.endpoint.c_str(),
                    entry.record.c_str()
                    );

                if (! entry.data.empty() )
                    printf("%s\n", entry.data.c_str() );

                output_line ++;        

            if ( output_line >= MAX_OUTPUT_LINE)
            {
                printf(">>>>>>>>>>>>>>>>Maximum line %d\n",   output_line);              
//                break;
		return true; 						
            }                
        }
    }    

    printf(">>>>>>>>>>>>>>>>Load Complete %d\n",   output_line);              
    return true;    
}

bool DataCenter::CheckFilter(class DataEntry entry)
{
    // Remove none information record
    if (is_sof_record(entry.record))
        return false;
    if (is_chirp_record( trim(entry.record) ))
        return false;

    if  (is_disable_endpoint(entry.endpoint))
        return false;   

    if ( 1) // Info
    {
        if ( entry.level == "0")
            return true;
        else
            return false;    

    }


    return true;
}

void DataCenter::SetFilter(int disable)
{
    printf("%s %d\n", __FUNCTION__, disable)  ;
    char endpoint_name[6];

    for ( int i=0; i < MAX_ENDPOINT_COUNT; i ++)
    {

        if ( disable & (1 << i))
        {
            snprintf(endpoint_name,sizeof(endpoint_name), "%02d", i);

            printf("name %s\n", endpoint_name)  ;
            disable_endpint_table.insert(endpoint_name);
        }
    }

    for (auto it = disable_endpint_table.begin(); it != disable_endpint_table.end(); ++it) 
    {
        printf("endpoint %s\n", (*it).c_str() );
    }       

}

void DataCenter::Print()
{

}
