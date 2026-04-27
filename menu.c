#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#include<float.h>
#include"cars.h"
#include"port.h"
#include "station.h"
#include"menu.h"

double calculateDistance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}


 //Recursively traverses the station BST to find the nearest one.

void findNearest_recursive(Station* node, float userX, float userY, Station** nearest, double* minDistance) {
    if (node == NULL) {
        return;
    }
    // In-order traversal
    findNearest_recursive(node->left, userX, userY, nearest, minDistance);

    double dist = calculateDistance(userX, userY, node->coordX, node->coordY);
    if (dist < *minDistance) {
        *minDistance = dist;
        *nearest = node;
    }

    findNearest_recursive(node->right, userX, userY, nearest, minDistance);
}
//1
void locNearSt(Station* station_root) {
    if (station_root == NULL) {
        printf("The station database is empty. Cannot find a station.\n");
        return;
    }

    float userX, userY;
    printf("Please enter your location (X Y coordinates\n): ");
    if (scanf("%f %f", &userX, &userY) != 2) {
        printf("Invalid input for coordinates.\n");
        return;
    }

    Station* nearestStation = NULL;
    double minDistance = DBL_MAX; // Initialize with the largest possible double value

    // Recursively find the nearest station
    findNearest_recursive(station_root, userX, userY, &nearestStation, &minDistance);

    if (nearestStation == NULL) {
        printf("Could not find any station in the database.\n");
        return;
    } 
        printf("\n--- Nearest Station Found ---\n");
    printf("Station Name: %s\n", nearestStation->name);
    printf("Station ID: %d\n", nearestStation->id);
    printf("Coordinates: (%.2f, %.2f)\n", nearestStation->coordX, nearestStation->coordY);
    printf("Distance from you: %.2f units\n", minDistance);
    printf("---------------------------\n");
}

void assignCar2Port(Car* car, Port* port) {
    // --- Safety Checks ---
    if (car == NULL || port == NULL) {
        printf("Error: Cannot assign a null car or port.\n");
        return;
    }
    // Assuming 0 for AVAILABLE and 1 for BUSY
    if (port->status != 0) {
        printf("Error: Port %d at station %d is already busy.\n", port->portNumber, port->stationID);
        return;
    }

    // --- 1. Update Port Details ---
    port->status = 1; // Set port status to BUSY
    port->carLicense = car->license; // Link the car's license to the port
    port->p2car = car; // Create a direct link from the port to the car object
    // port->date = getCurrentDate(); // You need a function to set the date/time

    // --- 2. Update Car Details ---
    car->port = port; // Link the car to the port object
    car->station_id = port->stationID;
    car->port_Num = port->portNumber;
    car->inqueue = 0; // The car is now charging, not in a queue

    printf("Success: Car %d has been assigned to Port %d at Station %d.\n",
           car->license, port->portNumber, port->stationID);
}



//2
void chargeCar(tCar** car_root_ptr, Station* station_root) {
    int licenseNb, stationId;

    printf("Enter car license number: ");
    if (scanf("%d", &licenseNb) != 1) {
        printf("Invalid input for license number.\n");
        while (getchar() != '\n');
        return;
    }
    printf("Enter station ID: ");
    if (scanf("%d", &stationId) != 1) {
        printf("Invalid input for station ID.\n");
        while (getchar() != '\n');
        return;
    }

    Station* station = searchStation(station_root, stationId);
    if (station == NULL) {
        printf("Error: Station with ID %d not found.\n", stationId);
        return;
    }

    Car* car = searchCar(*car_root_ptr, licenseNb);
    if (car == NULL) {
        printf("Car not found in database. Please enter car details.\n");
        car = (Car*)malloc(sizeof(Car));
        if (!car) {
            printf("Memory allocation failed\n");
            return;
        }
        car->license = licenseNb;
        
        char typeStr[10];
        printf("Enter preferred port type (SLOW, MID, FAST): ");
        scanf("%s", typeStr);
        
        
        car->port_type = stringToPortType(typeStr);

        car->total_payed = 0;
        car->station_id = -1;
        car->port_Num = -1;
        car->port = NULL;
        car->inqueue = 0;

        *car_root_ptr = addNewCarToBST(*car_root_ptr, car);
        printf("New car created and added to the database.\n");
    }

    Port* available_port = NULL;
    Port* current_port = station->port_list;

    while (current_port != NULL) {
        if (current_port->status == 0 && current_port->portType == car->port_type) {
            available_port = current_port;
            break;
        }
        current_port = current_port->next;
    }

    if (available_port != NULL) {
        printf("\nFound available Port %d. Assigning car...\n", available_port->portNumber);
        assignCar2Port(car, available_port);
    } else {
        printf("\nNo available ports of type '%s' at station %d.\n", portTypeToString(car->port_type), station->id);
        printf("Adding car %d to the waiting queue.\n", car->license);
        
        addCarToQueue(&(station->queue), car);
        car->inqueue = 1;
        car->station_id = station->id;
    }
    
}


   //3
  //Checks and displays the status of a specific car.
