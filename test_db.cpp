#include <iostream>
#include <pqxx/pqxx>

int main() {
    try {
        // 连接字符串
        std::string conn_str = "host=localhost port=5432 "
                              "dbname=course_selection "
                              "user=course_user password=course_password";
        
        std::cout << "正在连接到 PostgreSQL..." << std::endl;
        
        // 建立连接
        pqxx::connection conn(conn_str);
        
        if (conn.is_open()) {
            std::cout << "✅ 成功连接到PostgreSQL数据库！" << std::endl;
            std::cout << "数据库: " << conn.dbname() << std::endl;
            std::cout << "主机: " << conn.hostname() << std::endl;
            std::cout << "端口: " << conn.port() << std::endl;
            
            // 执行简单查询
            pqxx::work txn(conn);
            pqxx::result res = txn.exec("SELECT COUNT(*) FROM student");
            
            std::cout << "\n📊 学生数量: " << res[0][0].as<int>() << std::endl;
            
            // 获取学生列表
            res = txn.exec("SELECT id, name FROM student ORDER BY id");
            std::cout << "\n👨‍🎓 学生列表:" << std::endl;
            for (auto row : res) {
                std::cout << "  - [" << row[0].c_str() << "] " << row[1].c_str() << std::endl;
            }
            
            // 获取课程列表
            res = txn.exec("SELECT c.id, c.name, c.credits, t.name as teacher_name "
                          "FROM course c LEFT JOIN teacher t ON c.teacher_id = t.id "
                          "ORDER BY c.id");
            
            std::cout << "\n📚 课程列表:" << std::endl;
            for (auto row : res) {
                std::cout << "  - [" << row[0].c_str() << "] " << row[1].c_str() 
                         << " (" << row[2].c_str() << "学分)" 
                         << " - 教师: " << (row[3].is_null() ? "未分配" : row[3].c_str()) 
                         << std::endl;
            }
            
            // 获取选课统计
            res = txn.exec("SELECT COUNT(DISTINCT student_id) as student_count, "
                          "COUNT(DISTINCT course_id) as course_count "
                          "FROM enrollment");
            
            if (!res.empty()) {
                std::cout << "\n📈 选课统计:" << std::endl;
                std::cout << "  选课学生数: " << res[0][0].as<int>() << std::endl;
                std::cout << "  被选课程数: " << res[0][1].as<int>() << std::endl;
            }
            
            txn.commit();
            
            // 正确关闭连接（可选，因为会在析构时自动关闭）
            conn.close();  // 使用 close() 而不是 disconnect()
            
            std::cout << "\n✅ 数据库测试完成！" << std::endl;
            return 0;
        } else {
            std::cout << "❌ 无法连接到数据库" << std::endl;
            return 1;
        }
    } catch (const std::exception &e) {
        std::cerr << "\n❌ 错误: " << e.what() << std::endl;
        
        // 提供更多调试信息
        std::cerr << "\n💡 可能的解决方案:" << std::endl;
        std::cerr << "1. 确保PostgreSQL服务正在运行: sudo systemctl status postgresql" << std::endl;
        std::cerr << "2. 检查用户密码是否正确" << std::endl;
        std::cerr << "3. 验证数据库是否存在: sudo -u postgres psql -l" << std::endl;
        std::cerr << "4. 检查连接权限: sudo nano /var/lib/postgres/data/pg_hba.conf" << std::endl;
        
        return 1;
    }
}
