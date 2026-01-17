export module user;

import database;
import std;
import <print>;
import <memory>;

export class User {
public:
    std::string userid;
    std::string username;

protected:
    std::string password_hash;

public:
    User(std::string user_id, std::string user_name, std::string password);
    virtual ~User();

    // 数据库操作接口
    virtual bool load_from_db(std::shared_ptr<DatabaseConnection> db) = 0;
    virtual bool save_to_db(std::shared_ptr<DatabaseConnection> db) = 0;
    virtual bool delete_from_db(std::shared_ptr<DatabaseConnection> db) = 0;

    // 身份验证
    bool verify_password(const std::string& input_password) const;
    void change_password(const std::string& new_password);

    // 登录登出
    virtual bool login(const std::string& input_password);
    virtual void logout();

    // 面板显示
    virtual void getdashboard() = 0;

    // 静态方法
    static std::vector<std::shared_ptr<User>> load_all_from_db(
        std::shared_ptr<DatabaseConnection> db);

    static std::shared_ptr<User> authenticate(
        std::shared_ptr<DatabaseConnection> db,
        const std::string& username,
        const std::string& password);
};


User::User(std::string user_id, std::string user_name, std::string password)
    : userid(std::move(user_id)), username(std::move(user_name))
    , password_hash(std::move(password)) {}

User::~User() = default;

bool User::verify_password(const std::string& input_password) const {
    return password_hash == input_password;
}

void User::change_password(const std::string& new_password) {
    password_hash = new_password;
    std::print("{} 的密码已更改\n", username);
}

bool User::login(const std::string& input_password) {
    if (verify_password(input_password)) {
        std::print("✅ {} 登录成功\n", username);
        return true;
    }
    std::print("❌ {} 登录失败：密码错误\n", username);
    return false;
}

void User::logout() {
    std::print("{} 已登出\n", username);
}

std::vector<std::shared_ptr<User>> User::load_all_from_db(
    std::shared_ptr<DatabaseConnection> db) {

    std::vector<std::shared_ptr<User>> users;

    try {
        auto result = db->execute(R"(
            SELECT user_id, username, user_type
            FROM users
            ORDER BY user_id
        )");

        for (int i = 0; i < result.row_count(); i++) {
            std::string id = result.get_value(i, 0);
            std::string name = result.get_value(i, 1);
            std::string type = result.get_value(i, 2);

            auto user = std::make_shared<User>(id, name, "");
            users.push_back(user);
        }

    } catch (const std::exception& e) {
        std::print("加载用户失败: {}\n", e.what());
    }

    return users;
}

std::shared_ptr<User> User::authenticate(
    std::shared_ptr<DatabaseConnection> db,
    const std::string& username,
    const std::string& password) {

    try {
        auto result = db->execute_params(R"(
            SELECT user_id, username, password_hash, user_type
            FROM users
            WHERE username = $1 AND password_hash = $2
        )", {username, password});

        if (result.row_count() > 0) {
            std::string id = result.get_value(0, 0);
            std::string name = result.get_value(0, 1);

            auto user = std::make_shared<User>(id, name, password);
            return user;
        }

    } catch (const std::exception& e) {
        std::print("认证失败: {}\n", e.what());
    }

    return nullptr;
}
