export module registrar:course;

import std;
using std::string;
using std::vector;
using std::format;
using std::map;

export class Course {
public:
    Course(string id, string name, int credits);
    string info() const;
    void addStudent(class Student* student);
    void setTeacher(class Teacher* teacher);
    void assignGrade(class Student* student, string grade);
    string getGrade(Student* student) const;
    bool hasId(string id) const;
    Teacher* getTeacher() const;
    vector<Student*> getStudents() const;
    string getId() const;
    string getName() const;
    int getCredits() const;

private:
    string m_id;
    string m_name;
    int m_credits;
    Teacher* m_teacher{nullptr};
    vector<Student*> m_students;
    map<Student*, string> m_grades; // 学生-成绩映射
};

Course::Course(string id, string name, int credits)
    : m_id(id), m_name(name), m_credits(credits) {}

string Course::info() const {
    string teacherInfo = m_teacher ? m_teacher->info() : "无授课教师";
    return format("课程ID: {}, 名称: {}, 学分: {}, 授课教师: {}\n",
                  m_id, m_name, m_credits, teacherInfo);
}

void Course::addStudent(Student* student) {
    m_students.push_back(student);
    student->enrollIn(this);
}

void Course::setTeacher(Teacher* teacher) {
    m_teacher = teacher;
    teacher->addCourse(this);
}

void Course::assignGrade(Student* student, string grade) {
    m_grades[student] = grade;
}

string Course::getGrade(Student* student) const {
    auto it = m_grades.find(student);
    return it != m_grades.end() ? it->second : "未评定";
}

bool Course::hasId(string id) const { return m_id == id; }
Teacher* Course::getTeacher() const { return m_teacher; }
vector<Student*> Course::getStudents() const { return m_students; }
string Course::getId() const { return m_id; }
string Course::getName() const { return m_name; }
int Course::getCredits() const { return m_credits; }
