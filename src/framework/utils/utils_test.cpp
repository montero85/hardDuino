/*!\file utils_test.cpp
** \author
** \copyright TODO
** \brief Unit tests for utility classes, functions and templates.
** \details
**/
/****************************************************************/
#include "unique_ids.h"
#include <iostream>
#include <cstdint>

/*!    \brief Simple unit test for Unique IDs template module.
**
** The test is for a very basic facility, therefore it will only
** exercise an instance of UniqueId<uint8_t>.
**/
void testUniqueIDs(void)
{
    std::string errorMessage {"FAIL! - IDAllocator failed."};
    UniqueId<uint8_t> IDAllocator;
    uint8_t i = 0;
    uint8_t id;

    std::cout << "Allocate all the available ID [0-255]." << std::endl;
    do
    {
        id = IDAllocator.alloc();
        if(id != i)
        {
            throw std::runtime_error(errorMessage);
        }
        i++;
    }
    while(i > 0);
    std::cout << "Try to allocate another ID: ";
    try
    {
        id = IDAllocator.alloc();
    }
    catch(FailedIDAllocation &e)
    {
        std::cout << "OK - Raised FailedIDAllocation." << std::endl;
    }
    std::cout << "Release IDs 0, 100, 255" << std::endl;
    IDAllocator.release(0);
    IDAllocator.release(100);
    IDAllocator.release(255);
    std::cout << "Alloc -> should get 0.";
    id = IDAllocator.alloc();
    if(id != 0)
    {
        throw std::runtime_error(errorMessage);
    }
    std::cout << "- OK!" << std::endl;
    std::cout << "Alloc -> should get 100.";
    id = IDAllocator.alloc();
    if(id != 100)
    {
        throw std::runtime_error(errorMessage);
    }
    std::cout << "- OK!" << std::endl;
    std::cout << "Alloc -> should get 255.";
    id = IDAllocator.alloc();
    if(id != 255)
    {
        throw std::runtime_error(errorMessage);
    }
    std::cout << "- OK!" << std::endl;
    std::cout << "Release ID 100 (again)" << std::endl;
    IDAllocator.release(100);
    std::cout << "Alloc -> should get 100.";
    id = IDAllocator.alloc();
    if(id != 100)
    {
        throw std::runtime_error(errorMessage);
    }
    std::cout << "- OK!" << std::endl;
    std::cout << "Now release ID 50 to check wrapping" << std::endl;
    IDAllocator.release(50);
    std::cout << "Alloc -> should get 50.";
    id = IDAllocator.alloc();
    if(id != 50)
    {
        throw std::runtime_error(errorMessage);
    }
    std::cout << "- OK!" << std::endl;
    std::cout << "Try to allocate another ID (there shouldn't be more): ";
    try
    {
        id = IDAllocator.alloc();
    }
    catch(FailedIDAllocation &e)
    {
        std::cout << "OK - Raised FailedIDAllocation." << std::endl;
    }
}

int main(void)
{
    testUniqueIDs();
}
