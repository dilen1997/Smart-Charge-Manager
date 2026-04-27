#ifndef CARS_H 
#define CARS_H

#include"types.h"

typedef struct Car
{
    int license ;
    t_PortType port_type;
    float  total_payed ;
    int station_id;
    int port_Num;
    Port *port;
    int inqueue;

}Car;

typedef struct CarInLine
{
   int license;
   int  stationID;
} CarInLine;

typedef struct tCar{
    Car* car;
    tCar* right_car;
    tCar* left_car ; 
}tCar;

typedef struct Car_Node{
    Car * car;
    struct Car_Node* next_car; 
}Car_Node;


typedef struct Q_Car
{
    Car_Node *front;
    Car_Node *rear ; 
}Q_Car;

tCar* addNewCarToBST(tCar* node, Car* newCar);

void chargeCar(tCar** car_root_ptr, Station* station_root);

Car* dequeueCar(Q_Car* queue);

Car* searchCar(tCar* car_root, int licenseNb);

int countOccupiedPorts(Station* station);

int countCarsInQueue(Q_Car* queue);

#endif