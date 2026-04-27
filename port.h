#ifndef PORT_H 
#define PORT_H

#include"types.h"
#include<time.h>

typedef struct Port
{
   int stationID;
   int portNumber;
   t_PortType portType;
   int status;
   Date date;
    int carLicense;
    Car *p2car;
    Port *next ; 
} Port;

void assignCar2Port(Car* car, Port* port);

void releasePorts_recursive(Station* node, time_t current_time, int* released_count);

time_t DateToTimeT(Date d);

void checkQueueAndStartNextCharge(Station* station, Port* available_port);

#endif
