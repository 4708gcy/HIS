#include "../Head/Login.h"


// 管理员登录函数，验证管理员身份并返回登录结果
Admin* adminLogin(Admin *&adminHead){
    Admin *current = adminHead;
    std::string id = inputIDCheck("请输入管理员用户ID: ");


    while (current != nullptr)
    {
  
        if (current->getUserID() == id)
        {
            if(current->adminSignIn()) // 调用管理员登录方法验证密码并设置登录状态
            {
                return current;
            }else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }

      
        current = current->next;
    }
    std::cout << "未找到管理员账号" << std::endl;
    return nullptr;
}