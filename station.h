#ifndef HEADER_H 
#define HEADER_H

#include"types.h"
#include "cars.h"

typedef struct Station
{
    int id; 
    char* name;
    int numOfPorts;
    float coordX;
    float coordY; 
    Port  *port_list;
    Q_Car queue;
    Station *right;
    Station *left;
} Station;

void checkQueueAndStartNextCharge(Station* station, Port* available_port);

Station* searchStation(Station* root, int stationId);


#endif
