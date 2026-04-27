#define _CRT_SECURE_NO_WARNINGS
#include "port.h"
#include "display.h"
#include "station.h"
#include "load.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



// This function adds a new station to the bst based on its ID
Station* addNewStationToBST(Station* node, Station* newStation) {
    if (node == NULL) {
        return newStation;
    }
    if (newStation->id < node->id) {
        node->left = addNewStationToBST(node->left, newStation);
    } else if (newStation->id > node->id) {
        node->right = addNewStationToBST(node->right, newStation);
    }
   
    return node;
}
// This function loads stations from a file and adds them to the bst
void loadStations(Station **station_root) {
    FILE* file = fopen("data/Stations.txt", "r");
    if (!file) {
        printf("Error: Could not open data/Stations.txt\n");
         displayError(2);
        exit(1);
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file); // Skip header

    while (fgets(buffer, sizeof(buffer), file)) {
        Station* newStation = (Station*)malloc(sizeof(Station));
        if (!newStation) {
            printf("Memory allocation failed for Station\n");
            exit(1);
        }

        char name[100];
        sscanf(buffer, "%d,%99[^,],%d,%f,%f",
               &newStation->id, name, &newStation->numOfPorts,
               &newStation->coordX, &newStation->coordY);

        newStation->name = strdup(name); 
        if (!newStation->name) {
             printf("Memory allocation failed for station name\n");
             exit(1);
        }
        
        // Initialize other fields
        newStation->queue.front = NULL;
        newStation->queue.rear = NULL;
        newStation->left = NULL;
        newStation->right = NULL;
        newStation->port_list = NULL;

        // Add the new station to the bst
        *station_root = addNewStationToBST(*station_root, newStation);
    }
    fclose(file);
    printf("Stations loaded successfully.\n");
}


// This function converts a string to a PortType enum
t_PortType stringToPortType(const char* str) {
    if (str == NULL) return SLOW;
    if (strcmp(str, "FAST") == 0) return FAST;
    if (strcmp(str, "MID") == 0) return MID;
    return SLOW;
}



   //This function loads ports from a file and adds them to the port list of the matching station
void loadPorts(Station *station_root) {
    FILE* file = fopen("data/Ports.txt", "r");
    if (!file) {
        printf("Error: Could not open data/Ports.txt\n");
         displayError(1);
        exit(1);
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file); 

    while (fgets(buffer, sizeof(buffer), file)) {
        Port* newPort = (Port*)malloc(sizeof(Port));
        if (!newPort) {
            printf("Memory allocation failed for Port\n");
            displayError(3);
            exit(1);
        }

        char typeStr[20];
        int items_read = sscanf(buffer, "%d,%d,%19[^,],%d,%d,%d,%d,%d,%d,%d",
                                &newPort->stationID, &newPort->portNumber, typeStr,
                                &newPort->status, &newPort->date.year, &newPort->date.month,
                                &newPort->date.day, &newPort->date.hour, &newPort->date.min,
                                &newPort->carLicense);

        if (items_read >= 9) { 
            if (items_read < 10) {
                newPort->carLicense = 0; 
            }
            newPort->portType = stringToPortType(typeStr);
            newPort->p2car = NULL;
            newPort->next = NULL;
            Station *station = searchStation(station_root, newPort->stationID);
            addToPortListEnd(&station->port_list, newPort);
        } else {
            printf("Warning: Skipped malformed line in Ports.txt: %s", buffer);
            free(newPort);
        }
    }
    fclose(file);
    printf("Ports loaded successfully.\n");
}


// This function adds a new car to the BST
tCar* addNewCarToBST(tCar* node, Car* newCar) {
    if (newCar == NULL) {
        printf("Error: Attempted to insert a NULL car.\n");
        return node;
    }

    if (node == NULL) {
        tCar* newNode = (tCar*)malloc(sizeof(tCar));
        if (!newNode) {
            printf("Memory allocation failed for tCar node.\n");
            free(newCar); // Prevent memory leak of the car struct
            displayError(4);
            exit(1);
        }
        newNode->car = newCar;
        newNode->left_car = NULL;
        newNode->right_car = NULL;
        return newNode;
    }

    if (node->car == NULL) { 
        printf("Error: Tree node contains a NULL car. Overwriting.\n");
        node->car = newCar;
        return node;
    }

    if (newCar->license < node->car->license) {
        node->left_car = addNewCarToBST(node->left_car, newCar);
    } else if (newCar->license > node->car->license) {
        node->right_car = addNewCarToBST(node->right_car, newCar);
    } else {
        printf("Warning: Duplicate car license %d found. Skipping insertion.\n", newCar->license);
        free(newCar); // Free the duplicate car to prevent memory leak
    }
    return node;
}

