export module teachingsecretary;

import std;
import user;
import iregistrar;
import semester;
import course;
import <memory>;
import <vector>;
import <print>;

export class TeachingSecretary : public User, public IRegistrar {
public:
    std::string employee_id;
    std::vector<std::shared_ptr<Semester>> managed_semesters;

public:
    TeachingSecretary(std::string user_id, std::string user_name,
                     std::string password, std::string emp_id);

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

    void show_dashboard() override;

private:
    void create_semester_ui();
    void view_all_semesters();
    void activate_semester_ui();
    void deactivate_semester_ui();
};
TeachingSecretary::TeachingSecretary(std::string user_id, std::string user_name,
                                   std::string password, std::string emp_id)
    : User(std::move(user_id), std::move(user_name), std::move(password))
    , employee_id(std::move(emp_id)) {}

void TeachingSecretary::manage_semesters() {
    int choice;
    do {
        std::print("\n=== Semester Management ===\n");
        std::print("1. Create New Semester\n");
        std::print("2. View All Semesters\n");
        std::print("3. Activate Semester\n");
        std::print("4. Deactivate Semester\n");
        std::print("0. Back\n");
        std::print("Enter choice: ");
        std::cin >> choice;

        switch (choice) {
            case 1:
                create_semester_ui();
                break;
            case 2:
                view_all_semesters();
                break;
            case 3:
                activate_semester_ui();
                break;
            case 4:
                deactivate_semester_ui();
                break;
        }
    } while (choice != 0);
}

bool TeachingSecretary::create_semester(const std::string& semester_code,
                                      const std::string& semester_name,
                                      const std::string& start_date,
                                      const std::string& end_date) {
    auto now = std::chrono::system_clock::now();
    auto next_month = now + std::chrono::hours(24 * 30);
    auto semester = std::make_shared<Semester>(semester_code, semester_name, now, next_month);
    managed_semesters.push_back(semester);

    std::print("Semester created successfully!\n");
    std::print("Code: {}, Name: {}\n", semester_code, semester_name);
    return true;
}

bool TeachingSecretary::activate_semester(const std::string& semester_code) {
    for (auto& semester : managed_semesters) {
        if (semester->code == semester_code) {
            semester->status = "Active";
            std::print("Semester {} activated.\n", semester_code);
            return true;
        }
    }
    std::print("Semester not found!\n");
    return false;
}

bool TeachingSecretary::deactivate_semester(const std::string& semester_code) {
    for (auto& semester : managed_semesters) {
        if (semester->code == semester_code) {
            semester->status = "Inactive";
            std::print("Semester {} deactivated.\n", semester_code);
            return true;
        }
    }
    std::print("Semester not found!\n");
    return false;
}

void TeachingSecretary::manage_all_courses() {
    std::print("\n=== Course Management ===\n");
    std::print("This feature allows management of all courses in the system.\n");
}

void TeachingSecretary::manage_all_users() {
    std::print("\n=== User Management ===\n");
    std::print("This feature allows management of all users in the system.\n");
}

void TeachingSecretary::process_approvals() {
    std::print("\n=== Approval Processing ===\n");
    std::print("Processing pending approvals...\n");
}

void TeachingSecretary::generate_reports() {
    std::print("\n=== Report Generation ===\n");
    std::print("1. Enrollment Statistics\n");
    std::print("2. Course Utilization\n");
    std::print("3. Faculty Workload\n");
    std::print("Choice: ");

    int choice;
    std::cin >> choice;
    std::print("Generating report...\n");
}

void TeachingSecretary::show_dashboard() {
    std::print("\n=== Teaching Secretary Dashboard ===\n");
    std::print("Welcome, {}!\n", name);
    std::print("Employee ID: {}\n", employee_id);

    manage_semesters();
}

void TeachingSecretary::create_semester_ui() {
    std::string code, name, start, end;
    std::print("Semester Code: "); std::cin >> code;
    std::print("Semester Name: "); std::cin >> name;
    std::print("Start Date (YYYY-MM-DD): "); std::cin >> start;
    std::print("End Date (YYYY-MM-DD): "); std::cin >> end;
    create_semester(code, name, start, end);
}

void TeachingSecretary::view_all_semesters() {
    std::print("\n=== All Semesters ===\n");
    for (const auto& semester : managed_semesters) {
        std::print("{} - {} ({})\n", semester->code, semester->title, semester->status);
    }
}

void TeachingSecretary::activate_semester_ui() {
    std::string code;
    std::print("Enter semester code to activate: ");
    std::cin >> code;
    activate_semester(code);
}

void TeachingSecretary::deactivate_semester_ui() {
    std::string code;
    std::print("Enter semester code to deactivate: ");
    std::cin >> code;
    deactivate_semester(code);
}
