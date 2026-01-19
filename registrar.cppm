export module registrar;
export import :entity; // 导出公共模块，供main使用

export import :datamanager_interface;
export import :datamanager_memory;
import std;

using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::find;
using std::format;
using std::make_shared;
using std::numeric_limits;
using std::streamsize;
using std::invalid_argument;
using std::print;

// 工具函数：安全读取整数
int readInt(const string& prompt);

export class Registrar {
public:
    Registrar();
    int exec();

private:
    std::shared_ptr<DataManager> m_dataManager;
    void initDefaultUsers();
    void studentMenu();
    void teacherMenu();
    void secretaryMenu();
    void showAllCourses();
    void enrollCourse(SharedStudent student);
    void showStudentGrades(SharedStudent student);
    void showTeacherCourses(SharedTeacher teacher);
    void inputStudentGrades(SharedTeacher teacher);
    void createCourseBySecretary(SharedSecretary secretary);
    void assignTeacherBySecretary(SharedSecretary secretary);
    void showAllTeachers();
};

// -------------------------- 工具函数与类实现部分 --------------------------
// 工具函数：安全读取整数
int readInt(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        } else {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            print("输入无效，请输入数字！");
        }
    }
}

// Registrar 构造函数
Registrar::Registrar() {
    try {
#ifdef USE_POSTGRES
            string connStr = "user=postgres password=123456 dbname=course_selection host=localhost port=5432";
            m_dataManager = make_shared<DataManagerPostgres>(connStr);
#else
            m_dataManager = make_shared<DataManagerMemory>();
#endif
            initDefaultUsers();
        } catch (const std::exception& e) {
            print("系统初始化失败：",e.what());
            std::exit(1);
        }
}

// 系统主循环
int Registrar::exec() {
    print("===== 选课系统 =====");
    while (true) {
        int choice = readInt("\n1.学生 2.教师 3.教学秘书 0.退出\n请选择：");
        switch (choice) {
            case 1: studentMenu(); break;
            case 2: teacherMenu(); break;
            case 3: secretaryMenu(); break;
            case 0: print("再见！"); return 0;
            default: print("无效选项！");
        }
    }
}

void Registrar::initDefaultUsers() {
    try {
        // 学生
        if (!m_dataManager->getStudentById("2024001")) {
            m_dataManager->addStudent(make_shared<Student>("2024001", "张三"));
            m_dataManager->addStudent(make_shared<Student>("2024002", "李四"));
            m_dataManager->addStudent(make_shared<Student>("2024003", "王五"));
            m_dataManager->addStudent(make_shared<Student>("2024004", "赵六"));
            m_dataManager->addStudent(make_shared<Student>("2024005", "肥七"));
        }

        // 教师
        if (!m_dataManager->getTeacherById("T001")) {
            m_dataManager->addTeacher(make_shared<Teacher>("T001", "王老师"));
            m_dataManager->addTeacher(make_shared<Teacher>("T002", "李老师"));
            m_dataManager->addTeacher(make_shared<Teacher>("T002", "陈老师"));
            m_dataManager->addTeacher(make_shared<Teacher>("T002", "鲁老师"));
        }

        // 秘书
        if (!m_dataManager->getSecretaryById("S001")) {
            m_dataManager->addSecretary(make_shared<Secretary>("S001", "赵秘书"));
        }

        // 课程
        if (!m_dataManager->getCourseById("C001")) {
            auto sec = m_dataManager->getSecretaryById("S001");
            auto tea = m_dataManager->getTeacherById("T001");
            auto course = sec->createCourse("C001", "C++程序设计", 3);
            sec->assignTeacherToCourse(tea, course);
            m_dataManager->addCourse(course);
        }

        if (!m_dataManager->saveData()) {
            print("默认数据保存失败！");
        }
    } catch (const std::exception& e) {
        print("默认用户初始化失败：",e.what());
    }
}

// 学生菜单
void Registrar::studentMenu() {
    print("\n===== 学生登录 =====\n");
    cout << "输入学生ID：";
    string id;
    getline(cin, id);
    auto stu = m_dataManager->getStudentById(id);
    if (!stu) { print("ID不存在！"); return; }

    while (true) {
        int choice = readInt("\n1.查课程 2.选课 3.课表 4.成绩 0.返回\n选择：");
        switch (choice) {
            case 1: showAllCourses(); break;
            case 2: enrollCourse(stu); break;
            case 3: cout << "个人课表：\n" << stu->schedule(); break;
            case 4: showStudentGrades(stu); break;
            case 0: return;
            default: print("无效选项！");
        }
    }
}

// 教师菜单
void Registrar::teacherMenu() {
    print("\n===== 教师登录 =====\n");
    cout << "输入教师ID：";
    string id;
    getline(cin, id);
    auto tea = m_dataManager->getTeacherById(id);
    if (!tea) { print("ID不存在！"); return; }

    while (true) {
        int choice = readInt("\n1.授课课程 2.录成绩 0.返回\n选择：");
        switch (choice) {
            case 1: showTeacherCourses(tea); break;
            case 2: inputStudentGrades(tea); break;
            case 0: return;
            default: print("无效选项！");
        }
    }
}

// 秘书菜单
void Registrar::secretaryMenu() {
    print("\n===== 秘书登录 =====\n");
    cout << "输入秘书ID：";
    string id;
    getline(cin, id);
    auto sec = m_dataManager->getSecretaryById(id);
    if (!sec) { print("ID不存在！"); return; }

    while (true) {
        int choice = readInt("\n1.创建课程 2.分配教师 3.查课程 0.返回\n选择：");
        switch (choice) {
            case 1: createCourseBySecretary(sec); break;
            case 2: assignTeacherBySecretary(sec); break;
            case 3: showAllCourses(); break;
            case 0: return;
            default: print("无效选项！");
        }
    }
}