// This function loads cars from a file and adds them to the BST
void loadCars(tCar **car_root) {
    FILE* file = fopen("data/Cars.txt", "r");
    if (!file) {
        perror("Error opening Cars.txt");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        Car* newCar = (Car*)malloc(sizeof(Car));
        if (!newCar) {
            perror("Failed to allocate memory for Car");
            fclose(file);
            return;
        }

        char* token;
        token = strtok(line, ",");
        newCar->license = token ? atoi(token) : 0;
        token = strtok(NULL, ",");
        newCar->port_type = token ? stringToPortType(token) : SLOW;
        token = strtok(NULL, ",");
        newCar->total_payed = token ? atof(token) : 0.0f;
        token = strtok(NULL, ",");
        newCar->station_id = token ? atoi(token) : 0;
        token = strtok(NULL, ",");
        newCar->port_Num = token ? atoi(token) : 0;
        token = strtok(NULL, ",\n\r");
        newCar->inqueue = token ? atoi(token) : 0;
        newCar->port = NULL;

        // Add the newly created car to the BST
        *car_root = addNewCarToBST(*car_root, newCar);
    }
    fclose(file);
    printf("Successfully loaded data from cars\n");
}
     //This function searches for a car in the BST by its license number
Car* searchCar(tCar* car_root, int licenseNb) {
    tCar* current = car_root;
    while (current != NULL) {
        if (current->car == NULL) { 
            current = current->left_car; // Move arbitrarily to avoid infinite loop
            continue;
        }
        if (licenseNb == current->car->license) {
            return current->car;
        }
        if (licenseNb < current->car->license) {
            current = current->left_car;
        } else {
            current = current->right_car;
        }
    }
    return NULL; // Car not found
}


    //this function loads the line of cars from a file and adds them to the queue of the matching station
void loadLineOfCars(tCar* car_root, Station* station_root) {
    FILE* file = fopen("data/LineOfCars.txt", "r");
    if (!file) {
    displayError(1);
        return;
    }

    char buffer[100];
    fgets(buffer, sizeof(buffer), file); 

    int license, stationId;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (sscanf(buffer, "%d,%d", &license, &stationId) == 2) {
            Car* car = searchCar(car_root, license);
            Station* station = searchStation(station_root, stationId);
            if (car && station) {
                car->inqueue = 1;
                car->station_id = stationId;
                addCarToQueue(&(station->queue), car);
            }
        }
    }
    fclose(file);
}


// This function links cars to their respective ports based on the station and port number
void linkCarsToPorts(tCar* node, Station* station_root) {
    
    if (node == NULL) {
        return;
    }

    
    linkCarsToPorts(node->left_car, station_root);

    // Process the current node
    if (node->car != NULL) {
        
        if (node->car->inqueue == 0 && node->car->station_id != 0) {
            
    
            Station* station = searchStation(station_root, node->car->station_id);

            
            if (station != NULL) {
                // Search for the specific port within the station port list
                Port* current_port = station->port_list;
                while (current_port != NULL) {
                    if (current_port->portNumber == node->car->port_Num) {
                        break; // Found the matching port
                    }
                    current_port = current_port->next;
                }

                
                if (current_port != NULL) {
                    current_port->p2car = node->car;
                    node->car->port = current_port;
                }
            }
        }
    }
}


// This function links the data structures cars to ports and loads waiting cars from the queue file
void linkDataStructures(tCar* car_root, Station* station_root) {
    printf("Linking data structures...\n");

        printf("Linking cars to ports...\n");

    linkCarsToPorts(car_root, station_root);

    // Load and link waiting cars from the queue file
        printf("Linking car queues...\n");
    loadLineOfCars(car_root, station_root);

    printf("Data structures linked successfully.\n");
}



