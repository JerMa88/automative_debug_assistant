#include <string>

class Animal
{
    std::string species;
    int age = 0;

    int getAge(){return age;}
    void setAge(int age){this->age=age;}
    virtual void makeSound() = 0;
};