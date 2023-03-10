# HotelReservation/ User Manager
This class manage all users in our HotelReservation application such as adding , logging in, logging out, etc.

- First of all it initialize `std::vector<UserData> users` by reading `UsersInfo.json`.

Now we describe it;s methods:

- `searchByUsername`: Search based on username given and return index of user or -1 if doesn't exist.
- `searchByToken`: Search based on token given and return index of user or -1 if doesn't exist.
- `setFd`: Change user fd who correspond to token give with fd given.
- `usernameExist`: Check that if username give exist in users(`return true`) or not (`return false`).
- `userValidation`: Check that password given is equal to password saved for username given.
- `addUser`: Add new user to users based on information given and return true if successfully done.
- `setToken`: Set token for user based on username given.
- `userLoggedIn`: Change `is_logged_in` atrribute to `true` , set token and then return it for user based on username given.
- `userLoggedOut`: Change `is_logged_in` atrribute to `false` , clear token and then return `true` if successfully done.
- `getRole`: Return role of user based on token given.
- `isLoggedIn`: Check user is logged in(`return true`) or not (`return false`) based on username given.
- `reduceBalance`: Reduce account balance of user.
- `editInformation`: Edit user information.

 
