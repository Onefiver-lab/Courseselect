-- 插入测试数据
\c course_selection course_user;

-- 清空现有数据（可选，仅用于测试）
TRUNCATE TABLE enrollment, course, teacher, student, secretary RESTART IDENTITY CASCADE;

-- 插入学生
INSERT INTO student (id, name) VALUES 
('2024001', '张三'),
('2024002', '李四'),
('2024003', '王五')
ON CONFLICT (id) DO NOTHING;

-- 插入教师
INSERT INTO teacher (id, name) VALUES 
('T001', '王老师'),
('T002', '李老师'),
('T003', '张老师')
ON CONFLICT (id) DO NOTHING;

-- 插入秘书
INSERT INTO secretary (id, name) VALUES 
('S001', '赵秘书'),
('S002', '钱秘书')
ON CONFLICT (id) DO NOTHING;

-- 插入课程
INSERT INTO course (id, name, credits, teacher_id) VALUES 
('C001', 'C++程序设计', 3, 'T001'),
('C002', '数据库系统', 4, 'T002'),
('C003', '操作系统', 3, 'T003'),
('C004', '计算机网络', 4, 'T001')
ON CONFLICT (id) DO NOTHING;

-- 插入选课记录
INSERT INTO enrollment (student_id, course_id, grade) VALUES 
('2024001', 'C001', 'A'),
('2024001', 'C002', 'B'),
('2024002', 'C001', '85'),
('2024002', 'C003', NULL),
('2024003', 'C004', '92')
ON CONFLICT (student_id, course_id) DO UPDATE SET grade = EXCLUDED.grade;

-- 验证数据
SELECT '=== 数据统计 ===' as info;
SELECT '学生数量:', COUNT(*) FROM student;
SELECT '教师数量:', COUNT(*) FROM teacher;
SELECT '课程数量:', COUNT(*) FROM course;
SELECT '选课记录:', COUNT(*) FROM enrollment;

SELECT '=== 学生选课详情 ===' as info;
SELECT s.name as 学生, c.name as 课程, e.grade as 成绩, t.name as 教师
FROM student s
JOIN enrollment e ON s.id = e.student_id
JOIN course c ON e.course_id = c.id
LEFT JOIN teacher t ON c.teacher_id = t.id
ORDER BY s.id, c.name;
