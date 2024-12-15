#include "cat.h"
#include <iostream>

// Constructor for Cat class
Cat::Cat(const std::string &name, int age, const std::string &breed)
{
}

// Implementation of the makeSound function for Cat
void Cat::makeSound()
{
    std::cout << "Meow! I am a cat." << std::endl;
}

// Implementation of the foo function for Cat
void Cat::foo()
{
    std::cout << "This is the foo function for Cat." << std::endl;
