#ifndef LOAD_H
#define LOAD_H

void loadAllFiles(Station** station_head, tCar** car_head);

void loadLineOfCars(tCar* car_root, Station* station_root);

void updateAllFiles(Station* station_root, Port* port_root, tCar* car_root);

const char* portTypeToString(t_PortType type);

void printMenu();



void addToPortListEnd(Port **list, Port *port_to_add);

Station* searchStation(Station* station_root, int stationId);

tCar* addNewCarToBST(tCar* node, Car* newCar);

Car* searchCar(tCar* car_root, int licenseNb);

void addCarToQueue(Q_Car* queue, Car* car);

#endif