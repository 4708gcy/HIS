#include "../Head/User.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>


int User::getKHashIterations() const { return kHashIterations; }
void User::setUserID(const std::string &uid) { userID = uid; }
void User::setRole(UserRole r) { role = r; }




User::~User()
{
}
