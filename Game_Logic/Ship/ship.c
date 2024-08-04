#include "ship.h"
#include "../../Visual/Resources/xresources.h"

extern struct XResources resources;

void updateShipPosition(int x, int y) {
    resources.shipPos.x = x;
    resources.shipPos.y = y;
}

ShipPosition getShipPosition() {
    return resources.shipPos;
}
