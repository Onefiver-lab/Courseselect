export module teacher;

import user;
import courseselection;
import <vector>;
import <memory>;
import <string>;
import <print>;
import <algorithm>;

export class Teacher : public User {
public:
    std::string employee_number;
    std::vector<std::shared_ptr<CourseSelection>> teaching_sessions;

public:
    Teacher(std::string user_id, std::string user_name, std::string password, std::string emp_num);

    void view_teaching_courses();
    void manage_students(const std::string& course_id);
    bool input_grades(const std::string& student_id, const std::string& course_id,
                     const std::string& grade);
    bool set_capacity(const std::string& section_id, int capacity);
    void show_dashboard() override;
    void add_teaching_session(std::shared_ptr<CourseSelection> session);
};


Teacher::Teacher(std::string user_id, std::string user_name, std::string password, std::string emp_num)
    : User(std::move(user_id), std::move(user_name), std::move(password))
    , employee_number(std::move(emp_num)) {}

void Teacher::view_teaching_courses() {
    std::print("\n=== Teaching Courses ===\n");
    for (const auto& session : teaching_sessions) {
        std::print("{} ({})\n", session->course->title, session->section_code);
    }
}

void Teacher::manage_students(const std::string& course_id) {
    std::print("{} managing students for course: {}\n", name, course_id);
}

bool Teacher::input_grades(const std::string& student_id, const std::string& course_id,
                          const std::string& grade) {
    std::print("Grade {} recorded for student {} in course {}\n", grade, student_id, course_id);
    return true;
}

bool Teacher::set_capacity(const std::string& section_id, int capacity) {
    auto it = std::find_if(teaching_sessions.begin(), teaching_sessions.end(),
        [&section_id](const auto& session) {
            return session->section_code == section_id;
        });

    if (it != teaching_sessions.end()) {
        (*it)->max_students = capacity;
        std::print("Capacity set to {} for section {}\n", capacity, section_id);
        return true;
    }
    return false;
}

void Teacher::show_dashboard() {
    std::print("\n=== Teacher Dashboard ===\n");
    std::print("Teacher: {}\n", name);
    std::print("Employee Number: {}\n", employee_number);
    std::print("Teaching Sessions: {}\n", teaching_sessions.size());
}

void Teacher::add_teaching_session(std::shared_ptr<CourseSelection> session) {
    teaching_sessions.push_back(session);
    std::print("{} added to teach {}\n", name, session->course->title);
}
