//
//  main.m
//  smcutil
//
//  Created by kozlek on 31.03.13.
//  Copyright (c) 2013 kozlek. All rights reserved.
//

/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#import <Foundation/Foundation.h>
#import <stdio.h>

#import "SmcHelper.h"

#define NSStr(x) [NSString stringWithCString:(x) encoding:NSASCIIStringEncoding]

#define OPTION_NONE     0
#define OPTION_LIST     1
#define OPTION_READ     2
#define OPTION_WRITE    3
#define OPTION_HELP     4

void usage(const char* prog)
{
    printf("smcutil v%s\n", VERSION);
    printf("Usage:\n");
    printf("%s [options]\n", prog);
    printf("    -l         : list of all keys\n");
    printf("    -r <key>   : show key value\n");
    printf("    -h         : help\n");
    printf("\n");
}

UInt32 SMCReadIndexCount(io_connect_t connection)
{
    return [[SmcHelper readNumericKey:@"#KEY" connection:connection] intValue];
}

bool printKeyValue(SMCVal_t val)
{
    if (val.dataSize > 0)
    {
        if (!strncasecmp(val.dataType, "ch8*", 4)) {
            for (UInt32 i = 0; i < val.dataSize; i++)
                printf("%c", (char)val.bytes[i]);
        }
        else if (!strncasecmp(val.dataType, "flag", 4)) {
            printf(val.bytes[0] ? "TRUE" : "FALSE");
        }
        else  if ([SmcHelper isValidIntegerSmcType:NSStr(val.dataType)]) {
            printf("%d", [SmcHelper decodeNumericValueFromBuffer:val.bytes length:val.dataSize type:val.dataType].unsignedIntValue);
        }
        else if ([SmcHelper isValidFloatingSmcType:NSStr(val.dataType)]) {
            printf("%.2f", [SmcHelper decodeNumericValueFromBuffer:val.bytes length:val.dataSize type:val.dataType].floatValue);
        }
        else return false;
        
        return true;
    }
    else
    {
        printf("no data");
    }
    
    return false;
}

void printValueBytes(SMCVal_t val)
{
    printf("(bytes");
    if (val.dataSize > 0)
    {
        for (UInt32 i = 0; i < val.dataSize; i++)
            printf(" %02x", (unsigned char) val.bytes[i]);
    }
    else {
        printf(" -");
    }
    printf(")");
}

int newmain(char newkey[])
{
    int ret;
    ret = 0;
    @autoreleasepool {
        
        char key[5];
        SMCVal_t val;
        
        
        io_connect_t connection;
        
        if (kIOReturnSuccess == SMCOpen("AppleSMC", &connection)) {
            
                    snprintf(key, 5, "%s", newkey);
                    if (kIOReturnSuccess == SMCReadKey(connection, key, &val)) {
                        // printKeyValue(val);
                        float temp = [SmcHelper decodeNumericValueFromBuffer:val.bytes length:val.dataSize type:val.dataType].floatValue;
                        printf("%.2f\n", temp);
                        ret = (int) temp;
                    }
            
        
            
            SMCClose(connection);
        }
        else {
            printf("failed to connect to SMC!\n");
        }
    
    }
    return ret;
}

