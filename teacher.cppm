export module registrar:teacher;
import :course;
import :student;
import std;
using std::string;
using std::vector;
using std::format;
using std::find;

export class Teacher {
public:
    Teacher(string id, string name);
    string info() const;
    bool hasId(string id) const;
    void addCourse(Course* course);
    bool gradeStudent(Course* course, Student* student, string grade);
    vector<Course*> getCourses() const;
    string getName() const;
    string getId() const;

private:
    string m_id;
    string m_name;
    vector<Course*> m_courses;
};

Teacher::Teacher(string id, string name) : m_id(id), m_name(name) {}

string Teacher::info() const {
    return format("教师ID: {}, 姓名: {}", m_id, m_name);
}

bool Teacher::hasId(string id) const { return m_id == id; }
void Teacher::addCourse(Course* course) { m_courses.push_back(course); }

bool Teacher::gradeStudent(Course* course, Student* student, string grade) {
    if (course->getTeacher() != this) return false;
    auto& students = course->getStudents();
    if (find(students.begin(), students.end(), student) == students.end()) return false;
    course->assignGrade(student, grade);
    return true;
}

vector<Course*> Teacher::getCourses() const { return m_courses; }
string Teacher::getName() const { return m_name; }
string Teacher::getId() const { return m_id; }
