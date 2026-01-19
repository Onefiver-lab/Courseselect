export module registrar:secretary;
import :course;
import :teacher;
import std;
using std::string;
using std::format;

export class Secretary {
public:
    Secretary(string id, string name);
    string info() const;
    bool hasId(string id) const;
    Course* createCourse(string courseId, string courseName, int credits);
    bool assignTeacherToCourse(Teacher* teacher, Course* course);
    string getId() const;
    string getName() const;

private:
    string m_id;
    string m_name;
};

Secretary::Secretary(string id, string name) : m_id(id), m_name(name) {}

string Secretary::info() const {
    return format("秘书ID: {}, 姓名: {}", m_id, m_name);
}

bool Secretary::hasId(string id) const { return m_id == id; }

Course* Secretary::createCourse(string courseId, string courseName, int credits) {
    return new Course(courseId, courseName, credits);
}

bool Secretary::assignTeacherToCourse(Teacher* teacher, Course* course) {
    if (!teacher || !course) return false;
    course->setTeacher(teacher);
    return true;
}

string Secretary::getId() const { return m_id; }
string Secretary::getName() const { return m_name; }