time_t DateToTimeT(Date d) {
    struct tm time_struct = {0};
    time_struct.tm_year = d.year - 1900; // tm_year is years since 1900
    time_struct.tm_mon  = d.month - 1;  // tm_mon is 0-11
    time_struct.tm_mday = d.day;
    time_struct.tm_hour = d.hour;
    time_struct.tm_min  = d.min;
    time_struct.tm_sec  = 0;
    return mktime(&time_struct);
}


void checkCarStatus(tCar* car_root, Station* station_root) {
    int licenseNb;
    printf("\nPlease enter the car license number to check its status: ");
    if (scanf("%d", &licenseNb) != 1) {
        printf("Invalid input for license number.\n");
        while (getchar() != '\n');
        return;
    }

    // Find the car using the provided car_root
    Car* car = searchCar(car_root, licenseNb);

    if (car == NULL) {
        printf("Car with license number %d was not found in the system.\n", licenseNb);
        return;
    }

    // Case 1: Car is currently charging
    if (car->port != NULL && car->inqueue == 0) {
        Station* station = searchStation(station_root, car->station_id);
        
        
        if (car->port) { // Safety check
            // 1. Convert the start time from your Date struct to a standard time_t value
            time_t startTime = DateToTimeT(car->port->date);
            
            // 2. Get the current time
            time_t currentTime = time(NULL);
            
            // 3. Calculate the difference in minutes
            double minutes_charging = difftime(currentTime, startTime) / 60.0;

            printf("\n--- Car Status ---\n");
            printf("Car %d is currently CHARGING.\n", licenseNb);
            printf("  - Station: %s (ID: %d)\n", station ? station->name : "Unknown", car->station_id);
            printf("  - Port Number: %d\n", car->port_Num);
            printf("  - Charging for: %.0f minutes.\n", minutes_charging);
            printf("--------------------\n");
        }
    } 
    // Case 2: Car is waiting in a queue
    else if (car->inqueue == 1) {
        Station* station = searchStation(station_root, car->station_id);
        if (station) { // Safety check
            int position = getPositionInQueue(&(station->queue), licenseNb);

            printf("\n--- Car Status ---\n");
            printf("Car %d is currently IN QUEUE.\n", licenseNb);
            printf("  - Station: %s (ID: %d)\n", station->name, car->station_id);
            printf("  - Position in queue: %d\n", position);
            printf("  - Waiting for a %s port.\n", portTypeToString(car->port_type));
            printf("--------------------\n");
        }
    } 
    // Case 3: Car is in the system but idle
    else {
        printf("\n--- Car Status ---\n");
        printf("Car %d is in the system but is currently IDLE (not charging or in a queue).\n", licenseNb);
        printf("--------------------\n");
    }
}
//4