// 显示所有课程
void Registrar::showAllCourses() {
    print("\n===== 所有课程 =====\n");
    auto courses = m_dataManager->getAllCourses();
    if (courses.empty()) { print("暂无课程！"); return; }
    for (const auto& c : courses) cout << c->info();
}

// 学生选课
void Registrar::enrollCourse(SharedStudent stu) {
    showAllCourses();
    cout << "输入选课ID：";
    string cid;
    getline(cin, cid);
    auto course = m_dataManager->getCourseById(cid);
    if (!course) { print("课程不存在！"); return; }

    try {
        course->addStudent(stu);
        m_dataManager->addCourse(course);
        if (m_dataManager->saveData()) {
            print("选课成功！");
        } else {
            print("选课成功，但数据保存失败！");
        }
    } catch (const std::exception& e) {
        print("选课失败：",e.what());
    }
}

// 显示学生成绩
void Registrar::showStudentGrades(SharedStudent stu) {
    print("\n===== 个人成绩 =====\n");
    auto courses = m_dataManager->getAllCourses();
    bool hasGrade = false;
    for (const auto& c : courses) {
        auto& stus = c->getStudents();
        if (find(stus.begin(), stus.end(), stu) != stus.end()) {
            print("{}: 成绩{}", c->getName(), c->getGrade(stu));
            hasGrade = true;
        }
    }
    if (!hasGrade) print("暂无成绩！");
}

// 显示教师授课课程
void Registrar::showTeacherCourses(SharedTeacher tea) {
    print("\n===== 授课课程 ====\n");
    auto courses = m_dataManager->getAllCourses();
    bool hasCourse = false;
    for (const auto& c : courses) {
        if (c->getTeacher() == tea) {
            cout << c->info();
            hasCourse = true;
        }
    }
    if (!hasCourse) print("暂无授课课程！");
}

// 教师录成绩
void Registrar::inputStudentGrades(SharedTeacher tea) {
    auto courses = m_dataManager->getAllCourses();
    vector<SharedCourse> myCourses;
    for (const auto& c : courses) {
        if (c->getTeacher() == tea) myCourses.push_back(c);
    }
    if (myCourses.empty()) { print("无授课课程！\n"); return; }

    print("授课课程：\n");
    for (std::size_t i = 0; i < myCourses.size(); i++)
        print("{}: {}", i + 1, myCourses[i]->getName());

    int idx = readInt("选择课程序号：\n");
    if (idx < 1 || idx > myCourses.size()) { print("无效序号！\n"); return; }
    auto course = myCourses[idx - 1];

    auto stus = course->getStudents();
    if (stus.empty()) { print("无学生选课！\n"); return; }

    print("选课学生：\n");
    for (std::size_t i = 0; i < stus.size(); i++)
        cout << format("{}: {}", i + 1, stus[i]->info());

    int stuIdx = readInt("选择学生序号：\n");
    if (stuIdx < 1 || stuIdx > stus.size()) { print("无效序号！\n"); return; }
    auto student = stus[stuIdx - 1];

    cout << "输入成绩（支持A-F或0-100）：";
    string grade;
    getline(cin, grade);

    if (course->assignGrade(student, grade)) {
        m_dataManager->addCourse(course);
        if (m_dataManager->saveData()) {
            print("成绩录入成功！\n");
        } else {
            print("成绩录入成功，但数据保存失败！\n");
        }
    } else {
        print("成绩格式无效（仅支持A-F或0-100）！\n");
    }
}

// 秘书创建课程
void Registrar::createCourseBySecretary(SharedSecretary secretary) {
    cout << "输入课程ID：";
    string cid;
    getline(cin, cid);
    if (m_dataManager->getCourseById(cid)) {
        print("课程ID已存在！\n");
        return;
    }

    cout << "输入课程名称：";
    string cname;
    getline(cin, cname);
    int credits = readInt("输入课程学分（1-10）：");

    try {
        auto course = secretary->createCourse(cid, cname, credits);
        m_dataManager->addCourse(course);
        if (m_dataManager->saveData()) {
            print("课程创建成功！\n");
        } else {
            print("课程创建成功，但数据保存失败！\n");
        }
    } catch (const invalid_argument& e) {
        print("课程创建失败：\n",e.what());
    }
}

// 秘书分配教师
void Registrar::assignTeacherBySecretary(SharedSecretary secretary) {
    showAllCourses();
    cout << "输入要分配教师的课程ID：";
    string cid;
    getline(cin, cid);
    auto course = m_dataManager->getCourseById(cid);
    if (!course) { print("课程不存在！\n"); return; }

    showAllTeachers();
    cout << "输入教师ID：";
    string tid;
    getline(cin, tid);
    auto teacher = m_dataManager->getTeacherById(tid);
    if (!teacher) { print("教师不存在！\n"); return; }

    if (secretary->assignTeacherToCourse(teacher, course)) {
        m_dataManager->addCourse(course);
        if (m_dataManager->saveData()) {
            print("教师分配成功！\n");
        } else {
            print("教师分配成功，但数据保存失败！\n");
        }
    } else {
        print("教师分配失败！\n");
    }
}

// 显示所有教师
void Registrar::showAllTeachers() {
    print("\n===== 所有教师 =====\n");
    auto teachers = m_dataManager->getAllTeachers();
    if (teachers.empty()) { print("暂无教师！\n"); return; }
    for (const auto& t : teachers) cout << t->info() <<endl;
}
