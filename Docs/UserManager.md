# HotelReservation/ User Manager


### UserData
First we started with describing struct for users in our hotel reservation app. It hold all information about user.
```c++
typedef struct UserData{
    int id;
    std::string username;
    std::string password;
    bool privilege; // user is admin
    int account_balance;
    std::string phone_number;
    std::string address;
    bool is_logged_in = false;
    int socket_fd = -1;
    std::string token = "";
    std::string to_string();
} UserData;
```

### UserManager

This class manages all actions regarding users in our HotelReservation application such as adding, logging in, logging out, etc.

- First of all it initialize std::vector<UserData> users by reading UsersInfo.json.

```c++
class UserManager{
private:
    std::vector<UserData> users;

private:
    bool id_exists(int id);
    int create_Id();
    int search_by_token(std::string token);
    int search_by_username(std::string username);
    
public:
    UserManager();
    bool username_exist(std::string username);
    bool user_validation(std::string username, std::string password);
    bool signup(std::string username, std::string password,
                         int balance, std::string phone, std::string addr);
    std::string login(std::string username, int ufd);
    bool logout(std::string token);
    bool is_logged_in(std::string username);
    UserRole get_role(std::string token);
    void printINfo();
    bool reduce_balance(std::string token, int price);
    bool increase_balance(std::string token, int amount);
    bool edit_information(std::string token, std::string new_pass="", std::string phone="", std::string addr="");
    int get_id(std::string token);
    bool have_enough_money(std::string token, int price);
    void save(std::string path);
    void client_dead(int fd);
    std::string get_user_data(std::string token);
    std::string get_users_data(std::string token);
    std::string get_username(std::string token);
};
```

Now we describe its methods:
```c++
bool id_exists(int id);
```
- Check that id is alredy exist or not.
```c++
int create_Id();
```
- Create id for new user.
```c++
int search_by_token(std::string token);
```
- Search based on given token and return index of user or -1 if it doesn't exist.

```c++
int search_by_username(std::string username);
```
- Search based on given username and return index of user or -1 if it doesn't exist.

```c++
bool username_exist(std::string username);
```
- Checks if given username exists in users and (`return true`) if it does not (`return false`).

```c++
bool user_validation(std::string username, std::string password);
```
- Check that given password is equal to password saved for given username.

```c++
bool signup(std::string username, std::string password,
                         int balance, std::string phone, std::string addr);
```
- Add new user to users based on given information and return true if it’s successfully done.

```c++
std::string login(std::string username, int ufd);
```
- Change is_logged_in attribute to true, set token and then return it for user based on given username.

```c++
bool logout(std::string token);
```
- Change is_logged_in attribute to false , clear token and then return true if it’s successfully done.

```c++
UserRole get_role(std::string token);
```
- Return role of user based on given token.

```c++
bool is_logged_in(std::string username);
```
- Check if user is logged in and (`return true`) or else (`return false`) based on given username.

```c++
bool reduce_balance(std::string token, int price);
```
- Reduce account balance of user.

```c++
bool increase_balance(std::string token, int amount);
```
- Increase account balance of user.

```c++
bool edit_information(std::string token, std::string new_pass="", std::string phone="", std::string addr="");
```
- Edit user information.

```c++
int get_id(std::string token);
```
- Return id of user base on `token` given. Return -1 if not exist.

```c++
bool have_enough_money(std::string token, int price);
```
- Check that user with `token` has money more or equal than `price`.

```c++
void save(std::string path);
```
- Save state of user manager in `UsersInfo.json`.

```c++
void client_dead(int fd);
```
- Check that if the client with socket fd = `fd` , should deactive the user of that client.

```c++
std::string get_user_data(std::string token);
```
- Return json string of user iformation with token = `token`.
example
```json
{
    "id": "2",
    "username": "Ali",
    "role": "super-user",
    "status": "Online",
    "purse": 1000,
    "phone number": "09111111111",
    "address": "Ferdos"
}
```
```c++
std::string get_users_data(std::string token);
```
- Return string of json array of users information.
```c++
std::string UserManager::get_username(std::string token)
```
- Return username of user with token = `token`.