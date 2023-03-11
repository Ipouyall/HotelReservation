# HotelReservation/ User Manager
This class manage all users in our HotelReservation application such as adding , logging in, logging out, etc.

- First of all it initialize `std::vector<UserData> users` by reading `UsersInfo.json`.

Now we describe it;s methods:

- `search_by_username`: Search based on username given and return index of user or -1 if doesn't exist.
- `search_by_token`: Search based on token given and return index of user or -1 if doesn't exist.
- `username_exist`: Check that if username give exist in users(`return true`) or not (`return false`).
- `user_validation`: Check that password given is equal to password saved for username given.
- `signup`: Add new user to users based on information given and return true if successfully done.
    also check if username or address are not *null* and phone number is valid.
- `login`: Change `is_logged_in` atrribute to `true` , set token and then return it for user based on username given.
- `logout`: Change `is_logged_in` atrribute to `false` , clear token and then return `true` if successfully done.
- `get_role`: Return role of user based on token given.
- `is_logged_in`: Check user is logged in(`return true`) or not (`return false`) based on username given.
- `reduce_balance`: Reduce account balance of user.
- `increase_balance`: Increase account balance of user.
- `edit_information`: Edit user information.
- `get_id`: Return id of user base on token given.
