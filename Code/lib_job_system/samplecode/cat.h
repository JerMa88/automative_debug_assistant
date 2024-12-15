#include <string>

#include "animal.h"

class Cat : Animal
{
    Cat::Cat(const std::string &name, int age, const std::string &breed);

    std::string breed; 

    virtual void makeSound();
    void foo();
};