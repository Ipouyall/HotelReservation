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
- Remembering: if server restarted, it would remember the previous logged in clients
