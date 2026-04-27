
#ifndef DISPLAY_H
#define DISPLAY_H

#include "station.h" 

void loadAllFiles(Station** station_head,  tCar** car_head);

void dispAllSt(Station* station_root);

void dispAllSt_recursive(Station* node);

void displayError(int code);

void dispAllPorts(Port* head);

void printCarBST_Recursive(tCar* node);

void printAllCarsInBST(tCar* root);


Station* searchStationByName(Station* station_root, const char* name);



#endif





