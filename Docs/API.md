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
    "token": "<auth-token>",
    "privilege": "true/flase",
    "time": "<dd-mm-yyyy>"
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
    "time": "<dd-mm-yyyy>"
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
    "time": "<dd-mm-yyyy>"
}
```

If username has taken:

```json
{
    "kind": "error",
    "status_code": "231",
    "status": "User successfully signed up.",
    "message": "Username exists, try another one.",
    "time": "<dd-mm-yyyy>"
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
    "time": "<dd-mm-yyyy>"
}
```

On failure:

```json
{
    "kind": "error",
    "status_code": "503",
    "status": "Bad sequence of commands.",
    "message": "Signing up failed, make sure you entered valid data.",
    "time": "<dd-mm-yyyy>"
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
    "time": "<dd-mm-yyyy>"
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
    "time": "<dd-mm-yyyy>",
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
    "time": "<dd-mm-yyyy>",
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
    "time": "<dd-mm-yyyy>",
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
    ]
}
```

## View-rooms

Used to get data about our hotel's users.

### View-rooms/client

```json
{
    "kind": "view_rooms_info",
    "token": "<user-auth-token>",
}
```

### View-rooms/server

```json
{
    "kind": "success",
    "status_code": "001",
    "status": "Request validated successfully",
    "message": "Here is our rooms information",
    "time": "<dd-mm-yyyy>",
    "data": 
    [
        {
            "number": "<room-number>",
            "status": "true/false",
            "total beds": "<max-capacity>",
            "available bed": "<current-capacity>",
            "price(each bed)": "<price-per-bed>",
            "users": [
                {
                    "id": "<id>",
                    "beds": "<number-of-reservation>",
                    "check-in": "<dd-mm-yyyy>",
                    "check-out": "<dd-mm-yyyy>"
                },
                {
                    "id": "<id2>",
                    "beds": "<number-of-reservation>",
                    "check-in": "<dd-mm-yyyy>",
                    "check-out": "<dd-mm-yyyy>"
                }
            ]
        },
        {
            "number": "<room-number2>",
            "status": "true/false",
            "total beds": "<max-capacity>",
            "available bed": "<current-capacity>",
            "price(each bed)": "<price-per-bed>",
            "users": [
                {
                    "id": "<id>",
                    "beds": "<number-of-reservation>",
                    "check-in": "<dd-mm-yyyy>",
                    "check-out": "<dd-mm-yyyy>"
                },
                {
                    "id": "<id2>",
                    "beds": "<number-of-reservation>",
                    "check-in": "<dd-mm-yyyy>",
                    "check-out": "<dd-mm-yyyy>"
                }
            ]
        }
    ]
}

