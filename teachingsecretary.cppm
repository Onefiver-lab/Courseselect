export module teachingsecretary;

import user;
import iregistrar;
import semester;
import course;
import database;
import <print>;
import <memory>;
import <chrono>;
import <algorithm>;

export class TeachingSecretary : public User, public IRegistrar {
public:
    std::string employee_id;
    std::vector<std::shared_ptr<Semester>> managed_semesters;

public:
    TeachingSecretary(std::string user_id, std::string user_name,
                     std::string password, std::string emp_id);

    // 从 User 继承的数据库操作
    bool load_from_db(std::shared_ptr<DatabaseConnection> db) override;
    bool save_to_db(std::shared_ptr<DatabaseConnection> db) override;
    bool delete_from_db(std::shared_ptr<DatabaseConnection> db) override;

    // 从 IRegistrar 继承的接口
    void manage_semesters() override;
    bool create_semester(const std::string& semester_code,
                        const std::string& semester_name,
                        const std::string& start_date,
                        const std::string& end_date) override;
    bool activate_semester(const std::string& semester_code) override;
    bool deactivate_semester(const std::string& semester_code) override;

    void manage_all_courses() override;
    void manage_all_users() override;
    void process_approvals() override;
    void generate_reports() override;

    // 从 User 继承的面板显示
    void getdashboard() override;

    // 特定的数据库操作方法
    bool create_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                              const std::string& semester_code,
                              const std::string& semester_name,
                              const std::string& start_date,
                              const std::string& end_date);
    bool activate_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                                const std::string& semester_code);
    bool deactivate_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                                  const std::string& semester_code);

    std::vector<std::shared_ptr<Semester>> load_semesters_from_db(
        std::shared_ptr<DatabaseConnection> db);

    std::vector<std::shared_ptr<User>> load_all_users_from_db(
        std::shared_ptr<DatabaseConnection> db);

    std::vector<std::shared_ptr<Course>> load_all_courses_from_db(
        std::shared_ptr<DatabaseConnection> db);

    // 其他业务方法
    bool approve_registration(std::shared_ptr<DatabaseConnection> db,
                             const std::string& registration_id);

    bool reject_registration(std::shared_ptr<DatabaseConnection> db,
                            const std::string& registration_id);

    std::vector<std::pair<std::string, std::string>> get_pending_registrations(
        std::shared_ptr<DatabaseConnection> db) const;

    // 静态方法
    static std::vector<std::shared_ptr<TeachingSecretary>> load_all_secretaries(
        std::shared_ptr<DatabaseConnection> db);

private:
    void create_semester_ui(std::shared_ptr<DatabaseConnection> db);
    void view_all_semesters(std::shared_ptr<DatabaseConnection> db);
    void activate_semester_ui(std::shared_ptr<DatabaseConnection> db);
    void deactivate_semester_ui(std::shared_ptr<DatabaseConnection> db);
};



TeachingSecretary::TeachingSecretary(std::string user_id, std::string user_name,
                                   std::string password, std::string emp_id)
    : User(std::move(user_id), std::move(user_name), std::move(password))
    , employee_id(std::move(emp_id)) {}

// User 接口实现
bool TeachingSecretary::load_from_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        auto result = db->execute_params(
            "SELECT employee_id FROM teaching_secretaries WHERE user_id = $1",
            {userid}
        );

        if (result.row_count() > 0) {
            employee_id = result.get_value(0, 0);
            managed_semesters = load_semesters_from_db(db);
            return true;
        }
    } catch (const std::exception& e) {
        std::print("加载教务秘书数据失败: {}\n", e.what());
    }
    return false;
}

bool TeachingSecretary::save_to_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        db->begin_transaction();

        // 保存用户基本信息
        auto user_sql = R"(
            INSERT INTO users (user_id, username, password_hash, user_type)
            VALUES ($1, $2, $3, 'secretary')
            ON CONFLICT (user_id) DO UPDATE
            SET username = EXCLUDED.username,
                password_hash = EXCLUDED.password_hash
        )";

        db->execute_params(user_sql, {userid, username, password_hash});

        // 保存教务秘书信息
        auto secretary_sql = R"(
            INSERT INTO teaching_secretaries (user_id, employee_id)
            VALUES ($1, $2)
            ON CONFLICT (user_id) DO UPDATE
            SET employee_id = EXCLUDED.employee_id
        )";

        db->execute_params(secretary_sql, {userid, employee_id});

        db->commit();
        std::print("教务秘书 {} 信息已保存\n", username);
        return true;

    } catch (const std::exception& e) {
        try { db->rollback(); } catch (...) {}
        std::print("保存教务秘书数据失败: {}\n", e.what());
        return false;
    }
}

