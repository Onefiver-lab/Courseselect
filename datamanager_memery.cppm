export module registrar:datamanager_memory;
import :datamanager_interface;
import :entity;
import std;

using std::find_if;

export class DataManagerMemory : public DataManager {
public:
    // 学生操作
    void addStudent(SharedStudent student) override;
    SharedStudent getStudentById(std::string id) override;
    StudentList getAllStudents() override;

    // 课程操作
    void addCourse(SharedCourse course) override;
    SharedCourse getCourseById(std::string id) override;
    CourseList getAllCourses() override;

    // 教师操作
    void addTeacher(SharedTeacher teacher) override;
    SharedTeacher getTeacherById(std::string id) override;
    std::vector<SharedTeacher> getAllTeachers() override;

    // 秘书操作
    void addSecretary(SharedSecretary secretary) override;
    SharedSecretary getSecretaryById(std::string id) override;

    // 数据持久化（JSON文件存储）
    bool saveData() override;

private:
    StudentList m_students;
    CourseList m_courses;
    std::vector<SharedTeacher> m_teachers;
    std::vector<SharedSecretary> m_secretaries;
};

// -------------------------- 类实现部分 --------------------------
// 学生操作
void DataManagerMemory::addStudent(SharedStudent student) {
    if (!getStudentById(student->getId())) {
        m_students.push_back(student);
    }
}

SharedStudent DataManagerMemory::getStudentById(std::string id) {
    auto it = find_if(m_students.begin(), m_students.end(),
        [&id](const SharedStudent& s) { return s->hasId(id); });
    return it != m_students.end() ? *it : nullptr;
}

StudentList DataManagerMemory::getAllStudents() { return m_students; }

// 课程操作
void DataManagerMemory::addCourse(SharedCourse course) {
    if (!getCourseById(course->getId())) {
        m_courses.push_back(course);
    }
}

SharedCourse DataManagerMemory::getCourseById(std::string id) {
    auto it = find_if(m_courses.begin(), m_courses.end(),
        [&id](const SharedCourse& c) { return c->hasId(id); });
    return it != m_courses.end() ? *it : nullptr;
}

CourseList DataManagerMemory::getAllCourses() { return m_courses; }

// 教师操作
void DataManagerMemory::addTeacher(SharedTeacher teacher) {
    if (!getTeacherById(teacher->getId())) {
        m_teachers.push_back(teacher);
    }
}

SharedTeacher DataManagerMemory::getTeacherById(std::string id) {
    auto it = find_if(m_teachers.begin(), m_teachers.end(),
        [&id](const SharedTeacher& t) { return t->hasId(id); });
    return it != m_teachers.end() ? *it : nullptr;
}

std::vector<SharedTeacher> DataManagerMemory::getAllTeachers() { return m_teachers; }

// 秘书操作
void DataManagerMemory::addSecretary(SharedSecretary secretary) {
    if (!getSecretaryById(secretary->getId())) {
        m_secretaries.push_back(secretary);
    }
}

SharedSecretary DataManagerMemory::getSecretaryById(std::string id) {
    auto it = find_if(m_secretaries.begin(), m_secretaries.end(),
        [&id](const SharedSecretary& s) { return s->hasId(id); });
    return it != m_secretaries.end() ? *it : nullptr;
}

// 数据持久化（JSON文件存储）
bool DataManagerMemory::saveData() {
    std::ofstream f("data.json");
    if (!f.is_open()) return false;

    f << "{" << std::endl;
    f << "  \"students\": [";
    for (std::size_t i = 0; i < m_students.size(); i++) {
        f << "{\"id\":\"" << m_students[i]->getId() << "\",\"name\":\"" << m_students[i]->getName() << "\"}";
        if (i != m_students.size() - 1) f << ",";
    }
    f << "]," << std::endl;

    f << "  \"courses\": [";
    for (std::size_t i = 0; i < m_courses.size(); i++) {
        f << "{\"id\":\"" << m_courses[i]->getId() << "\",\"name\":\"" << m_courses[i]->getName()
          << "\",\"credits\":" << m_courses[i]->getCredits() << "}";
        if (i != m_courses.size() - 1) f << ",";
    }
    f << "]" << std::endl;
    f << "}" << std::endl;

    f.close();
    return true;
}
