# HotelReservation/API

Server and client use json-formatted messages for communication.

**Attention** server's statuses used is response, stored in `Errors.json` and are based on status code.

## Sign-in

### Sign-in/client

Client would send a message to server with following format:

```json
{
    "kind": "sign_in",
    "username": "<username>",
    "password": "<password>",
}
```

### Sign-in/server

On success, server would response like:

```json
{
    "kind": "success",
    "status_code": "230",
    "status": "User logged in.",
    "message": "You logged in successfully",
    "token": "<auth-token>"
}
```

- `auth-token` : using for remembering user and all logged-in users should use it with their later requests.

On failure:

```json
{
    "kind": "error",
    "status_code": "430",
    "status": "Invalid username or password.",
    "message": "Sign_in failed due to invalid password/username or you logged in before.",
}
```

This would happen due to invalid username/password, 
also, we designed that such that user can be in our system with only one client and can't use more than one client at the same time.

## Sign-up

At first, client would ask if username is valid:

### Sign-up/client/validating username

```json
{
    "kind": "free_username",
    "username": "<username>",
}
```

### Sign-up/server/validating username

If username is valid and available:

```json
{
    "kind": "success",
    "status_code": "311",
    "status": "User Signed up. Enter your password, purse, phone and address.",
    "message": "Just validated you username, please complete your registration.",
}
```

If username has taken:

```json
{
    "kind": "error",
    "status_code": "231",
    "status": "User successfully signed up.",
    "message": "Username exists, try another one.",
}
```

### Sign-up/client/adding new user

```json
{
    "kind": "sign_up",
    "username": "<username>",
    "password": "<password>",
    "balance": "<user's charge>",
    "phone": "<user phone number>",
    "addr": "<user address>",
}
```

### Sign-up/server/adding new user

On success:

```json
{
    "kind": "success",
    "status_code": "311",
    "status": "User Signed up. Enter your password, purse, phone and address.",
    "message": "We honor to announce you are part of our community from now on.",
}
```

On failure:

```json
{
    "kind": "error",
    "status_code": "503",
    "status": "Bad sequence of commands.",
    "message": "Signing up failed, make sure you entered valid data.",
}
```

## Logout

### Logout/client

```json
{
    "kind": "success",
    "token": "<auth-token>",
}
```

### Logout/server

```json
{
    "kind": "success",
    "status_code": "201",
    "status": "User logged out successfully.",
    "message": "Hope to see you later!",
}
```

## Recover

This command is used to logging user in automatically when server restarted (server would remember user's auth-token for 2 min after The last user that was logged in).

Currently, it hasn't implemented in server-side and also client isn't using it yet,
instead, client would handle that and login again!

### Recover/client

```json
{
    "kind": "sign_in",
    "token": "<token>",
}
```

## View-user-info

Each user can view it's own data (except password).

### View-user-info/client

```json
{
    "kind": "user_info",
    "token": "<user-auth-token>",
}
```

### View-user-info/server

On success:

```json
{
    "kind": "success",
    "status_code": "001",
    "status": "Request validated successfully",
    "message": "Here is your information",
    "data": 
    {
        "id": "<id>",
        "username": "<username>",
        "role": "super-user/user",
        "status": "Online/Offline",
        "purse": "<account_balance> for non super-user",
        "phone number": "<phone_number> for non super-user",
        "address": "<address> for non super-user",
    }
}
```

On failure(which shouldn't normally happen):

```json
{
    "kind": "error",
    "status_code": "000",
    "status": "No content.",
    "message": "Couldn't find any data from this user, you may need to login again or edit your info.",
}
```

## View-users

Used to get data about our hotel's users.
This is only available for ***super-user***s or ***admin***s.

### View-users/client

```json
{
    "kind": "view_users_info",
    "token": "<user-auth-token>",
}
```

### View-users/server

```json
{
    "kind": "success",
    "status_code": "001",
    "status": "Request validated successfully",
    "message": "Here is all users in our hotel",
    "data": 
    [
        {
        "id": "<id>",
        "username": "<username>",
        "role": "super-user/user",
        "status": "Online/Offline",
        "purse": "<account_balance> for non super-user",
        "phone number": "<phone_number> for non super-user",
        "address": "<address> for non super-user",
        },
        {
        "id": "<id2>",
        "username": "<username2>",
        "role": "super-user/user",
        "status": "Online/Offline",
        "purse": "<account_balance> for non super-user",
        "phone number": "<phone_number> for non super-user",
        "address": "<address> for non super-user",
        },
        ...
    ]
}
```