bool TeachingSecretary::delete_from_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        auto sql = "DELETE FROM users WHERE user_id = $1";
        db->execute_params(sql, {userid});
        return true;
    } catch (const std::exception& e) {
        std::print("删除教务秘书失败: {}\n", e.what());
        return false;
    }
}

// IRegistrar 接口实现
void TeachingSecretary::manage_semesters() {
    // 这里需要一个 DatabaseConnection 对象
    // 实际实现中应该传入 db 参数
    std::print("学期管理功能需要数据库连接\n");
}

bool TeachingSecretary::create_semester(const std::string& semester_code,
                                      const std::string& semester_name,
                                      const std::string& start_date,
                                      const std::string& end_date) {
    std::print("创建学期: {} ({})\n", semester_name, semester_code);
    return true;
}

bool TeachingSecretary::activate_semester(const std::string& semester_code) {
    std::print("激活学期: {}\n", semester_code);
    return true;
}

bool TeachingSecretary::deactivate_semester(const std::string& semester_code) {
    std::print("停用学期: {}\n", semester_code);
    return true;
}

void TeachingSecretary::manage_all_courses() {
    std::print("管理所有课程\n");
}

void TeachingSecretary::manage_all_users() {
    std::print("管理所有用户\n");
}

void TeachingSecretary::process_approvals() {
    std::print("处理审批\n");
}

void TeachingSecretary::generate_reports() {
    std::print("生成报表\n");
}

// User 接口实现
void TeachingSecretary::getdashboard() {
    std::print("\n=== 教务秘书面板 ===\n");
    std::print("姓名: {}\n", username);
    std::print("工号: {}\n", employee_id);
    std::print("管理的学期数: {}\n", managed_semesters.size());
}

// 特定的数据库操作方法
bool TeachingSecretary::create_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                                            const std::string& semester_code,
                                            const std::string& semester_name,
                                            const std::string& start_date,
                                            const std::string& end_date) {
    try {
        auto sql = R"(
            INSERT INTO semesters (semester_id, semester_name, start_date, end_date, status)
            VALUES ($1, $2, $3, $4, 'inactive')
            ON CONFLICT (semester_id) DO UPDATE
            SET semester_name = EXCLUDED.semester_name,
                start_date = EXCLUDED.start_date,
                end_date = EXCLUDED.end_date
        )";

        db->execute_params(sql, {semester_code, semester_name, start_date, end_date});

        // 创建内存对象
        auto now = std::chrono::system_clock::now();
        auto end_time = now + std::chrono::hours(24 * 120); // 120天

        auto semester = std::make_shared<Semester>(
            semester_code, semester_name, now, end_time);

        managed_semesters.push_back(semester);

        std::print("学期 {} 已创建并保存到数据库\n", semester_name);
        return true;

    } catch (const std::exception& e) {
        std::print("创建学期失败: {}\n", e.what());
        return false;
    }
}

bool TeachingSecretary::activate_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                                              const std::string& semester_code) {
    try {
        db->begin_transaction();

        // 将所有学期设置为 inactive
        auto deactivate_all = "UPDATE semesters SET status = 'inactive'";
        db->execute(deactivate_all);

        // 激活指定学期
        auto sql = R"(
            UPDATE semesters SET status = 'active'
            WHERE semester_id = $1
        )";

        db->execute_params(sql, {semester_code});

        db->commit();

        // 更新内存中的状态
        for (auto& semester : managed_semesters) {
            if (semester->semesterId == semester_code) {
                semester->status = "active";
            } else {
                semester->status = "inactive";
            }
        }

        std::print("学期 {} 已激活\n", semester_code);
        return true;

    } catch (const std::exception& e) {
        try { db->rollback(); } catch (...) {}
        std::print("激活学期失败: {}\n", e.what());
        return false;
    }
}

bool TeachingSecretary::deactivate_semester_in_db(std::shared_ptr<DatabaseConnection> db,
                                                const std::string& semester_code) {
    try {
        auto sql = R"(
            UPDATE semesters SET status = 'inactive'
            WHERE semester_id = $1
        )";

        int affected = db->execute_params(sql, {semester_code}).row_count();

        if (affected > 0) {
            // 更新内存中的状态
            for (auto& semester : managed_semesters) {
                if (semester->semesterId == semester_code) {
                    semester->status = "inactive";
                }
            }
            std::print("学期 {} 已停用\n", semester_code);
            return true;
        } else {
            std::print("未找到学期 {}\n", semester_code);
            return false;
        }

    } catch (const std::exception& e) {
        std::print("停用学期失败: {}\n", e.what());
        return false;
    }
}

