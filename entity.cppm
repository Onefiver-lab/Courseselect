// entity.ixx（统一存放所有实体类的完整定义，彻底解决不完整类型和循环依赖）
export module registrar:entity;
import std;

// 必须引入的标准库模板（部分编译器对std模块的自动导入支持不全）
using std::string;
using std::vector;
using std::map;
using std::find;
using std::format;
using std::shared_ptr;
using std::enable_shared_from_this;
using std::invalid_argument;
using std::stoi;

// 基础实体基类（所有业务实体的公共父类）
export class BaseEntity {
public:
    BaseEntity(string id, string name) : m_id(std::move(id)), m_name(std::move(name)) {}
    virtual ~BaseEntity() = default;

    string getId() const { return m_id; }
    string getName() const { return m_name; }
    bool hasId(const string& id) const { return m_id == id; }
    virtual string info() const = 0; // 纯虚函数，强制子类实现

private:
    string m_id;
    string m_name;
};

// ===================== 泛化类 User =====================
// 【泛化设计】User 作为 Student/Teacher/Secretary 的泛化基类
// 仅用于设计层面的泛化说明，无实际业务逻辑扩展，不对外提供额外接口
// 所有子类的核心逻辑仍保持独立，仅继承该类标识泛化关系
export class User : public BaseEntity {
public:
    // 构造函数透传 BaseEntity 的 id/name，保持与原有逻辑一致
    User(string id, string name) : BaseEntity(std::move(id), std::move(name)) {}
    // 析构函数默认，不新增任何虚函数或成员变量
    virtual ~User() = default;

    // 无额外接口，完全复用 BaseEntity 的能力，仅作为泛化标识
};

// 提前声明类（解决类内部互相引用的问题）
export class Course;
export class Student;
export class Teacher;
export class Secretary;

// 共享智能指针类型别名
export using SharedStudent = shared_ptr<Student>;
export using SharedTeacher = shared_ptr<Teacher>;
export using SharedCourse = shared_ptr<Course>;
export using SharedSecretary = shared_ptr<Secretary>;
export using StudentList = vector<SharedStudent>;
export using CourseList = vector<SharedCourse>;

// 【关键修正1：调整类定义顺序】
// 先定义无依赖的类，再定义有交叉依赖的类，避免"未完整定义"报错
// 1. 学生类（依赖Course，后定义Course，需确保调用时Course已完整）
// 学生类（修正后，无类型不匹配问题）
// 【核心修改】继承泛化类 User（而非直接继承 BaseEntity），接口/实现完全不变
export class Student : public User, public enable_shared_from_this<Student> {
public:
    Student(string id, string name) : User(std::move(id), std::move(name)) {}

    string info() const override {
        return format("学生ID: {}, 姓名: {}\n", getId(), getName());
    }

    void enrollIn(SharedCourse course) {
        auto it = find(_courses.begin(), _courses.end(), course);
        if (it != _courses.end()) return;
        _courses.push_back(course);
    }

    string schedule(); // 仅声明，延后实现

    // 【修正：命名正确+类型匹配，返回课程列表（CourseList）的const引用】
    const CourseList& getCourses() const { return _courses; }

private:
    CourseList _courses; // 成员变量是课程列表，对应getCourses()
};

// 2. 教师类（依赖Course，同理，复杂函数延后实现）
// 【核心修改】继承泛化类 User（而非直接继承 BaseEntity），接口/实现完全不变
export class Teacher : public User, public enable_shared_from_this<Teacher> {
public:
    Teacher(string id, string name) : User(std::move(id), std::move(name)) {}

    string info() const override {
        return format("教师ID: {}, 姓名: {}", getId(), getName());
    }

    void addCourse(SharedCourse course) {
        auto it = find(m_courses.begin(), m_courses.end(), course);
        if (it != m_courses.end()) return;
        m_courses.push_back(course);
    }

    bool gradeStudent(SharedCourse course, SharedStudent student, string grade); // 延后实现

    CourseList getCourses() const { return m_courses; }

private:
    CourseList m_courses;
};

