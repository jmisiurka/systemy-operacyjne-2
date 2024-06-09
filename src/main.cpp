#include "../include/Factory.h"
#include <ncurses.h>

void setupNcurses();

int main() {
    Factory factory = Factory();
    factory.run();
}