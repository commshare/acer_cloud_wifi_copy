/*
 *  Copyright 2013 Acer Cloud Technology Inc.
 *  All Rights Reserved.
 *
 *  THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
 *  TRADE SECRETS OF ACER CLOUD TECHNOLOGY INC.
 *  USE, DISCLOSURE OR REPRODUCTION IS PROHIBITED WITHOUT
 *  THE PRIOR EXPRESS WRITTEN PERMISSION OF
 *  ACER CLOUD TECHNOLOGY INC.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <fstream>

#include "log.h"

int main(int argc, char *argv[]) {

    int testInstanceNum = 0;
    std::string line;
    std::ifstream file("port_CloudPC.txt");

    if(argc < 2){
        printf("Usage: %s rootpath\n", argv[0]);
        return -1;
    }

    if(file){
        while(std::getline(file, line)){
            std::size_t pos = line.find("port=");
            if(pos != std::string::npos) {
                pos = line.find("=");
                testInstanceNum = atoi(line.substr(pos+1).c_str());
                printf("port=%d\n", testInstanceNum);
                break;
            }
        }
    }else{
        printf("port_CloudPC.txt not found!\n");
    }

    LOGInit("imltool", argv[1]);
    // Flush out existing logs
    #if (defined LINUX || defined __CLOUDNODE__) && !defined LINUX_EMB
    LOGSetEnableInMemoryLogging(false, testInstanceNum);
    LOG_INFO("rootpath: %s, testInstanceNum: %d", argv[1], testInstanceNum);
    LOGSetEnableInMemoryLogging(false, testInstanceNum);
	#endif
    return 0;
}
