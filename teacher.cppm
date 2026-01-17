export module teacher;

import user;
import courseselection;
import database;
import <print>;
import <memory>;
import <algorithm>;

export class Teacher : public User {
public:
    std::string employee_number;
    std::vector<std::shared_ptr<CourseSelection>> teaching_sessions;

public:
    Teacher(std::string user_id, std::string user_name, std::string password,
            std::string emp_num);

    // 数据库操作
    bool load_from_db(std::shared_ptr<DatabaseConnection> db) override;
    bool save_to_db(std::shared_ptr<DatabaseConnection> db) override;
    bool delete_from_db(std::shared_ptr<DatabaseConnection> db) override;

    // 教学任务管理
    std::vector<std::shared_ptr<CourseSelection>> load_teaching_sessions(
        std::shared_ptr<DatabaseConnection> db);
    bool assign_teaching_session(std::shared_ptr<DatabaseConnection> db,
                                const std::string& course_id,
                                const std::string& section_id);

    // 学生管理
    std::vector<std::pair<std::string, std::string>> get_students_in_course(
        std::shared_ptr<DatabaseConnection> db,
        const std::string& course_id) const;
    bool input_grades(std::shared_ptr<DatabaseConnection> db,
                     const std::string& student_id,
                     const std::string& course_id,
                     const std::string& grade);

    // 原始方法
    void view_teaching_courses();
    void manage_students(const std::string& course_id);
    bool set_capacity(const std::string& section_id, int capacity);
    void show_dashboard() override;
    void add_teaching_session(std::shared_ptr<CourseSelection> session);

    // 静态方法
    static std::vector<std::shared_ptr<Teacher>> load_all_teachers(
        std::shared_ptr<DatabaseConnection> db);
};



Teacher::Teacher(std::string user_id, std::string user_name, std::string password,
                 std::string emp_num)
    : User(std::move(user_id), std::move(user_name), std::move(password))
    , employee_number(std::move(emp_num)) {}

bool Teacher::load_from_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        auto result = db->execute_params(
            "SELECT employee_number FROM teachers WHERE user_id = $1",
            {userid}
        );

        if (result.row_count() > 0) {
            employee_number = result.get_value(0, 0);
            teaching_sessions = load_teaching_sessions(db);
            return true;
        }
    } catch (const std::exception& e) {
        std::print("加载教师数据失败: {}\n", e.what());
    }
    return false;
}

bool Teacher::save_to_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        db->begin_transaction();

        auto user_sql = R"(
            INSERT INTO users (user_id, username, password_hash, user_type)
            VALUES ($1, $2, $3, 'teacher')
            ON CONFLICT (user_id) DO UPDATE
            SET username = EXCLUDED.username,
                password_hash = EXCLUDED.password_hash
        )";

        db->execute_params(user_sql, {userid, username, password_hash});

        auto teacher_sql = R"(
            INSERT INTO teachers (user_id, employee_number)
            VALUES ($1, $2)
            ON CONFLICT (user_id) DO UPDATE
            SET employee_number = EXCLUDED.employee_number
        )";

        db->execute_params(teacher_sql, {userid, employee_number});

        db->commit();
        std::print("教师 {} 信息已保存\n", username);
        return true;

    } catch (const std::exception& e) {
        try { db->rollback(); } catch (...) {}
        std::print("保存教师数据失败: {}\n", e.what());
        return false;
    }
}

bool Teacher::delete_from_db(std::shared_ptr<DatabaseConnection> db) {
    try {
        auto sql = "DELETE FROM users WHERE user_id = $1";
        db->execute_params(sql, {userid});
        return true;
    } catch (const std::exception& e) {
        std::print("删除教师失败: {}\n", e.what());
        return false;
    }
}

std::vector<std::shared_ptr<CourseSelection>> Teacher::load_teaching_sessions(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<CourseSelection>> sessions;

    try {
        auto sql = R"(
            SELECT c.course_id, c.course_name, cs.section_id,
                   cs.time_slot, cs.classroom, cs.max_capacity
            FROM teaching_assignments ta
            JOIN courses c ON ta.course_id = c.course_id
            JOIN course_sections cs ON ta.section_id = cs.section_id
            WHERE ta.teacher_id = $1
        )";

        auto result = db->execute_params(sql, {userid});

        // 这里需要创建 CourseSelection 对象
        // 根据实际数据库结构实现

    } catch (const std::exception& e) {
        std::print("加载教学任务失败: {}\n", e.what());
    }

    return sessions;
}

