#include "random.h"

int genInt(int min, int max){
    return (rand()*clock()+clock())%(max-min)+min;
}
