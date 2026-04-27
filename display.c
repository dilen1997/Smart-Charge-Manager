#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stddef.h>
#include "station.h"
#include"port.h"
#include"cars.h"
#include <ctype.h>
#include <string.h>
#include <time.h>
#include"display.h"

time_t DateToTimeT(Date d);
const char* portTypeToString(t_PortType type);
int countOccupiedPorts(Station* station);
int countCarsInQueue(Q_Car* queue);
void dispStationPorts(Port* head); 
void dispAllSt_recursive(Station* node);


//Function to display an error message based on the error code
void displayError(int code) {
    switch (code) {
        case 1: printf("Error: File not found.\n"); break; 
        case 2: printf("Error: Station not found.\n"); break;
        case 3: printf("Eror: Port not found \n");break;
        case 4: printf("Eror Car not found \n");break;
        default: printf("Error: An unknown error occurred.\n"); break;   
    }
}



//Converts a port type enum value to its matching string name
const char* portTypeToString(t_PortType type) {
    switch (type) {
        case 0: return "SLOW";
        case 1: return "MID";
        case 2: return "FAST";
        default: return "UNKNOWN";
    }
}


//Recursively prints the cars in the binary search tree in-order by license number
void printCarBST_Recursive(tCar* node) {
    
    if (node == NULL) {
        return;
    }

   
    printCarBST_Recursive(node->left_car);

   
    if (node->car != NULL) {
        printf("  License: %-10d | Port Type: %-5s | Total Paid: %7.2f\n",
               node->car->license,
               portTypeToString(node->car->port_type),
               node->car->total_payed);
    }

  
    printCarBST_Recursive(node->right_car);
}

//Prints all cars in the BST in sorted order by license number
void printAllCarsInBST(tCar* root) {
    if (root == NULL) {
        printf("The car tree is empty.\n");
        return;
    }
    
    printf("--- Car List (Sorted by License) ---\n");
    printCarBST_Recursive(root);
    printf("------------------------------------\n");
}

//Recursively scans all stations and displays cars currently charging at busy ports
void displayChargingCars_recursive(Station* node, int* found_any) {
    
    if (node == NULL) {
        return;
    }

   
    displayChargingCars_recursive(node->left, found_any);

    
    Port* current_port = node->port_list;
    int is_first_for_station = 1; 

    while (current_port != NULL) {
       
        if (current_port->status == 1 && current_port->p2car != NULL) {
            
            // If this is the first charging car we find at this station print the station name
            if (is_first_for_station) {
                printf("\nStation: %s (ID: %d)\n", node->name, node->id);
                is_first_for_station = 0;
            }

            // Print details of the charging car
            printf("  -> Port #%d: Car with license %d is charging.\n", 
                   current_port->portNumber, 
                   current_port->carLicense);
            
            *found_any = 1; 
        }
        current_port = current_port->next;
    }

    
    displayChargingCars_recursive(node->right, found_any);
}


//Displays all stations by calling a recursive function to print their details in order
void dispAllSt(Station* station_root) {
    printf("\n\n--- Displaying All Stations ---\n");
    printf("======================================================================================\n");

    if (station_root == NULL) {
        printf("The station database is empty.\n");
    } else {
        dispAllSt_recursive(station_root);
    }
    
    printf("======================================================================================\n");
}

//Recursively displays each station details including occupied ports and queue size  in order traversal
void dispAllSt_recursive(Station* node) {
    if (node == NULL) {
        return;
    }
    
    dispAllSt_recursive(node->left);

   
    
   
    int occupied_ports = countOccupiedPorts(node);
    int queued_cars = countCarsInQueue(&(node->queue)); 
    printf("\n>> Station ID: %-5d | Name: %-20s | Occupied/Total Ports: %d/%-5d | Cars in Queue: %d\n",
           node->id,
           node->name,
           occupied_ports,
           node->numOfPorts,
           queued_cars);

    
    dispStationPorts(node->port_list);

   
    dispAllSt_recursive(node->right);
}

