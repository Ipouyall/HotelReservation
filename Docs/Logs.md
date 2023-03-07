# HotelReservation/ Control logs

We designed two ways for logging:

1. terminal: we use `google/glog` package for logging and logs are server on **stderr** be default.
    - It's purpose is to generate useful logs to present the program flow.
2. file: we defined our own package to write logs in files(third party packages are also available)
    - this is based on the project and designed to track state-transitions and prove program correctness

Currently, you can't change log-level using command line arguments.
