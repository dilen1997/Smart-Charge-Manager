#ifndef TYPES_H
#define TYPES_H


typedef struct Date Date;
typedef struct Car Car;
typedef struct Port Port;
typedef struct Station Station;
typedef struct tCar tCar;
typedef struct Q_Car Q_Car;


typedef enum { SLOW, MID, FAST } t_PortType;

typedef struct Date
{
   int year ;
   int month ; 
   int day ; 
   int hour;
   int min; 
}Date ; 


#endif