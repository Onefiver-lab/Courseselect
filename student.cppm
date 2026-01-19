export module registrar:student;
import :course;
import std;

using std::string;
using std::vector;
using std::format;



export class Student {
public:
    Student(string id, string name);
    void enrollIn(Course* course);
    string schedule();
    string info();
    bool hasId(string id);

private:
    string m_name;
    string m_id;
    vector<Course*> _courses;
};

Student::Student(string id, string name)
    : m_name(name)
    , m_id(id) {
}

string Student::info() {
    return format("{} {}\n", m_id, m_name);
}

bool Student::hasId(string id) {
    return id == m_id;
}

void Student::enrollIn(Course* course) {
    _courses.push_back(course);
}

string Student::schedule() {
    std::string scheduleStr;
    for (auto* course : _courses) {
        scheduleStr += course->info();
    }
    return scheduleStr.empty() ? "暂无选课\n" : scheduleStr;
}
