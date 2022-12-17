#ifndef __LOGIC__
#define __LOGIC__

#include <config.h>
#include <motor.h>
#include <wifi_server.h>

#ifdef touch1_debug
    #define situate_ready  "{\"situate\":\"ready\",\"stepname\":\"/casting_publish\"}" 
    #define situate_busy  "{\"situate\":\"busy\",\"stepname\":\"/casting_publish\"}" 
    #define situate_finish  "{\"situate\":\"finish\",\"stepname\":\"/casting_publish\"}"  
    #define clean_busy  "{\"situate\":\"busy\",\"stepname\":\"/clean_casting\"}" 
#endif

#ifdef touch2_debug                        
  #define situate_ready  "{\"situate\":\"ready\",\"stepname\":\"/casting_publish_1\"}" 
  #define situate_busy  "{\"situate\":\"busy\",\"stepname\":\"/casting_publish_1\"}" 
  #define situate_finish  "{\"situate\":\"finish\",\"stepname\":\"/casting_publish_1\"}" 
  #define clean_busy  "{\"situate\":\"busy\",\"stepname\":\"/clean_casting_1\"}" 
#endif

void feeding1();
int tasktomotor();
int whenisfinish();
int ResetWeight(); //投料和复位重量

#endif