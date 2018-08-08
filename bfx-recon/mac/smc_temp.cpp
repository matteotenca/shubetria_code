//
//  smc_temp.cpp
//  Shubetria
//
//  Created by Matteo Tenca on 08/08/18.
//

#include <stdio.h>
#include "smc_temp.h"

int SMCreadCpuTemp(char *newkey) {

    int temp = 0;
    if (open_smc() != kIOReturnSuccess) {
        return -1;
    }

    temp = get_tmp(newkey, FAHRENHEIT);
//    printf("CPU 0 Diode: %0.1f°C\n", get_tmp(newkey, CELSIUS));
    //printf("Fan 0: %d RPM\n", get_fan_rpm(0));

    close_smc();
    
    return temp;
}