// 3. 课程类（被Student/Teacher依赖，优先完整定义，无前置依赖）
export class Course : public BaseEntity, public enable_shared_from_this<Course> {
public:
    Course(string id, string name, int credits)
        : BaseEntity(std::move(id), std::move(name)), m_credits(credits) {
        if (credits < 1 || credits > 10) {
            throw invalid_argument("学分必须在1-10之间");
        }
    }

    string info() const override {
        string teacherInfo = m_teacher ? m_teacher->info() : "无授课教师";
        return format("课程ID: {}, 名称: {}, 学分: {}, 授课教师: {}\n",
                      getId(), getName(), m_credits, teacherInfo);
    }

    void addStudent(SharedStudent student) {
        auto it = find(m_students.begin(), m_students.end(), student);
        if (it != m_students.end()) return;
        m_students.push_back(student);
        student->enrollIn(shared_from_this());
    }

    void setTeacher(SharedTeacher teacher) {
        if (!teacher) return;
        m_teacher = teacher;
        teacher->addCourse(shared_from_this());
    }

    bool assignGrade(SharedStudent student, string grade) {
        if (!isValidGrade(grade)) return false;
        auto it = find(m_students.begin(), m_students.end(), student);
        if (it == m_students.end()) return false;
        m_grades[student] = std::move(grade);
        return true;
    }

    string getGrade(SharedStudent student) const {
        auto it = m_grades.find(student);
        return it != m_grades.end() ? it->second : "未评定";
    }

    SharedTeacher getTeacher() const { return m_teacher; }
    const StudentList& getStudents() const { return m_students; }

    int getCredits() const { return m_credits; }

private:
    int m_credits;
    SharedTeacher m_teacher{nullptr};
    StudentList m_students;
    map<SharedStudent, string> m_grades;

    // 内部调用成绩校验函数
    bool isValidGrade(const string& grade) {
        if (grade == "A" || grade == "B" || grade == "C" || grade == "D" || grade == "F") {
            return true;
        }
        try {
            int score = stoi(grade);
            return score >= 0 && score <= 100;
        } catch (...) {
            return false;
        }
    }
};

// 4. 秘书类（依赖Course/Teacher，已完整定义，可正常实现）
// 【核心修改】继承泛化类 User（而非直接继承 BaseEntity），接口/实现完全不变
export class Secretary : public User, public enable_shared_from_this<Secretary> {
public:
    Secretary(string id, string name) : User(std::move(id), std::move(name)) {}

    string info() const override {
        return format("秘书ID: {}, 姓名: {}", getId(), getName());
    }

    SharedCourse createCourse(string courseId, string courseName, int credits) {
        return shared_ptr<Course>(new Course(std::move(courseId), std::move(courseName), credits));
    }

    bool assignTeacherToCourse(SharedTeacher teacher, SharedCourse course) {
        if (!teacher || !course) return false;
        course->setTeacher(teacher);
        return true;
    }
};

// 【关键修正3：延后实现有交叉依赖的成员函数】
// 此时Course已完整定义，Student::schedule()可正常调用Course::info()
inline string Student::schedule() {
    string scheduleStr;
    for (const auto& course : _courses) {
        scheduleStr += course->info(); // Course已完整定义，无报错
    }
    return scheduleStr.empty() ? "暂无选课\n" : scheduleStr;
}

// 延后实现Teacher::gradeStudent()，此时Course已完整定义
inline bool Teacher::gradeStudent(SharedCourse course, SharedStudent student, string grade) {
    if (course->getTeacher() != shared_from_this()) return false;
    auto& students = course->getStudents();
    if (find(students.begin(), students.end(), student) == students.end()) return false;
    return course->assignGrade(student, std::move(grade));
}

// 全局共享成绩校验工具函数（可选，供外部调用）
export bool isValidGrade(const string& grade) {
    if (grade == "A" || grade == "B" || grade == "C" || grade == "D" || grade == "F") {
        return true;
    }
    try {
        int score = stoi(grade);
        return score >= 0 && score <= 100;
    } catch (...) {
        return false;
    }
}
