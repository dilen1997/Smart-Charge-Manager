#define _CRT_SECURE_NO_WARNINGS
#include"cars.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h> 
#include"station.h"
#include"port.h"
tCar* findMinValueNode(tCar* node);

 //adds a new car to the queue
void addCarToQueue(Q_Car* queue, Car* car) {
    // 1. memory allocation for the new node 
    Car_Node* newNode = (Car_Node*)malloc(sizeof(Car_Node));
    if (!newNode) {
        printf("Memory allocation failed for new queue node\n");
        return;
    }

    //Initialization new node 
    newNode->car = car;
    newNode->next_car = NULL; // new node is last, thats why he is a pointer 

    //in case queue is empty 
    if (queue->rear == NULL) {
        // if queue is empty new node is the first and last 
        queue->front = newNode;
        queue->rear = newNode;
    } 
    // 
    // in case line isn't empty 
    else {
        

        queue->rear->next_car = newNode;
        
        
    // updatind the pointer so it will point on the new node 
        queue->rear = newNode;
    }
}

// dequeues a car from the front of the queue
Car* dequeueCar(Q_Car* queue) {
    if (!queue || !queue->front) return NULL;
    Car_Node* temp = queue->front;
    Car* car = temp->car;
    queue->front = (Car_Node*)temp->next_car;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    return car;
}





// Counts the number of occupied ports in a station
int countOccupiedPorts(Station* station) {
    if (!station || !station->port_list) return 0;
    int count = 0;
    Port* current = station->port_list;
    while (current) {
        if (current->status == 1) {
            count++;
        }
        current = current->next;
    }
    return count;
}






// Counts the number of cars in the queue
int countCarsInQueue(Q_Car* queue) {
    int count = 0;
    Car_Node* current = queue->front;
    while (current != NULL) {
        count++;
        current = current->next_car;
    }
    return count;
}



// Compares two cars by their total spending used for sorting
int compareCarsBySpending(const void* a, const void* b) {
    Car* car_a = *(Car**)a;
    Car* car_b = *(Car**)b;

    if (car_b->total_payed > car_a->total_payed) {
        return 1; // b is greater
    }
    if (car_b->total_payed < car_a->total_payed) {
        return -1; // a is greater
    }
    return 0; // they are equal
}




// Finds the minimum value node in a BST
tCar* findMinValueNode(tCar* node) {
    tCar* current = node;
    // Loop down to find the leftmost leaf
    while (current && current->left_car != NULL) {
        current = current->left_car;
    }
    return current;
}


// Removes a car from the BST by its license number
tCar* removeCarFromBST(tCar* root, int licenseNb, int* was_removed) {
    // Base case: If the tree is empty, the car is not here.
    if (root == NULL) {
        return root;
    }

    
    if (licenseNb < root->car->license) {
        root->left_car = removeCarFromBST(root->left_car, licenseNb, was_removed);
    } else if (licenseNb > root->car->license) {
        root->right_car = removeCarFromBST(root->right_car, licenseNb, was_removed);
    } else {
        *was_removed = 1;

        //Node with only one child or no child
        if (root->left_car == NULL) {
            tCar* temp = root->right_car;
            free(root->car); 
            free(root);      
            return temp;
        } else if (root->right_car == NULL) {
            tCar* temp = root->left_car;
            free(root->car);
            free(root);
            return temp;
        }

       
     
        tCar* temp = findMinValueNode(root->right_car);
        Car* temp_car_ptr = root->car;
        root->car = temp->car;
        temp->car = temp_car_ptr; 

        
        root->right_car = removeCarFromBST(root->right_car, temp->car->license, was_removed);
    }
    return root;
}

// Frees the memory allocated for the car queue
void freeCarQueue(Q_Car* queue) {
    if (queue == NULL) return;

    Car_Node* current = queue->front;
    Car_Node* next_node;

    while (current != NULL) {
        next_node = current->next_car;
    
        free(current); 
        current = next_node;
    }
    queue->front = NULL;
    queue->rear = NULL;
}

  //releases all cars from a station, including those in charging ports and the queue.
void forceReleaseCarsFromStation(Station* station) {
    if (station == NULL) return;

    printf("Releasing all cars from station %d...\n", station->id);

    // Release cars from charging ports
    Port* current_port = station->port_list;
    while(current_port != NULL) {
        if(current_port->status == 1 && current_port->p2car != NULL) {
            current_port->p2car->port = NULL;
            current_port->p2car->station_id = -1;
            current_port->p2car->port_Num = -1;
        }
        current_port = current_port->next;
    }

    // Release cars from the queue
    Car_Node* current_car_node = station->queue.front;
    while(current_car_node != NULL) {
        if(current_car_node->car != NULL) {
            current_car_node->car->inqueue = 0;
            current_car_node->car->station_id = -1;
        }
        current_car_node = current_car_node->next_car;
    }
}

// Frees the memory allocated for the car BST
void freeCarBST(tCar* car_root) {
    if (car_root == NULL) {
        return;
    }

    //Recursive release of the sub-trees
    freeCarBST(car_root->left_car);
    freeCarBST(car_root->right_car);

    //Deallocate the current node's content.
    if (car_root->car != NULL) {
        free(car_root->car); 
    }

    // realease the node itself
    free(car_root); 

}

int getPositionInQueue(Q_Car* queue, int licenseNb) {
    if (queue == NULL || queue->front == NULL) {
        return 0; 
    }
    int position = 1;
    Car_Node* current = queue->front;
    while (current != NULL) {
        if (current->car && current->car->license == licenseNb) {
            return position;
        }
        position++;
        current = (Car_Node*)current->next_car;
    }
    return 0; // Car not found in this queue
}




//Recursively counts the total number of cars in the BST.
int countCars_recursive(tCar* node) {
    if (node == NULL) {
        return 0;
    }
 
    return 1 + countCars_recursive(node->left_car) + countCars_recursive(node->right_car);
}

