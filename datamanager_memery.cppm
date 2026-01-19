// Module
// File: datamanager_memory.cppm   Version: 0.1.0   License: AGPLv3
// Created:Wang Qingming,Huang Yulin,Li Lin       2026-01-19 11:04:57
// Description:
//
export module registrar:datamanager_memory;
import :datamanager_interface;
import :student;
import :course;
import :teacher;
import :secretary;
import std;
using std::string;
using std::vector;
using std::find_if;

export class DataManagerMemory : public DataManager {
public:
    // 学生操作
    void addStudent(Student* student) override {
        if (!getStudentById(student->getId())) {
            m_students.push_back(student);
        }
    }

    Student* getStudentById(string id) override {
        auto it = find_if(m_students.begin(), m_students.end(),
            [&id](Student* s) { return s->hasId(id); });
        return it != m_students.end() ? *it : nullptr;
    }

    vector<Student*> getAllStudents() override { return m_students; }

    // 课程操作
    void addCourse(Course* course) override {
        if (!getCourseById(course->getId())) {
            m_courses.push_back(course);
        }
    }

    Course* getCourseById(string id) override {
        auto it = find_if(m_courses.begin(), m_courses.end(),
            [&id](Course* c) { return c->hasId(id); });
        return it != m_courses.end() ? *it : nullptr;
    }

    vector<Course*> getAllCourses() override { return m_courses; }

    // 教师操作
    void addTeacher(Teacher* teacher) override {
        if (!getTeacherById(teacher->getId())) {
            m_teachers.push_back(teacher);
        }
    }

    Teacher* getTeacherById(string id) override {
        auto it = find_if(m_teachers.begin(), m_teachers.end(),
            [&id](Teacher* t) { return t->hasId(id); });
        return it != m_teachers.end() ? *it : nullptr;
    }

    vector<Teacher*> getAllTeachers() override { return m_teachers; }

    // 秘书操作
    void addSecretary(Secretary* secretary) override {
        if (!getSecretaryById(secretary->getId())) {
            m_secretaries.push_back(secretary);
        }
    }

    Secretary* getSecretaryById(string id) override {
        auto it = find_if(m_secretaries.begin(), m_secretaries.end(),
            [&id](Secretary* s) { return s->hasId(id); });
        return it != m_secretaries.end() ? *it : nullptr;
    }

    // 数据持久化（内存版无实际操作，返回true）
    bool saveData() override { return true; }

private:
    vector<Student*> m_students;
    vector<Course*> m_courses;
    vector<Teacher*> m_teachers;
    vector<Secretary*> m_secretaries;
};
