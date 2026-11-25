#pragma once
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

bool datacenter_load_csv(char* filename);

void datacenter_set_filter(int disable_endpint);

void datacenter_print();

#ifdef __cplusplus
}
#endif