std::vector<std::shared_ptr<Semester>> TeachingSecretary::load_semesters_from_db(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<Semester>> semesters;

    try {
        auto result = db->execute(R"(
            SELECT semester_id, semester_name, start_date, end_date, status
            FROM semesters
            ORDER BY start_date DESC
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string semester_id = result.get_value(i, 0);
            std::string semester_name = result.get_value(i, 1);
            std::string start_date_str = result.get_value(i, 2);
            std::string end_date_str = result.get_value(i, 3);
            std::string status = result.get_value(i, 4);

            if (!semester_id.empty() && !semester_name.empty()) {
                // 转换日期字符串为 time_point（简化处理）
                auto start_date = std::chrono::system_clock::now();
                auto end_date = std::chrono::system_clock::now();

                auto semester = std::make_shared<Semester>(
                    semester_id, semester_name, start_date, end_date);

                semester->status = status;
                semesters.push_back(semester);
            }
        }

    } catch (const std::exception& e) {
        std::print("加载学期数据失败: {}\n", e.what());
    }

    return semesters;
}

std::vector<std::shared_ptr<User>> TeachingSecretary::load_all_users_from_db(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<User>> users;

    try {
        auto result = db->execute(R"(
            SELECT u.user_id, u.username, u.user_type,
                   s.student_id, t.employee_number, ts.employee_id
            FROM users u
            LEFT JOIN students s ON u.user_id = s.user_id
            LEFT JOIN teachers t ON u.user_id = t.user_id
            LEFT JOIN teaching_secretaries ts ON u.user_id = ts.user_id
            ORDER BY u.user_type, u.username
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string user_id = result.get_value(i, 0);
            std::string username = result.get_value(i, 1);
            std::string user_type = result.get_value(i, 2);
            std::string student_id = result.get_value(i, 3);
            std::string teacher_emp = result.get_value(i, 4);
            std::string secretary_emp = result.get_value(i, 5);

            if (!user_id.empty()) {
                std::shared_ptr<User> user;

                if (user_type == "student") {
                    // 创建学生对象需要专业信息，这里简化处理
                    auto dummy_major = std::make_shared<Major>("", "");
                    user = std::make_shared<Student>(
                        user_id, username, "", student_id, dummy_major);
                } else if (user_type == "teacher") {
                    user = std::make_shared<Teacher>(
                        user_id, username, "", teacher_emp);
                } else if (user_type == "secretary") {
                    user = std::make_shared<TeachingSecretary>(
                        user_id, username, "", secretary_emp);
                } else {
                    user = std::make_shared<User>(user_id, username, "");
                }

                users.push_back(user);
            }
        }

    } catch (const std::exception& e) {
        std::print("加载用户数据失败: {}\n", e.what());
    }

    return users;
}

std::vector<std::shared_ptr<Course>> TeachingSecretary::load_all_courses_from_db(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<Course>> courses;

    try {
        auto result = db->execute(R"(
            SELECT course_id, course_name, credits, course_type,
                   max_capacity, is_elective
            FROM courses
            ORDER BY course_id
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string course_id = result.get_value(i, 0);
            std::string course_name = result.get_value(i, 1);
            int credits = result.get_value(i, 2).empty() ? 0 : std::stoi(result.get_value(i, 2));
            std::string course_type = result.get_value(i, 3);
            int max_capacity = result.get_value(i, 4).empty() ? 50 : std::stoi(result.get_value(i, 4));
            bool is_elective = result.get_value(i, 5) == "1" || result.get_value(i, 5) == "true";

            if (!course_id.empty()) {
                std::shared_ptr<Course> course;

                if (course_type == "major") {
                    course = std::make_shared<MajorCourse>(
                        course_id, course_name, credits, is_elective, max_capacity);
                } else {
                    course = std::make_shared<PublicCourse>(
                        course_id, course_name, credits, max_capacity);
                }

                courses.push_back(course);
            }
        }

    } catch (const std::exception& e) {
        std::print("加载课程数据失败: {}\n", e.what());
    }

    return courses;
}

