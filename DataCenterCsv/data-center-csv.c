#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif

#include "data-center.h"


#define PROJECT_NAME "data-center-csv"
#define PROJECT_VER "0.0.1"

#define MAX_LINE    10000

bool               verbose = false;

static void print_usage(char *m_name) {
    fprintf(stderr,
            "Usage: \n"
            "  %s <cmd> [options]\n"
            "where <cmd> is one of:\n"
            "  --info -i        Select print information\n"
            "  --version -v     Print version information \n"
            "  --file -f        Select input file \n"     
            "  --disable -d     Disable EndPoint \n" 
            "\n"
            "",
            m_name);
}

// Display program version
static void display_version(char *m_name) {
    fprintf(stderr, "%s " PROJECT_VER "\n", m_name);
}

void cleanup(void) {

}

void error(void) {
    exit(1);
}

void clear_buffer(unsigned char *data, size_t length) {
    for (int i = 0; i < length; i++)
        data[i] = 0;
}

void print_buffer(unsigned char *data, size_t length) {
    printf("Sending Report...\n");
    for (int i = 0; i < length; i++)
        printf("%02x", data[i]);
    printf("\n");
}

bool read_response_16(unsigned char *data, uint32_t offset, uint16_t expected_result, uint16_t *resp) {
    uint16_t r = *resp;

    memcpy(&r, data + offset, sizeof(uint16_t));

    *resp = r;
    return r == expected_result;
}

bool read_response_32(unsigned char *data, uint32_t offset, uint32_t expected_result, uint32_t *resp) {
    uint32_t r = *resp;

    memcpy(&r, data + offset, sizeof(uint32_t));

    *resp = r;
    return r == expected_result;
}

void write_buffer_32(unsigned char *data, uint32_t cmd) {
    memcpy(data, &cmd, 4);
}

void write_buffer_16(unsigned char *data, uint16_t cmd) {
    memcpy(data, &cmd, 2);
}

uint16_t checksum16(const unsigned char *data, size_t size) {
    uint16_t sum = 0;
    size_t   i;

    for (i = 0; i + 1 < size; i += 2) {
        uint16_t value = data[i] | (data[i + 1] << 8);
        sum += value;
    }

    if (i < size) {
        sum += data[i];
    }

    return sum;
}

void print_data(const unsigned char *data, int length) {
    for (int i = 0; i < length; i++) {
        if (i % 16 == 0) {
            if (i > 0) {
                printf("\n");
            }
            printf("%04x: ", i); // Print address offset
        }
        printf("%02x ", data[i]);
    }
    printf("\n");
}

char *get_full_path(const char *file_name) {
    char *full_path = NULL;

#ifdef _WIN32
    char buffer[MAX_PATH];
    if (GetFullPathName(file_name, MAX_PATH, buffer, NULL) != 0) {
        full_path = strdup(buffer);
    } else {
        fprintf(stderr, "ERROR: Could not resolve full path for file: '%s'\n", file_name);
    }
#else
    char buffer[PATH_MAX];
    if (realpath(file_name, buffer) != NULL) {
        full_path = strdup(buffer);
    } else {
        fprintf(stderr, "ERROR: Could not resolve full path for file: '%s'\n", file_name);
    }
#endif

    return full_path;
}




int main(int argc, char *argv[]) {
    int         opt, opt_index;

    char* filename = NULL;
    bool select_info = false;
    int disable_endpoint = 0;
    int endpoint = 0;
    char *endptr = NULL;

    // clang-format off
    struct option longoptions[] = {{"help", no_argument, 0, 'h'},
                                 {"version", no_argument, 0, 'v'},
                                 {"info", no_argument, NULL, 'i'},
                                 {"file", required_argument, NULL, 'f'},    
                                 {"disable", required_argument, NULL, 'd'},                                                        
                                 {NULL, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "hvif:d:", longoptions, &opt_index)) != -1) {
        switch (opt) {
            case 'h': // help
                print_usage(PROJECT_NAME);
                break;
            case 'v': // version
                display_version(PROJECT_NAME);
                break;
            case 'i': // info
                select_info = true;
                break;   
            case 'f': // file
                filename = get_full_path(optarg);
                break;      
            case 'd': // disable
                endpoint = strtol(optarg, &endptr, 0);
                if (errno == ERANGE || *endptr != '\0') {
                    fprintf(stderr, "ERROR: invalid offset value -'%s'.\n", optarg);
                    exit(1);
                }           
                disable_endpoint |= (1 << endpoint);    
                break;      
            default:
               print_usage(PROJECT_NAME);
               break;        
        }

    
    }

         // exit clean after printing
    if (filename == NULL) 
    {
        print_usage(PROJECT_NAME);
        exit(0);   
    }      

    datacenter_set_filter(disable_endpoint);

    if ( ! datacenter_load_csv(filename))
    {
        fprintf(stderr, "ERROR: Load CSV file failed : '%s'\n", filename);
            exit(0);
    }    
  
    datacenter_print( );

    cleanup();
    exit(0);
}
