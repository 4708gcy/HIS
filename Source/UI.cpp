#include "../Head/UI.h"

// 开始界面，显示欢迎信息和主菜单
int beginUI()
{
    std::cout << "欢迎使用HIS系统!" << std::endl;
    std::cout << "请选择你的身份" << std::endl;
    std::cout << "1. 管理员" << std::endl;
    std::cout << "2. 医生" << std::endl;
    std::cout << "3. 护士" << std::endl;
    std::cout << "4. 药剂师" << std::endl;
    std::cout << "5. 患者" << std::endl;
    std::cout << "0. 退出系统" << std::endl;

    int choice;
    std::cin >> choice;
    return choice;

}

// 管理员科室管理菜单(可扩展)
std::string adminDepartmentMenu()
{
    std::cout << "科室选择界面" << std::endl;
    std::cout << "请选择你要进入的科室:" << std::endl;
    std::cout << "1. 内科" << std::endl;
    std::cout << "2. 外科" << std::endl;
    std::cout << "3. 妇产科" << std::endl;
    std::cout << "4. 急诊科" << std::endl;
    std::cout << "5. 儿科" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice;
    std::cin >> choice;
    switch (choice)
    {
    case 0:
        return "0";
    case 1:
        return "内科";
    case 2:
        return "外科";
    case 3:
        return "妇产科";
    case 4:
        return "急诊科";
    case 5:
        return "儿科";
    default:
        return "";
    }
    return "";
}

// 管理员身份主菜单
int adminMenu()
{
    std::cout << "欢迎进入管理员界面!" << std::endl;
    std::cout << "请选择操作:" << std::endl;
    std::cout << "1. 账户管理" << std::endl;
    std::cout << "2. 医疗记录管理" << std::endl;
    std::cout << "3. 药品管理" << std::endl;
    std::cout << "4. 药品使用记录管理" << std::endl;
    std::cout << "0. 退出登录" << std::endl;

    int choice;
    std::cin >> choice;
    return choice;
}


// 管理员医疗记录管理菜单
int adminMedicalRecordMenu()
{
    std::cout << "医疗记录管理界面" << std::endl;
    std::cout << "请选择你要管理的医疗记录类型:" << std::endl;
    std::cout << "1. 挂号记录" << std::endl;
    std::cout << "2. 看诊记录" << std::endl;
    std::cout << "3. 检查记录" << std::endl;
    std::cout << "4. 住院记录" << std::endl;
    std::cout << "5. 用药记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice;
    std::cin >> choice;
    return choice;
}

// 管理员挂号记录管理菜单
int adminRegistrationManagementMenu()
{
    std::cout << "挂号记录管理界面" << std::endl;
    std::cout << "请选择你要进行的操作:" << std::endl;
    std::cout << "1. 查看挂号记录" << std::endl;
    std::cout << "2. 修改挂号状态" << std::endl;
    std::cout << "3. 删除挂号记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice;
    std::cin >> choice;
    return choice;
}

// 管理员挂号记录查看方式选择菜单
int adminRegistrationViewMenu(){
    std::cout << "请选择你要查看的方式:" << std::endl;
    std::cout << "1. 查看该科室的所有挂号记录" << std::endl;
    std::cout << "2. 根据挂号状态查看" << std::endl;
    std::cout << "3. 根据患者ID查看" << std::endl;
    std::cout << "4. 根据医生ID查看" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice;
    std::cin >> viewChoice;
    return viewChoice;
}