bool Teacher::assign_teaching_session(std::shared_ptr<DatabaseConnection> db,
                                     const std::string& course_id,
                                     const std::string& section_id) {
    try {
        auto sql = R"(
            INSERT INTO teaching_assignments (teacher_id, course_id, section_id)
            VALUES ($1, $2, $3)
            ON CONFLICT DO NOTHING
        )";

        db->execute_params(sql, {userid, course_id, section_id});
        std::print("教师 {} 已分配教学任务: {} - {}\n",
                  username, course_id, section_id);
        return true;

    } catch (const std::exception& e) {
        std::print("分配教学任务失败: {}\n", e.what());
        return false;
    }
}

std::vector<std::pair<std::string, std::string>> Teacher::get_students_in_course(
    std::shared_ptr<DatabaseConnection> db,
    const std::string& course_id) const {

    std::vector<std::pair<std::string, std::string>> students;

    try {
        auto sql = R"(
            SELECT s.user_id, u.username
            FROM course_selections cs
            JOIN students s ON cs.student_id = s.student_id
            JOIN users u ON s.user_id = u.user_id
            WHERE cs.course_id = $1 AND cs.status = 'enrolled'
            ORDER BY u.username
        )";

        auto result = db->execute_params(sql, {course_id});

        for (int i = 0; i < result.row_count(); i++) {
            students.emplace_back(
                result.get_value(i, 0),
                result.get_value(i, 1)
            );
        }

    } catch (const std::exception& e) {
        std::print("获取课程学生列表失败: {}\n", e.what());
    }

    return students;
}

bool Teacher::input_grades(std::shared_ptr<DatabaseConnection> db,
                          const std::string& student_id,
                          const std::string& course_id,
                          const std::string& grade) {
    try {
        auto sql = R"(
            INSERT INTO course_grades (student_id, course_id, grade, recorded_by)
            VALUES ($1, $2, $3, $4)
            ON CONFLICT (student_id, course_id) DO UPDATE
            SET grade = EXCLUDED.grade,
                recorded_by = EXCLUDED.recorded_by,
                recorded_at = CURRENT_TIMESTAMP
        )";

        db->execute_params(sql, {student_id, course_id, grade, userid});
        std::print("成绩已录入: 学生 {} 课程 {} 成绩 {}\n",
                  student_id, course_id, grade);
        return true;

    } catch (const std::exception& e) {
        std::print("录入成绩失败: {}\n", e.what());
        return false;
    }
}

void Teacher::view_teaching_courses() {
    std::print("\n=== 教学课程 ===\n");
    if (teaching_sessions.empty()) {
        std::print("暂无教学任务\n");
    } else {
        for (const auto& session : teaching_sessions) {
            std::print("{} - {}\n",
                      session->course->coursename,
                      session->sectionid);
        }
    }
}

void Teacher::manage_students(const std::string& course_id) {
    std::print("{} 管理课程 {} 的学生\n", username, course_id);
}

bool Teacher::set_capacity(const std::string& section_id, int capacity) {
    auto it = std::find_if(teaching_sessions.begin(), teaching_sessions.end(),
        [&section_id](const auto& session) {
            return session->sectionid == section_id;
        });

    if (it != teaching_sessions.end()) {
        (*it)->maxCapacity = capacity;
        std::print("课程容量已设置: {} -> {}\n", section_id, capacity);
        return true;
    }
    std::print("未找到课程段: {}\n", section_id);
    return false;
}

void Teacher::show_dashboard() {
    std::print("\n=== 教师面板 ===\n");
    std::print("姓名: {}\n", username);
    std::print("工号: {}\n", employee_number);
    std::print("教学任务数: {}\n", teaching_sessions.size());
}

void Teacher::add_teaching_session(std::shared_ptr<CourseSelection> session) {
    teaching_sessions.push_back(session);
    std::print("已添加教学任务: {}\n", session->course->coursename);
}

std::vector<std::shared_ptr<Teacher>> Teacher::load_all_teachers(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<Teacher>> teachers;

    try {
        auto result = db->execute(R"(
            SELECT u.user_id, u.username, t.employee_number
            FROM users u
            JOIN teachers t ON u.user_id = t.user_id
            WHERE u.user_type = 'teacher'
            ORDER BY t.employee_number
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string user_id = result.get_value(i, 0);
            std::string username = result.get_value(i, 1);
            std::string employee_number = result.get_value(i, 2);

            auto teacher = std::make_shared<Teacher>(
                user_id, username, "", employee_number);

            teachers.push_back(teacher);
        }

    } catch (const std::exception& e) {
        std::print("加载所有教师失败: {}\n", e.what());
    }

    return teachers;
}