void stopCharge(tCar* car_root, Station* station_root) {
    int licenseNb;
    printf("Enter the license number of the car to stop charging: ");
    if (scanf("%d", &licenseNb) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    Car* car = searchCar(car_root, licenseNb);
    if (!car) {
        printf("Car with license %d not found.\n", licenseNb);
        return;
    }

    if (car->port == NULL) {
        printf("Car %d is not currently charging.\n", licenseNb);
        return;
    }

    Port* port = car->port;
    Station* station = searchStation(station_root, port->stationID);

    // Free the port
    port->status = 0;
    port->carLicense = 0;
    port->p2car = NULL;

    // Update the car's status
    car->port = NULL;
    car->station_id = -1;
    car->port_Num = -1;
    
    printf("Charging stopped for car %d. Port %d at station %d is now free.\n", 
           licenseNb, port->portNumber, port->stationID);

    // Check if another car from the queue can use the now-free port
    if (station) {
        // CORRECTED: Pass the port pointer itself, not its type.
        checkQueueAndStartNextCharge(station, port);
    }
}


//5 in display.c




//6 in display.c



//7
//Generates and displays a statistics report for a specific station.
void reportStStat(Station* station_root) {
    int station_id;
    printf("\nPlease enter station ID for statistics report: ");
    if (scanf("%d", &station_id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }

    // Find the station using the provided root
    Station* station = searchStation(station_root, station_id);
    if (station == NULL) {
        printf("Error: Station with ID %d not found.\n", station_id);
        return;
    }

    printf("\n--- Statistics Report for Station %d (%s) ---\n", station->id, station->name);

    // 1. Calculate port utilization rate 
    int occupied_ports = countOccupiedPorts(station);
    float utilization = (station->numOfPorts > 0) ? ((float)occupied_ports / station->numOfPorts) * 100 : 0;
    printf("1. Port Utilization: %.2f%%\n", utilization);

    // 2. Calculate ratio of available ports to number of vehicles
    // CORRECTED: The queue is in station->queue, not station->car
    int queued_cars = countCarsInQueue(&(station->queue));
    int total_cars_at_station = occupied_ports + queued_cars;
    
    // Assuming SLOW and MID are AC, FAST is DC
    // Make sure the enum values for SLOW, MID, FAST are defined and accessible here.
    int available_ac = countAvailablePortsByType(station, SLOW) + countAvailablePortsByType(station, MID);
    int available_dc = countAvailablePortsByType(station, FAST);

    float ac_ratio = (total_cars_at_station > 0) ? (float)available_ac / total_cars_at_station : (float)available_ac;
    float dc_ratio = (total_cars_at_station > 0) ? (float)available_dc / total_cars_at_station : (float)available_dc;
    
    printf("2. Available AC Ports to Cars Ratio: %.2f\n", ac_ratio);
    printf("3. Available DC Ports to Cars Ratio: %.2f\n", dc_ratio);

    // 4. load level calculation
    printf("4. Load Level Analysis:\n");
    int available_ports = available_ac + available_dc;
    float load_metric = (total_cars_at_station > 0) ? (float)available_ports / total_cars_at_station : (float)available_ports;

    if (queued_cars == 0 && load_metric > 1) {
        printf("   - Status: No Load (Metric: %.2f, Queue is empty)\n", load_metric);
    } else if (load_metric < 0.2) {
        printf("   - Status: Very Heavy Load (Metric: %.2f)\n", load_metric);
    } else if (load_metric >= 0.2 && load_metric < 1) {
        printf("   - Status: Low Load (Metric: %.2f)\n", load_metric);
    } else {
        printf("   - Status: Balanced (Metric: %.2f)\n", load_metric);
    }

    printf("------------------------------------------------------\n");
}



//8
void dispTopCustomers(tCar* car_root) {
    printf("\n--- Displaying Top 5 Customers by Spending ---\n");
    if (car_root == NULL) {
        printf("No car data available.\n");
        printf("------------------------------------------\n");
        return;
    }

    // 1. Count the total number of cars to allocate the exact memory needed.
    int car_count = countCars_recursive(car_root);
    if (car_count == 0) {
        printf("No cars found in the system.\n");
        printf("------------------------------------------\n");
        return;
    }

    // 2. Dynamically allocate an array of pointers to Car.
    Car** all_cars_array = (Car**)malloc(car_count * sizeof(Car*));
    if (all_cars_array == NULL) {
        printf("Error: Memory allocation failed for car array.\n");
        return;
    }

    // 3. Collect all cars from the BST into the array.
    int index = 0;
    collectCars_recursive(car_root, all_cars_array, &index);

    // 4. Sort the array by total spending (descending) using qsort.
    qsort(all_cars_array, car_count, sizeof(Car*), compareCarsBySpending);

    // 5. Print the top 5 customers (or fewer if there are less than 5).
    printf("Rank | License    | Total Spending\n");
    printf("------------------------------------------\n");
    int top_n = (car_count < 5) ? car_count : 5;
    for (int i = 0; i < top_n; i++) {
        printf("#%-4d| %-10d | %.2f ILS\n",
               i + 1,
               all_cars_array[i]->license,
               all_cars_array[i]->total_payed);
    }
    printf("------------------------------------------\n");

    // 6. Free the dynamically allocated array to prevent memory leaks.
    free(all_cars_array);
}

void collectCars_recursive(tCar* node, Car** array, int* index) {
    if (node == NULL || node->car == NULL) {
        return;
    }
    // In-order traversal to fill the array
    collectCars_recursive(node->left_car, array, index);
    
    array[*index] = node->car;
    (*index)++;
    
    collectCars_recursive(node->right_car, array, index);
}


//9 in port.c 


//10 in port.c

//11 in port.c 

//12
    // Removes a customer from the system.                
void remCustomer(tCar** car_root_ptr) {
    int licenseNb;
    printf("\n--- Remove a Customer ---\n");
    
    printf("Enter the license number of the customer to remove: ");
    if (scanf("%d", &licenseNb) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    // Find the car to ensure it exists
    Car* car = searchCar(*car_root_ptr, licenseNb);
    if (car == NULL) {
        printf("Error: Customer with license number %d not found.\n", licenseNb);
        return;
    }

    
    if (car->port != NULL || car->inqueue == 1) {
        printf("Error: Cannot remove customer while their car is active.\n");
        if (car->port != NULL) {
            printf("The car is currently charging at station %d, port %d.\n", car->station_id, car->port_Num);
        } else {
            printf("The car is currently waiting in a queue at station %d.\n", car->station_id);
        }
        return;
    }

    //  Ask for confirmation
    char confirm;
    printf("Customer found with license %d. Are you sure you want to remove this customer? (y/n): ", licenseNb);
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        int was_removed = 0;
        // Call the recursive removal function
        *car_root_ptr = removeCarFromBST(*car_root_ptr, licenseNb, &was_removed);
        if (was_removed) {
            printf("Customer removed successfully.\n");
        }
    } else {
        printf("Operation canceled.\n");
    }
}


//13
 //closes a station by releasing all cars and removing the station from the system
void closeStation(Station** station_root_ptr) {
    int stationId;
    printf("\n--- Close a Station (Force Operation) ---\n");
    
    printf("Enter the Station ID to close: ");
    if (scanf("%d", &stationId) != 1) {
        printf("Invalid input.\n");
        while(getchar() != '\n');
        return;
    }

    // 1. Find the station to ensure it exists
    Station* station = searchStation(*station_root_ptr, stationId);
    if (station == NULL) {
        printf("Error: Station with ID %d not found.\n", stationId);
        return;
    }

    // 2. Ask for confirmation
    char confirm;
    printf("WARNING: This will forcefully release all cars and permanently delete station %s (ID: %d).\n", station->name, station->id);
    printf("Are you sure you want to continue? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        // Forcefully release all cars from ports and queues
        forceReleaseCarsFromStation(station);

        // Call the recursive removal function to delete the station and free its memory
        int was_removed = 0;
        *station_root_ptr = removeStationFromBST(*station_root_ptr, stationId, &was_removed);
        if (was_removed) {
            printf("Station and all its assets have been forcefully closed and removed.\n");
        }
    } else {
        printf("Operation canceled.\n");
    }
}

// Prints the main menu options to the console.
 
void printMenu(void) {
    printf("\n--- Main Menu ---\n");
    printf("1. Locate nearest station\n");
    printf("2. Charge car\n");
    printf("3. Check car status\n");
    printf("4. Stop charge\n");
    printf("5. Display all stations\n");
    printf("6. Display cars at a station\n");
    printf("7. Report station statistics\n");
    printf("8. Display top customers\n");
    printf("9. Add a new port\n");
    printf("10. Release ports\n");
    printf("11. Remove out-of-order port\n");
    printf("12. Remove a customer\n");
    printf("13. Close a station\n");
    printf("14. Exit System\n");
    printf("-------------------\n");
    printf("Enter your choice: \n");
}

void runMenuSystem(Station* station_root, Port* port_root, tCar* car_root) {
    int choice = 0;

    do {
        printMenu();
        
        if (scanf("%d", &choice) != 1) {
            // Clear input buffer on invalid input
            while (getchar() != '\n');
            choice = -1; // Set to an invalid choice to trigger the default case
        }
    
        switch (choice) {
            case 1:
                locNearSt(station_root);
                printf("Function 'Locate nearest station' called.\n");
                break;
            case 2:
                 chargeCar(&car_root,station_root);
                printf("Function 'Charge car' called.\n");
                break;
            case 3:
                 checkCarStatus(car_root,station_root);
                printf("Function 'Check car status' called.\n");
                break;
            case 4:
                 stopCharge(car_root , station_root);
                printf("Function 'Stop charge' called.\n");
                break;
            case 5:
                 dispAllSt(station_root);
                printf("Function 'Display all stations' called.\n");
                break;
            case 6:
                 dispCarsAtSt(station_root);
                printf("Function 'Display cars at a station' called.\n");
                break;
            case 7:
                 reportStStat(station_root);
                printf("Function 'Report station statistics' called.\n");
                break;
            case 8:
                 dispTopCustomers(car_root);
                printf("Function 'Display top customers' called.\n");
                break;
            case 9:
                 addNewPort(station_root);
                printf("Function 'Add a new port' called.\n");
                break;
            case 10:
                 releasePorts(station_root);
                printf("Function 'Release ports' called.\n");
                break;
            case 11:
                 remOutOrderPort(station_root);
                printf("Function 'Remove out-of-order port' called.\n");
                break;
            case 12:
                 remCustomer(&car_root);
                printf("Function 'Remove a customer' called.\n");
                break;
            case 13:
                 closeStation(&station_root);
                printf("Function 'Close a station' called.\n");
                break;
            case 14:
                printf("Exiting menu...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 14.\n");
                break;
        }
    } while (choice != 14);
    
}

