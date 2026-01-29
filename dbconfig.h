// dbconfig.h - PostgreSQL连接配置
#pragma once
#include <string>

namespace db {
    // 数据库连接配置
    const std::string HOST = "localhost";
    const std::string PORT = "5432";
    const std::string DB_NAME = "course_selection";
    const std::string USER = "course_user";
    const std::string PASSWORD = "course_password";
    
    // 构建连接字符串
    inline std::string getConnectionString() {
        return "host=" + HOST + 
               " port=" + PORT + 
               " dbname=" + DB_NAME + 
               " user=" + USER + 
               " password=" + PASSWORD;
    }
    
    // 备用连接字符串（不同格式）
    inline std::string getPGConnectionString() {
        return "postgresql://" + USER + ":" + PASSWORD + 
               "@" + HOST + ":" + PORT + "/" + DB_NAME;
    }
}