//Displays a table of all ports in a station showing their number type, status and linked car if occupied
void dispStationPorts(Port* head) {
    printf("   -------------------------------------------------------------------\n");
    printf("   %-12s | %-12s | %-10s | %-12s | %-12s\n", 
           "Port Number", "Station ID", "Type", "Status", "Car License");
    printf("   -------------------------------------------------------------------\n");

    if (head == NULL) {
        printf("   No ports are configured for this station.\n");
        return;
    }

    Port* current = head;
    while (current != NULL) {
        const char* typeStr = portTypeToString(current->portType);
        const char* statusStr = (current->status == 0) ? "Available" : "Occupied";

        // Print the main part of the line
        printf("   %-12d | %-12d | %-10s | %-12s | ",
               current->portNumber,
               current->stationID,
               typeStr,
               statusStr);

     
        if (current->status == 1 && current->carLicense > 0) {
            printf("%-12d\n", current->carLicense);
        } else {
            printf("N/A\n");
        }

        current = current->next;
    }
    printf("   -------------------------------------------------------------------\n");
}



char my_tolower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}


//allows searching for a station by ID or name then displays cars currently charging and waiting in that station queue
void dispCarsAtSt(Station* station_root) {
    char choice[10];
    char search_name[100];
    int stationId;
    Station* station = NULL;

    printf("\nSearch for a station by ID or Name? (enter 'id' or 'name'): ");
    if (scanf("%9s", choice) != 1) {
        fprintf(stderr, "Error reading choice.\n");
        return;
    }


    for (int i = 0; choice[i]; i++) {
        choice[i] = my_tolower(choice[i]);
    }

    if (strcmp(choice, "id") == 0) {
        printf("Please enter the Station ID: ");
        if (scanf("%d", &stationId) != 1) {
            fprintf(stderr, "Invalid input for Station ID.\n");
            while (getchar() != '\n');
            return;
        }
        station = searchStation(station_root, stationId);
    } else if (strcmp(choice, "name") == 0) {
        printf("Please enter the Station Name: ");
        while (getchar() != '\n');
        fgets(search_name, sizeof(search_name), stdin);
        search_name[strcspn(search_name, "\n")] = 0;
        station = searchStationByName(station_root, search_name);
    } else {
        fprintf(stderr, "Invalid choice. Please enter 'id' or 'name'.\n");
        return;
    }

    if (station == NULL) {
        fprintf(stderr, "Error: Station was not found.\n");
        return;
    }

    printf("\n--- Cars at Station: %s (ID: %d) ---\n", station->name, station->id);


    printf("\n[ Currently Charging ]\n");
    int charging_found = 0;
    Port* current_port = station->port_list;
    while (current_port != NULL) {
        if (current_port->status == 1 && current_port->p2car != NULL) {
            time_t startTime = DateToTimeT(current_port->date);
            time_t currentTime = time(NULL);
            double minutes_charging = difftime(currentTime, startTime) / 60.0;

            printf("  - License: %-10d | Port Type: %-5s | Charging at port: %d\n",
                   current_port->carLicense,
                   portTypeToString(current_port->portType),
                   current_port->portNumber);
            charging_found = 1;
        }
        current_port = current_port->next;
    }

    if (!charging_found) {
        printf("  No cars are currently charging at this station.\n");
    }

    printf("\n[ Waiting in Queue ]\n");
    int queue_found = 0;
    Car_Node* current_node = station->queue.front;
    while (current_node != NULL) {
        if (current_node->car != NULL) {
            printf("  - License: %-10d | Waiting for: %s port\n",
                   current_node->car->license,
                   portTypeToString(current_node->car->port_type));
            queue_found = 1;
        }
        current_node = current_node->next_car;
    }

    if (!queue_found) {
        printf("  The waiting queue is empty at this station.\n");
    }
    printf("-------------------------------------------\n");
}