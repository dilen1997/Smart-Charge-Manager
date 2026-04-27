#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stddef.h>
#include "station.h"
#include "load.h"
#include "display.h"
#include"menu.h"
void freeCarBST(tCar* car_root);
void freeCarQueue(Q_Car* queue);
void freePortList(Port* port_head);
void freeStationBST(Station* station_root);
// This function prints a visual representation of the car BST
void printBST(tCar* node, int level) {
    if (node == NULL) {
        return;
    }
    
    // Print the right subtree first for a more intuitive tree layout
    printBST(node->right_car, level + 1);
    
    // Print the current node's license with indentation
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    
    if (node->car) {
        printf("%d\n", node->car->license);
    }
    
    // Print the left subtree
    printBST(node->left_car, level + 1);
}


 void freeAll(Station* station_root, tCar* car_root) {
    printf("\n Freeing all allocated memory...\n");
    
    // קריאה זו מפעילה שרשרת שתשחרר גם את כל ה-Ports
    freeStationBST(station_root);
    
    // קריאה זו משחררת את כל המכוניות
    freeCarBST(car_root);

    printf(" Memory freed successfully. Goodbye! \n");
}

int main() {
    // 1. אתחול מצביעים לשורשים של מבני הנתונים
    Station* station_root = NULL;
    tCar* car_root = NULL;
    Port* port_root = NULL;
    // 2. טעינת כל המידע מהקבצים לתוך מבני הנתונים
    // הפונקציה מקבלת את כתובות המצביעים כדי שתוכל לשנות אותם
    printf("Loading data from files...\n");
    loadAllFiles(&station_root, &car_root);
    printf("Data loaded successfully.\n\n");

    // 3. הפעלת מערכת התפריט
    runMenuSystem(station_root, port_root, car_root);

    // 4. שחרור כל הזיכרון שהוקצה דינמית
 freeAll(station_root,car_root);
    return 0;
}