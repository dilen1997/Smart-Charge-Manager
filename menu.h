#ifndef MENU_H
#define MENU_H
void printMenu();

void locNearSt(Station* station_root);

void runMenuSystem(Station* station_root, Port* port_root, tCar* car_root);

void collectCars_recursive(tCar* node, Car** array, int* index);

int compareCarsBySpending(const void* a, const void* b);

void addToPortListEnd(Port **list, Port *port_to_add);

void releasePorts_recursive(Station* node, time_t current_time, int* released_count);

void releasePorts(Station* station_root);

void remOutOrderPort(Station* station_root);

tCar* removeCarFromBST(tCar* root, int licenseNb, int* was_removed);

tCar* findMinValueNode(tCar* node);

void freePortList(Port* head);

void freeCarQueue(Q_Car* queue);

void addCarToQueue(Q_Car* queue, Car* car);

const char* portTypeToString(t_PortType type);

void assignCar2Port(Car* car, Port* port);

t_PortType stringToPortType(const char* str); 

void stopCharge();

void dispAllSt(Station* station_root);

void dispCarsAtSt(Station* station_root);

int countCars_recursive(tCar* node);

Station* removeStationFromBST(Station* root, int stationId, int* was_removed);

void forceReleaseCarsFromStation(Station* station);

int countAvailablePortsByType(Station* station, t_PortType port_type);

void collectCars_recursive(tCar* node, Car** array, int* index);

int getPositionInQueue(Q_Car* queue, int licenseNb);

int countCars_recursive(tCar* node);

void addNewPort(Station* station_root);

#endif