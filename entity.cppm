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
    BaseEntity(string id, string name);
    virtual ~BaseEntity() = default;

    string getId() const;
    string getName() const;
    bool hasId(const string& id) const;
    virtual string info() const = 0; // 纯虚函数，强制子类实现

private:
    string m_id;
    string m_name;
};


export class User : public BaseEntity {
public:
    // 构造函数
    User(string id, string name);
    // 析构函数
    virtual ~User() = default;
};


export class Course;
export class Student;
export class Teacher;
export class Secretary;


export using SharedStudent = shared_ptr<Student>;
export using SharedTeacher = shared_ptr<Teacher>;
export using SharedCourse = shared_ptr<Course>;
export using SharedSecretary = shared_ptr<Secretary>;
export using StudentList = vector<SharedStudent>;
export using CourseList = vector<SharedCourse>;


export class Student : public User, public enable_shared_from_this<Student> {
public:
    Student(string id, string name);

    string info() const override;

    void enrollIn(SharedCourse course);

    string schedule();


    const CourseList& getCourses() const;

private:
    CourseList _courses; // 成员变量是课程列表，对应getCourses()
};


export class Teacher : public User, public enable_shared_from_this<Teacher> {
public:
    Teacher(string id, string name);

    string info() const override;

    void addCourse(SharedCourse course);

    bool gradeStudent(SharedCourse course, SharedStudent student, string grade); // 延后实现

    CourseList getCourses() const;

private:
    CourseList m_courses;
};


export class Course : public BaseEntity, public enable_shared_from_this<Course> {
public:
    Course(string id, string name, int credits);

    string info() const override;

    void addStudent(SharedStudent student);

    void setTeacher(SharedTeacher teacher);

    bool assignGrade(SharedStudent student, string grade);

    string getGrade(SharedStudent student) const;

    SharedTeacher getTeacher() const;
    const StudentList& getStudents() const;

    int getCredits() const;

private:
    int m_credits;
    SharedTeacher m_teacher{nullptr};
    StudentList m_students;
    map<SharedStudent, string> m_grades;


    bool isValidGrade(const string& grade);
};


export class Secretary : public User, public enable_shared_from_this<Secretary> {
public:
    Secretary(string id, string name);

    string info() const override;

    SharedCourse createCourse(string courseId, string courseName, int credits);

    bool assignTeacherToCourse(SharedTeacher teacher, SharedCourse course);
};

// -------------------------- 类实现部分 --------------------------
// BaseEntity 实现
BaseEntity::BaseEntity(string id, string name) : m_id(std::move(id)), m_name(std::move(name)) {}
string BaseEntity::getId() const { return m_id; }
string BaseEntity::getName() const { return m_name; }
bool BaseEntity::hasId(const string& id) const { return m_id == id; }

// User 实现
User::User(string id, string name) : BaseEntity(std::move(id), std::move(name)) {}

// Student 实现
Student::Student(string id, string name) : User(std::move(id), std::move(name)) {}

string Student::info() const {
    return format("学生ID: {}, 姓名: {}\n", getId(), getName());
}

void Student::enrollIn(SharedCourse course) {
    auto it = find(_courses.begin(), _courses.end(), course);
    if (it != _courses.end()) return;
    _courses.push_back(course);
}

const CourseList& Student::getCourses() const { return _courses; }



inline string Student::schedule() {
    string scheduleStr;
    for (const auto& course : _courses) {
        scheduleStr += course->info();
    }
    return scheduleStr.empty() ? "暂无选课\n" : scheduleStr;
}

// Teacher 实现
Teacher::Teacher(string id, string name) : User(std::move(id), std::move(name)) {}

string Teacher::info() const  {
    return format("教师ID: {}, 姓名: {}", getId(), getName());
}

void Teacher::addCourse(SharedCourse course) {
    auto it = find(m_courses.begin(), m_courses.end(), course);
    if (it != m_courses.end()) return;
    m_courses.push_back(course);
}

CourseList Teacher::getCourses() const { return m_courses; }


inline bool Teacher::gradeStudent(SharedCourse course, SharedStudent student, string grade) {
    if (course->getTeacher() != shared_from_this()) return false;
    auto& students = course->getStudents();
    if (find(students.begin(), students.end(), student) == students.end()) return false;
    return course->assignGrade(student, std::move(grade));
}

// Course 实现
Course::Course(string id, string name, int credits)
    : BaseEntity(std::move(id), std::move(name)), m_credits(credits) {
    if (credits < 1 || credits > 10) {
        throw invalid_argument("学分必须在1-10之间");
    }
}

string Course::info() const {
    string teacherInfo = m_teacher ? m_teacher->info() : "无授课教师";
    return format("课程ID: {}, 名称: {}, 学分: {}, 授课教师: {}\n",
                  getId(), getName(), m_credits, teacherInfo);
}

void Course::addStudent(SharedStudent student) {
    auto it = find(m_students.begin(), m_students.end(), student);
    if (it != m_students.end()) return;
    m_students.push_back(student);
    student->enrollIn(shared_from_this());
}

void Course::setTeacher(SharedTeacher teacher) {
    if (!teacher) return;
    m_teacher = teacher;
    teacher->addCourse(shared_from_this());
}

bool Course::assignGrade(SharedStudent student, string grade) {
    if (!isValidGrade(grade)) return false;
    auto it = find(m_students.begin(), m_students.end(), student);
    if (it == m_students.end()) return false;
    m_grades[student] = std::move(grade);
    return true;
}

string Course::getGrade(SharedStudent student) const {
    auto it = m_grades.find(student);
    return it != m_grades.end() ? it->second : "未评定";
}

SharedTeacher Course::getTeacher() const { return m_teacher; }
const StudentList& Course::getStudents() const { return m_students; }
int Course::getCredits() const { return m_credits; }

bool Course::isValidGrade(const string& grade) {
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

// Secretary 实现
Secretary::Secretary(string id, string name) : User(std::move(id), std::move(name)) {}

string Secretary::info() const {
    return format("秘书ID: {}, 姓名: {}", getId(), getName());
}

SharedCourse Secretary::createCourse(string courseId, string courseName, int credits) {
    return shared_ptr<Course>(new Course(std::move(courseId), std::move(courseName), credits));
}

bool Secretary::assignTeacherToCourse(SharedTeacher teacher, SharedCourse course) {
    if (!teacher || !course) return false;
    course->setTeacher(teacher);
    return true;
}


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
