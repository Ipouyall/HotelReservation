# HotelReservation

Socket Programming,First CA of The Computer Networks course @ University of Tehran, spring 2023

## How to run

### 01. Install Requirements

On macos, run:

```shell
brew install cmake libedit
```

For linux:

```shell
sudo apt-get install cmake libedit-dev
```

### 02. Generate Makefile

Run:

```shell
cmake .
```

This could take up to 30(min) for the first time.

### Build project

Run:

```shell
make
```

This command would generate `hr_test`, `hr_client`, `hr_server`.
***

## Features

- Powerful CLI
  - enabled auto-completion: use `tab` for this feature
  - command history: use `up` and `down` arrow keys for this feature
- Handle multiple clients
- State recovering: if server restarted, it would remember the previous logged in clients

## Overview

### Start

- To start server:

```shell
./hr_server
```

After starting, it would ask for current date:

![Server](Docs/screenshots/run-server.png)

After setting system's date, server would start listen on the port specified in server's config.

- To start client:

```shell
./hr_client
```

After connecting to server, command-line tools would get activated.

![Client](Docs/screenshots/run-client.png)

### Connecting

Client would try to connect to the server in the specified port:

![Connecting](Docs/screenshots/connect-to-server.png)

### Sign up

You can use `sign up` command to signing up in a hotel:

- Enter your username to check if it is available:

![Signup1](Docs/screenshots/signup-vu.png)

- Then you have to fill the reservation form:

![SignupFill](Docs/screenshots/signup-completed.png)

- Sign up may fail if username has already taken:

![SignupFail](Docs/screenshots/signup-nvu.png)

### Sign in

You can use `sign in` command to signing in to a hotel:

![Sign in success](Docs/screenshots/signin-s.png)

Or, it may fail due to wrong info(Morad) or if user is online using another client(Pouya):

![Sign in failed](Docs/screenshots/signin-f.png)

### View User info

You can see your profile:

![Profile admin](Docs/screenshots/profile-admin.png)

![Profile user](Docs/screenshots/profile-user.png)

### View all users

Admins can see all hotel users' profile:

![View all users](Docs/screenshots/all-users.png)

### View rooms information

Users can see hotel's room and a summary of their status:

![View rooms user](Docs/screenshots/view-rooms-user.png)

While admins are able to see users reservations for rooms:

![view rooms admin](Docs/screenshots/view-rooms-admin.png)

### Booking

Users can book a room:

![Book room](Docs/screenshots/book-room.png)

This would also update your account's charge:

![decrease charge](Docs/screenshots/profile-after-reserve.png)

### Cancelling

Users can cancel their reservations:

![cancel reservation](Docs/screenshots/cancelling.png)

And you would earn ***50%*** of the money back:

![balance after cancelling](Docs/screenshots/profile-after-cancel.png)

### Pass day


### Ending program

- To quit client:
  - You can use `quit` or `exit` command
  - You can use `Ctrl + C` to quit
- To quit server:
  - You can use `exit` or `quit` command
  - You shouldn't use `Ctrl + C` to quit, it is unsafe and wouldn't save the state of the server
