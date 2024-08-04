#ifndef SHIP_H
#define SHIP_H

typedef struct {
    int x;
    int y;
} ShipPosition;

void updateShipPosition(int x, int y);
ShipPosition getShipPosition();

#endif 
