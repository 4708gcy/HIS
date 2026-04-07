/** 
 * @file Login.h
 * @brief 用户登录相关功能的定义
 * @details 该头文件定义了用户登录、注册、密码验证等相关函数，以及一些辅助工具函数
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */


#ifndef LOGIN_H
#define LOGIN_H

#include "User.h"
#include "Admin.h"
#include "SHA-256.h"
#include "GetTime.h"

int UI();
void login();

#endif