```

## Book-a-room

Used to book a room.
This is only available for ***user***s.

### Book-a-room/client
```json
{
    "kind": "booking",
    "token": "<user-auth-token>",
    "roomID": "<room-number>",
    "beds_count": "<number-of-reservation>",
    "check_in": "<dd-mm-yyyy>",
    "check_out": "<dd-mm-yyyy>"
}
```

### Book-a-room/server
On success:

```json
{
    "kind": "booking",
    "status_code": "110",
    "status": "Successfully done.",
    "message": "Room reserved successfully",
    "time": "<dd-mm-yyyy>",
}
```

On failure:

```json
{
    "kind": "error",
    "status_code": "110",
    "status": "Successfully done.",
    "message": "Room reserved successfully",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only users can book a room",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "101",
    "status": "The desired room was not found",
    "message": "Room number doesn't exist",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "503",
    "status": "Bad sequence of commands.",
    "message": "You should cancel your previous reservation/Error in entered dates",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "108",
    "status": "Your account balance is not enough",
    "message": "You don't have enough money to book this room",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "109",
    "status": "The room capacity is full",
    "message": "Room doesn't meet your request",
    "time": "<dd-mm-yyyy>",
}
```

## View-reservations
Used for getting user reservations.
This is only available for ***user***s.

### View-reservations/client
```json
{
    "kind": "view_reservations",
    "token": "<user-auth-token>",
}
```

### View-reservations/server
On success:
```json
{
    "kind": "success",
    "status_code": "001",
    "status": "Request validated successfully",
    "message": "Here is your reservations",
    "time": "<dd-mm-yyyy>",
    "data": 
    [
        {
            "room number": "<room-number>",
            "price(per bed)": "<price-per-bed>",
            "bed(s) you have": "<number-of-reservation>",
            "check in": "<dd-mm-yyyy>",
            "check out": "<dd-mm-yyyy>"
        },
        {
            "room number": "<room-number2>",
            "price(per bed)": "<price-per-bed>",
            "bed(s) you have": "<number-of-reservation>",
            "check in": "<dd-mm-yyyy>",
            "check out": "<dd-mm-yyyy>"
        }
    ]
}
```
On failure:
```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only users can book a room",
    "time": "<dd-mm-yyyy>",
}
```

(which shouldn't normally happen):

```json
{
    "kind": "error",
    "status_code": "000",
    "status": "No content.",
    "message": "Operation failed, please try again later!",
    "time": "<dd-mm-yyyy>",
}
```

## Pass-days
Used to pass day of application.
This is only available for ***super-user***s or ***admin***s.

### Pass-daysView-reservations/client
```json
{
    "kind": "passing_time",
    "token": "<user-auth-token>",
    "days": "<number-of-days>"
}
```

### Pass-daysView-reservations/server
On success:
```json
{
    "kind": "success",
    "status_code": "110",
    "status": "Successfully done.",
    "message": "System's date updated successfully",
    "time": "<dd-mm-yyyy>",
}
```
On failure:
```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only admins can pass days",
    "time": "<dd-mm-yyyy>",
}
```

## Edit-user-info
Used to edit user's information.


### Edit-user-info/client
```json
{
    "kind": "edit_information",
    "token": "<user-auth-token>",
    "pass": "<new password>",
    "phone": "<user new phone number>",
    "addr": "<user new address>",
    "addr": "<dd-mm-yyyy>",
}
```

### Edit-user-info/server
On success:
```json
{
    "kind": "success",
    "status_code": "110",
    "status": "Successfully done.",
    "message": "User information updated successfully",
    "time": "<dd-mm-yyyy>",
}
```
On failure:
```json
{
    "kind": "error",
    "status_code": "503",
    "status": "Bad sequence of commands.",
    "message": "Operation failed, please try again later",
    "time": "<dd-mm-yyyy>",
}
```

## Leave-room
Used when user want to leave room.

### Leave-room/client
```json
{
    "kind": "leaving",
    "token": "<user-auth-token>",
    "roomID": "<room-number>"
}
```

### Leave-room/server
On success:
```json
{
    "kind": "success",
    "status_code": "413",
    "status": "successfully Leaving.",
    "message": "Hope you enjoyed our hotel, bye",
    "time": "<dd-mm-yyyy>",
}
```
On failure:
```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only users can leave a room!",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "503",
    "status": "Bad sequence of commands.",
    "message": "Only users can leave a room!",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "102",
    "status": "Your reservation was not found",
    "message": "You are not in this room (at least, yet)",
    "time": "<dd-mm-yyyy>",
}
```

## 
Used Empty-roomwhen user want to make room empty.
This is only available for ***super-user***s or ***admin***s.
### Empty-room/client
```json
{
    "kind": "emptying",
    "token": "<user-auth-token>",
    "roomID": "<room-number>",
}
```

### Empty-room/server
On success:

```json
{
    "kind": "success",
    "status_code": "413",
    "status": "successfully Leaving.",
    "message": "All users has kicked out",
    "time": "<dd-mm-yyyy>",
}
```
On failure:

```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only admins can manage a room!",
    "time": "<dd-mm-yyyy>",
}
```
```json
{
    "kind": "error",
    "status_code": "101",
    "status": "The desired room was not found",
    "message": "That isn't a valid room",
    "time": "<dd-mm-yyyy>",
}
```

## Modify-room
Used for changing the room attributes.
This is only available for ***super-user***s or ***admin***s.

### Modify-room/client
```json
{
    "kind": "modify_room",
    "token": "<user-auth-token>",
    "roomID": "<room-number>",
    "max_cap": "<max-capacity>",
    "price": "<price-per-bed>"
}
```

### Modify-room/server
On success:
```json
{
    "kind": "success",
    "status_code": "105",
    "status": "Successfully modified.",
    "message": "Room (<room-num>) modified successfully",
    "time": "<dd-mm-yyyy>",
}
```
On failure:

```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only admins can manage a room!",
    "time": "<dd-mm-yyyy>",
}
```
```json
{
    "kind": "error",
    "status_code": "101",
    "status": "The desired room was not found",
    "message": "That isn't a valid room",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "109",
    "status": "The room capacity is full",
    "message": "Room should be empty for modification!",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "000",
    "status": "No content.",
    "message": "Couldn't modify room!!!",
    "time": "<dd-mm-yyyy>",
}
```
## Remove-room
Used for removing the room.
This is only available for ***super-user***s or ***admin***s.

### Remove-room/client
```json
{
    "kind": "remove_room",
    "token": "<user-auth-token>",
    "roomID": "<room-number>"
}
```

### Remove-room/server
On success:
```json
{
    "kind": "success",
    "status_code": "106",
    "status": "Successfully deleted.",
    "message": "Room (<room-num>) removed successfully",
    "time": "<dd-mm-yyyy>",
}
```
On failure:

```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only admins can manage a room!",
    "time": "<dd-mm-yyyy>",
}
```
```json
{
    "kind": "error",
    "status_code": "101",
    "status": "The desired room was not found",
    "message": "This isn't a valid room",
    "time": "<dd-mm-yyyy>",
}
```

```json
{
    "kind": "error",
    "status_code": "109",
    "status": "The room capacity is full",
    "message": "Room should be empty for removing!",
    "time": "<dd-mm-yyyy>",
}
```

## Add-room
Used for adding the room.
This is only available for ***super-user***s or ***admin***s.

### Add-room/client
```json
{
    "kind": "remove_room",
    "token": "<user-auth-token>",
    "roomID": "<room-number>"
}
```

### Add-room/server
On success:
```json
{
    "kind": "success",
    "status_code": "104",
    "status": "Successfully added.",
    "message": "Room (<room-num>) added successfully",
    "time": "<dd-mm-yyyy>",
}
```
On failure:

```json
{
    "kind": "error",
    "status_code": "403",
    "status": "Access denied!",
    "message": "Only admins can manage a room!",
    "time": "<dd-mm-yyyy>",
}
```
```json
{
    "kind": "error",
    "status_code": "111",
    "status": "This room already exists",
    "message": "This room already exists",
    "time": "<dd-mm-yyyy>",
}
```
