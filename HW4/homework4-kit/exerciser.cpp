#include "exerciser.h"

void exercise(connection *C) {   
    query1(C, 0, 35, 40, 0, 0, 0, 0, 5, 6, 0, 0, 0, 0, 0.9, 1.1, 0, 0, 0);
    query1(C, 0, 35, 40, 0, 0, 0, 0, 5, 6, 0, 0, 0, 1, 0.9, 1.1, 0, 0, 0);
    query1(C, 0, 35, 40, 0, 0, 0, 0, 5, 6, 0, 0, 0, 0, 0.9, 1.1, 1, 0.9, 1.1);
    query2(C, "DarkBlue");
    query3(C, "Duke");
    query4(C, "NC", "DarkBlue");
    query5(C, 10);
}
