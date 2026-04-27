#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include"station.h"
#include"port.h"
void freePortList(Port* head);
void freeCarQueue(Q_Car* queue);
        
// This function searches for a station in the binary search tree by its ID
Station* searchStation(Station* station_root, int stationId) {
    Station* current = station_root;
    while(current != NULL) {
        if(stationId == current->id) {
            return current;
        }
        if(stationId < current->id) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return NULL; // Station not found

}

//Searches the station tree by name and returns the matching station node if found
Station* searchStationByName(Station* station_root, const char* name) {
    Station* current = station_root;
    while(current != NULL) {
        int cmp = strcmp(name, current->name);
        if (cmp == 0) { 
            return current;
        }
        if (cmp < 0) { 
            current = current->left;
        } else { 
            current = current->right;
        }
    }
    return NULL; // Station not found
}


// This function assigns a car to a port and updates the ports status
void checkQueueAndStartNextCharge(Station* station, Port* available_port) {
    
    if (station == NULL || available_port == NULL || station->queue.front == NULL) {
        return;
    }

    t_PortType requiredPortType = available_port->portType;
    Car_Node* current_node = station->queue.front;
    Car_Node* prev_node = NULL;
    Car* car_to_charge = NULL;

    // Scan the queue to find the first car waiting for the specific port type
    while (current_node != NULL) {
        if (current_node->car && current_node->car->port_type == requiredPortType) {
            car_to_charge = current_node->car;
            break; 
        }
        prev_node = current_node;
        current_node = current_node->next_car;
    }

    // If no compatible car was found, do nothing.
    if (car_to_charge == NULL) {
        return;
    }

    //  Remove the found car's node from the queue
    if (prev_node == NULL) {
        station->queue.front = current_node->next_car;
    } else {
        prev_node->next_car = current_node->next_car;
    }
    // If the current node is the rear of the queue update the rear pointer
    if (station->queue.rear == current_node) {
        station->queue.rear = prev_node;
    }
    //  Free the memory of the removed node     
    if (station->queue.front == NULL) {
        station->queue.rear = NULL;
    }

    free(current_node);

    // 5. Assign the car to the port using the existing helper function
    printf("Success: Car %d moved from queue to charging port #%d at station #%d.\n",
           car_to_charge->license, available_port->portNumber, station->id);
    assignCar2Port(car_to_charge, available_port);
}



// This function is used when removing a station from the BST to find the next node to replace
Station* findMinStationNode(Station* node) {
    Station* current = node;
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;
}





//Recursively finds and removes a station from the BST.
Station* removeStationFromBST(Station* root, int stationId, int* was_removed) {
    if (root == NULL) return root;

    if (stationId < root->id) {
        root->left = removeStationFromBST(root->left, stationId, was_removed);
    } else if (stationId > root->id) {
        root->right = removeStationFromBST(root->right, stationId, was_removed);
    } else {
     
        *was_removed = 1;

        // Case 1: Node with only one child or no child
        if (root->left == NULL) {
            Station* temp = root->right;
            // Free all memory associated with the station before freeing the node
            free(root->name);
            freePortList(root->port_list);
            freeCarQueue(&(root->queue));
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Station* temp = root->left;
            free(root->name);
            freePortList(root->port_list);
            freeCarQueue(&(root->queue));
            free(root);
            return temp;
        }


        Station* temp = findMinStationNode(root->right);
        
       
        int temp_id = root->id;
        char* temp_name = root->name;
       
        
   
        root->id = temp->id;
        
        free(root->name);
        root->name = strdup(temp->name); 
       

    
        root->right = removeStationFromBST(root->right, temp->id, was_removed);
    }
    return root;
}

   
// Frees the entire binary search tree of stations including all associated data
void freeStationBST(Station* station_root) {
    if (station_root == NULL) {
        return; 
    }

    // recursively realease the subtress 
         
    freeStationBST(station_root->left);
    freeStationBST(station_root->right);

    //realeaseing all dynamic content of the station
    if (station_root->name != NULL) {
        free(station_root->name);
    }
    if (station_root->port_list != NULL) {
        freePortList(station_root->port_list);
    }
    
    // release the queue waiting list of the station
    freeCarQueue(&(station_root->queue));

    // realse the station node itself
    free(station_root);
}
