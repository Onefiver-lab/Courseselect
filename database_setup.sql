-- 数据库设置脚本 - 在psql中执行
\c course_selection course_user;

-- 启用UUID扩展
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- 1. 创建教师表
CREATE TABLE IF NOT EXISTS teacher (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 2. 创建学生表
CREATE TABLE IF NOT EXISTS student (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 3. 创建秘书表
CREATE TABLE IF NOT EXISTS secretary (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 4. 创建课程表
CREATE TABLE IF NOT EXISTS course (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    credits INTEGER NOT NULL CHECK (credits >= 1 AND credits <= 10),
    teacher_id VARCHAR(50) REFERENCES teacher(id) ON DELETE SET NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 5. 创建选课表
CREATE TABLE IF NOT EXISTS enrollment (
    id SERIAL PRIMARY KEY,
    student_id VARCHAR(50) NOT NULL REFERENCES student(id) ON DELETE CASCADE,
    course_id VARCHAR(50) NOT NULL REFERENCES course(id) ON DELETE CASCADE,
    grade VARCHAR(10),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(student_id, course_id),
    CONSTRAINT check_grade_format CHECK (
        grade IS NULL OR 
        grade IN ('A', 'B', 'C', 'D', 'F') OR
        (grade ~ '^[0-9]{1,3}$' AND grade::INTEGER >= 0 AND grade::INTEGER <= 100)
    )
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_course_teacher ON course(teacher_id);
CREATE INDEX IF NOT EXISTS idx_enrollment_student ON enrollment(student_id);
CREATE INDEX IF NOT EXISTS idx_enrollment_course ON enrollment(course_id);

-- 创建更新触发器函数
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

-- 为每个表创建触发器
DO $$ 
BEGIN
    -- 教师表
    IF NOT EXISTS (SELECT 1 FROM pg_trigger WHERE tgname = 'update_teacher_updated_at') THEN
        CREATE TRIGGER update_teacher_updated_at 
            BEFORE UPDATE ON teacher 
            FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    END IF;
    
    -- 学生表
    IF NOT EXISTS (SELECT 1 FROM pg_trigger WHERE tgname = 'update_student_updated_at') THEN
        CREATE TRIGGER update_student_updated_at 
            BEFORE UPDATE ON student 
            FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    END IF;
    
    -- 秘书表
    IF NOT EXISTS (SELECT 1 FROM pg_trigger WHERE tgname = 'update_secretary_updated_at') THEN
        CREATE TRIGGER update_secretary_updated_at 
            BEFORE UPDATE ON secretary 
            FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    END IF;
    
    -- 课程表
    IF NOT EXISTS (SELECT 1 FROM pg_trigger WHERE tgname = 'update_course_updated_at') THEN
        CREATE TRIGGER update_course_updated_at 
            BEFORE UPDATE ON course 
            FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    END IF;
    
    -- 选课表
    IF NOT EXISTS (SELECT 1 FROM pg_trigger WHERE tgname = 'update_enrollment_updated_at') THEN
        CREATE TRIGGER update_enrollment_updated_at 
            BEFORE UPDATE ON enrollment 
            FOR EACH ROW EXECUTE FUNCTION update_updated_at_column();
    END IF;
END $$;

-- 授予权限给course_user
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO course_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO course_user;
GRANT ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public TO course_user;

ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO course_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO course_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON FUNCTIONS TO course_user;

-- 显示表信息
\dt

-- 显示已创建的对象
SELECT 'Tables created successfully' as message;