// 其他业务方法
bool TeachingSecretary::approve_registration(std::shared_ptr<DatabaseConnection> db,
                                           const std::string& registration_id) {
    try {
        auto sql = R"(
            UPDATE registrations
            SET status = 'approved',
                approved_by = $1,
                approved_at = CURRENT_TIMESTAMP
            WHERE registration_id = $2 AND status = 'pending'
        )";

        int affected = db->execute_params(sql, {userid, registration_id}).row_count();

        if (affected > 0) {
            std::print("注册申请 {} 已批准\n", registration_id);
            return true;
        } else {
            std::print("未找到待审批的注册申请 {}\n", registration_id);
            return false;
        }

    } catch (const std::exception& e) {
        std::print("批准注册失败: {}\n", e.what());
        return false;
    }
}

bool TeachingSecretary::reject_registration(std::shared_ptr<DatabaseConnection> db,
                                          const std::string& registration_id) {
    try {
        auto sql = R"(
            UPDATE registrations
            SET status = 'rejected',
                approved_by = $1,
                approved_at = CURRENT_TIMESTAMP,
                rejection_reason = 'Rejected by teaching secretary'
            WHERE registration_id = $2 AND status = 'pending'
        )";

        int affected = db->execute_params(sql, {userid, registration_id}).row_count();

        if (affected > 0) {
            std::print("注册申请 {} 已拒绝\n", registration_id);
            return true;
        } else {
            std::print("未找到待审批的注册申请 {}\n", registration_id);
            return false;
        }

    } catch (const std::exception& e) {
        std::print("拒绝注册失败: {}\n", e.what());
        return false;
    }
}

std::vector<std::pair<std::string, std::string>> TeachingSecretary::get_pending_registrations(
    std::shared_ptr<DatabaseConnection> db) const {

    std::vector<std::pair<std::string, std::string>> registrations;

    try {
        auto result = db->execute(R"(
            SELECT r.registration_id, s.username || ' - ' || c.course_name as description
            FROM registrations r
            JOIN students s ON r.student_id = s.student_id
            JOIN courses c ON r.course_id = c.course_id
            WHERE r.status = 'pending'
            ORDER BY r.created_at
        )");

        for (int i = 0; i < result.row_count(); i++) {
            registrations.emplace_back(
                result.get_value(i, 0),
                result.get_value(i, 1)
            );
        }

    } catch (const std::exception& e) {
        std::print("获取待审批注册失败: {}\n", e.what());
    }

    return registrations;
}

// 静态方法
std::vector<std::shared_ptr<TeachingSecretary>> TeachingSecretary::load_all_secretaries(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<TeachingSecretary>> secretaries;

    try {
        auto result = db->execute(R"(
            SELECT u.user_id, u.username, ts.employee_id
            FROM users u
            JOIN teaching_secretaries ts ON u.user_id = ts.user_id
            WHERE u.user_type = 'secretary'
            ORDER BY ts.employee_id
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string user_id = result.get_value(i, 0);
            std::string username = result.get_value(i, 1);
            std::string employee_id = result.get_value(i, 2);

            auto secretary = std::make_shared<TeachingSecretary>(
                user_id, username, "", employee_id);

            secretaries.push_back(secretary);
        }

    } catch (const std::exception& e) {
        std::print("加载所有教务秘书失败: {}\n", e.what());
    }

    return secretaries;
}

// 私有方法
void TeachingSecretary::create_semester_ui(std::shared_ptr<DatabaseConnection> db) {
    std::string code, name, start, end;

    std::print("学期代码: ");
    std::cin >> code;

    std::print("学期名称: ");
    std::cin.ignore();
    std::getline(std::cin, name);

    std::print("开始日期 (YYYY-MM-DD): ");
    std::cin >> start;

    std::print("结束日期 (YYYY-MM-DD): ");
    std::cin >> end;

    create_semester_in_db(db, code, name, start, end);
}

void TeachingSecretary::view_all_semesters(std::shared_ptr<DatabaseConnection> db) {
    auto semesters = load_semesters_from_db(db);

    if (semesters.empty()) {
        std::print("暂无学期数据\n");
    } else {
        std::print("\n=== 所有学期 ===\n");
        for (const auto& semester : semesters) {
            std::print("{} - {} ({})\n",
                      semester->semesterId,
                      semester->name,
                      semester->status);
        }
    }
}

void TeachingSecretary::activate_semester_ui(std::shared_ptr<DatabaseConnection> db) {
    std::string code;
    std::print("请输入要激活的学期代码: ");
    std::cin >> code;

    activate_semester_in_db(db, code);
}

void TeachingSecretary::deactivate_semester_ui(std::shared_ptr<DatabaseConnection> db) {
    std::string code;
    std::print("请输入要停用的学期代码: ");
    std::cin >> code;

    deactivate_semester_in_db(db, code);
}
