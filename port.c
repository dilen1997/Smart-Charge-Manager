#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "station.h"
#include "port.h"
#include "cars.h"



// void releasePorts_recursive(Station* node, time_t current_time, int* released_count);
// time_t DateToTimeT(Date d);
// void checkQueueAndStartNextCharge(Station* station, Port* available_port);

   /*Releases all ports that have been charging for more than 10 hours.
  This function goes through through all stations and all ports in the system.
  */

void releasePorts(Station* station_root) {
    printf("\n--- Releasing Ports with Over 10 Hours Charging Time ---\n");
    
    if (station_root == NULL) {
        printf("Station data is not available.\n");
        return;
    }

    time_t current_time;
    time(&current_time); 

    int released_count = 0;
   
    releasePorts_recursive(station_root, current_time, &released_count);

    if (released_count == 0) {
        printf("No ports exceeded the 10-hour charging limit.\n");
    } else {
        printf("\nFinished. A total of %d ports were released.\n", released_count);
    }
    printf("--------------------------------------------------------\n");
}




// Recursively traverses the station tree to check and release ports.
void releasePorts_recursive(Station* node, time_t current_time, int* released_count) {
    // Base case for recursion
    if (node == NULL) {
        return;
    }

    // Traverse the entire tree (left, process, right)
    releasePorts_recursive(node->left, current_time, released_count);

    
    Port* current_port = node->port_list;
    const double MAX_CHARGE_SECONDS = 10 * 3600; 

    while (current_port != NULL) {
        if (current_port->status == 1 && current_port->p2car != NULL) {
            // Calculate how long the car has been charging
            time_t start_time = DateToTimeT(current_port->date);
            double seconds_charging = difftime(current_time, start_time);

            if (seconds_charging > MAX_CHARGE_SECONDS) {
                printf("Releasing Port #%d at Station #%d (Car: %d, Charged for %.1f hours)\n",
                       current_port->portNumber, node->id, current_port->p2car->license, seconds_charging / 3600.0);
                
                (*released_count)++;

               
                
                // Disconnect the car from the port
                current_port->p2car->port = NULL; 
                
                // Free the port
                current_port->status = 0; // Set to available
                current_port->carLicense = 0;
                current_port->p2car = NULL;
                
                // Check if a car from the queue can use this newly freed port
                checkQueueAndStartNextCharge(node, current_port);
            }
        }
        current_port = current_port->next;
    }

    releasePorts_recursive(node->right, current_time, released_count);
}

// Converts a Date structure to time_t for easier time calculations.
void remOutOrderPort(Station* station_root) {
    int station_id, port_num;
    printf("\n--- Remove Out-of-Order Port ---\n");
    
    printf("Enter Station ID: ");
    if (scanf("%d", &station_id) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }
    
    printf("Enter Port Number to remove: ");
    if (scanf("%d", &port_num) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    // Find the station first
    Station* station = searchStation(station_root, station_id);
    if (station == NULL) {
        printf("Error: Station with ID %d not found.\n", station_id);
        return;
    }

    //Search for the port within the station's specific port list
    Port* current = station->port_list;
    Port* prev = NULL;

    while (current != NULL && current->portNumber != port_num) {
        prev = current;
        current = current->next;
    }

    // Handle case where the port was not found in the station
    if (current == NULL) {
        printf("Error: Port #%d was not found at Station #%d.\n", port_num, station_id);
        return;
    }

    // Handle case where the port is currently busy
    // Assuming status 1 means occupied.
    if (current->status == 1) {
        printf("Error: Cannot remove port #%d while it is occupied. Please release it first.\n", port_num);
        return;
    }

    // 5. Remove the port from the linked list
    if (prev == NULL) {
        // The port to remove is the first one in the list
        station->port_list = current->next;
    } else {
        // The port is in the middle or at the end of the list
        prev->next = current->next;
    }
    
    // 6. Free the memory of the removed port
    free(current);
    
    // 7. Decrement the station's port counter
    station->numOfPorts--;

    printf("\nSuccess! Port #%d at Station #%d has been removed.\n", port_num, station_id);
    printf("Station %d now has %d ports.\n", station_id, station->numOfPorts);
}



void freePortList(Port* port_head) {
    Port* current = port_head;
    Port* next_node;

    while (current != NULL) {
        next_node = current->next;
        //here as well we dont release current->p2car because it is managed elsewhere
        free(current); // We only free the port structure itself
        current = next_node;
    }
}
 
int countAvailablePortsByType(Station* station, t_PortType port_type) {
    if (station == NULL || station->port_list == NULL) {
        return 0;
    }
    
    int count = 0;
    Port* current_port = station->port_list;
    while (current_port != NULL) {
        if (current_port->status == 2 && current_port->portType == port_type) {
            count++;
        }
        current_port = current_port->next;
    }
    return count;
}


// Adds a Port node to the end of a linked list.
void addToPortListEnd(Port **list, Port *port_to_add) {
    if (*list == NULL) {
        *list = port_to_add;
        return;
    }
    Port *current = *list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = port_to_add;
}

void addNewPort(Station* station_root) {
    int station_id, port_num, type_choice;
    t_PortType port_type;

    printf("\n--- Adding a New Charging Port ---\n");
    
    // 1. Get user input for station ID
    printf("Enter Station ID: ");
    if (scanf("%d", &station_id) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    // 2. Find the station
    Station* station = searchStation(station_root, station_id);
    if (station == NULL) {
        printf("Error: Station with ID %d not found.\n", station_id);
        return;
    }

    // 3. Get user input for port details
    printf("Enter new Port Number: ");
    if (scanf("%d", &port_num) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    // Check if the port number already exists in the station's port list
    Port* temp_port = station->port_list;
    while (temp_port != NULL) {
        if (temp_port->portNumber == port_num) {
            printf("Error: Port number %d already exists in station %d.\n", port_num, station_id);
            return;
        }
        temp_port = temp_port->next;
    }
    
    printf("Select Port Type (0 for SLOW, 1 for MID, 2 for FAST): ");
    if (scanf("%d", &type_choice) != 1 || type_choice < 0 || type_choice > 2) {
        printf("Error: Invalid port type selected.\n");
        while(getchar() != '\n');
        return;
    }
    port_type = (t_PortType)type_choice;

    // 4. Create and initialize the new port
    Port* new_port = (Port*)malloc(sizeof(Port));
    if (new_port == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    
    new_port->stationID = station_id;
    new_port->portNumber = port_num;
    new_port->portType = port_type;
    new_port->status = 0; 
    new_port->carLicense = 0;
    new_port->p2car = NULL;
    new_port->next = NULL;
    // Initialize date to zero
    new_port->date.day = 0; new_port->date.month = 0; new_port->date.year = 0;
    new_port->date.hour = 0; new_port->date.min = 0;

    // 5. Add the new port to the station's specific port list
    addToPortListEnd(&(station->port_list), new_port);

 
    station->numOfPorts++;

    printf("\nSuccess! New port #%d has been added to station #%d.\n", port_num, station_id);
    printf("Station %d now has %d ports.\n", station_id, station->numOfPorts);
}