#include <string>

#include "animal.h"

class Dog : Animal
{
    std::string breed; 

    virtual void makeSound();
    void foo();
    void linkerError();
};