// This function loads all files and links the data structures
void loadAllFiles(Station** station_head, tCar** car_head) {
    // Ensure roots are NULL before loading
    *station_head = NULL;
    *car_head = NULL;

    loadStations(station_head);
    loadPorts(*station_head);
    loadCars(car_head); // Now populates the BST directly
    
    printAllCarsInBST(*car_head);

    
    linkDataStructures(*car_head, *station_head);
    
    printf("All files loaded and data linked.\n");
}


  //this function writes station data to a file in an in-order traversal
void writeStationData(FILE* file, Station* node) {
    if (node == NULL) {
        return;
    }
    writeStationData(file, node->left);
    fprintf(file, "%d,%s,%d,%.2f,%.2f\n",
            node->id, node->name, node->numOfPorts, node->coordX, node->coordY);
    writeStationData(file, node->right);
}

// This function writes car data to Cars.txt and LineOfCars.txt in an in-order traversal
void writeCarData(FILE* carsFile, FILE* lineOfCarsFile, tCar* node) {
    if (node == NULL || node->car == NULL) {
        return;
    }
    Car* car = node->car;
    writeCarData(carsFile, lineOfCarsFile, node->left_car);
    
    // Write main car data to Cars.txt
    fprintf(carsFile, "%d,%s,%.2f,%d,%d,%d\n",
            car->license, portTypeToString(car->port_type), car->total_payed,
            car->station_id, car->port_Num, car->inqueue);
            
    // If car is in a queue, write it to LineOfCars.txt
    if (car->inqueue == 1) {
        fprintf(lineOfCarsFile, "%d,%d\n", car->license, car->station_id);
    }
    
    writeCarData(carsFile, lineOfCarsFile, node->right_car);
}



// This function updates the Stations.txt file with the current station data
void updateStationsFile(const char* filename, Station* station_root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening stations file for writing");
        return;
    }
    fprintf(file, "ID,Name,NumOfPorts,CoordX,CoordY\n");
    writeStationData(file, station_root);
    fclose(file);
}
//this function updates the Ports.txt file with the current port data
void updatePortsFile(const char* filename, Port* port_root) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening ports file for writing");
        return;
    }
    fprintf(file, "StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense\n");
    
    Port* current = port_root;
    while (current != NULL) {
        // Use the linked car license if it exists otherwise use the stored one (or 0)
        int licenseToSave = (current->status == 1 && current->p2car != NULL) 
                            ? current->p2car->license 
                            : current->carLicense;

        fprintf(file, "%d,%d,%s,%d,%d,%d,%d,%d,%d,%d\n",
                current->stationID, current->portNumber, portTypeToString(current->portType),
                current->status, current->date.year, current->date.month, current->date.day,
                current->date.hour, current->date.min, licenseToSave);
        current = current->next;
    }
    fclose(file);
}
  //this function updates the Cars.txt and LineOfCars.txt files with the current car data
void updateCarsFiles(const char* cars_filename, const char* line_of_cars_filename, tCar* car_root) {
    FILE* carsFile = fopen(cars_filename, "w");
    FILE* lineOfCarsFile = fopen(line_of_cars_filename, "w");

    if (!carsFile || !lineOfCarsFile) {
        if (carsFile) fclose(carsFile);
        if (lineOfCarsFile) fclose(lineOfCarsFile);
        perror("Error opening car files for writing");
        return;
    }

    // Write headers
    fprintf(carsFile, "License,PortType,TotalPayed,StationID,PortNumber,InQueue\n");
    fprintf(lineOfCarsFile, "License,StationID\n");

    // Write data using the recursive helper
    writeCarData(carsFile, lineOfCarsFile, car_root);

    fclose(carsFile);
    fclose(lineOfCarsFile);
}


// This function updates all data files Stations.txt, Ports.txt, Cars.txt, and LineOfCars.txt
void updateAllFiles(Station* station_root, Port* port_root, tCar* car_root) {
    printf("\nUpdating all data files...\n");

    updateStationsFile("data/Stations.txt", station_root);
    printf("Stations.txt has been updated.\n");

    updatePortsFile("data/Ports.txt", port_root);
    printf("Ports.txt has been updated.\n");

    updateCarsFiles("data/Cars.txt", "data/LineOfCars.txt", car_root);
    printf("Cars.txt and LineOfCars.txt have been updated.\n");

    printf("All files saved successfully.\n");
}