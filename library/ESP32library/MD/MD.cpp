#include <MD.h>

void MD::Brake(){
    Move(0.0);
}

void MD::Reverse(bool rev){
    reverse = rev;
}
