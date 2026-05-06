#include "LinkedList.h"
#include "User.h"

#include <cassert>
#include <string>

int main()
{
    LinkedList<std::string> ids;
    assert(ids.empty());
    ids.push_back("reg000001");
    ids.push_back("reg000002");
    ids.push_back("reg000003");
    assert(ids.size() == 3);
    assert(ids[1] == "reg000002");

    auto it = ids.erase(ids.begin() + 1);
    assert(it != ids.end());
    assert(*it == "reg000003");
    assert(ids.size() == 2);

    LinkedList<std::string> copied = ids;
    ids.push_back("reg000004");
    assert(copied.size() == 2);
    assert(ids.size() == 3);

    int counted = 0;
    for (const auto &id : ids)
    {
        assert(!id.empty());
        ++counted;
    }
    assert(counted == 3);

    assert(User::calculateStayDays("2026-01-31 08:00:00", "2026-02-02 08:00:00") == 2);
    assert(User::calculateStayDays("2025-12-31 08:00:00", "2026-01-02 08:00:00") == 2);
    assert(User::calculateStayDays("2026-05-06 08:00:00", "2026-05-06 20:00:00") == 1);
    assert(User::calculateStayDays("#", "2026-05-06 20:00:00") == 1);

    return 0;
}
