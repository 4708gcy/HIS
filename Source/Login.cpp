#include "../Head/Login.h"
#include <iostream>
#include <cstdlib>

int UI()
{
    std::cout << "Welcome to the Hospital Management System!" << std::endl;
    std::cout << "Please select your role:" << std::endl;
    std::cout << "1. Admin" << std::endl;
    std::cout << "2. Doctor" << std::endl;
    std::cout << "3. Nurse" << std::endl;
    std::cout << "4. Pharmacist" << std::endl;
    std::cout << "5. Patient" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    return choice;
}

void login()
{
    Admin *admin = new Admin();
    admin->setUserID("0001");
    admin->setRole(UserRole::ADMIN);
    admin->setUsername("admin");
    std::string salt = generateSalt();
    int iterations = admin->getKHashIterations();
    std::string hashedPassword = SHA256Encrypt("123456", salt, iterations);



    while (1)
    {
        int choice = UI();
        switch (choice)
        {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            exit(0);
        default:
            std::cout << "Invalid choice!" << std::endl;
            system("pause");
        }
    }
}
