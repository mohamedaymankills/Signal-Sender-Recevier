# Real-Time-Signals-Sender-Recevier

## Table of Contents

1. [Introduction](#introduction)
2. [Project Structure](#project-structure)
3. [Features](#features)
4. [Usage](#usage)
5. [Implementation Details](#implementation-details)
   - [Real-Time Signals Overview](#real-time-signals-overview)
   - [Sender Program](#sender-program)
   - [Receiver Program](#receiver-program)
6. [Signal Workflow](#signal-workflow)
7. [Configuration and Setup](#configuration-and-setup)
   - [Core Dump Configuration](#core-dump-configuration)
8. [Example Execution](#example-execution)
9. [Error Handling](#error-handling)
10. [Future Enhancements](#future-enhancements)

---

## Introduction

This project demonstrates inter-process communication using **real-time signals** in a Linux environment. The system consists of two applications:

1. **Sender**: Sends a real-time signal (`SIGRTMIN`) to a receiver process with attached data (0 or 1).
2. **Receiver**: Handles the signal and takes action based on the attached data:
   - **0**: Terminates gracefully.
   - **1**: Aborts with a core dump.


The project highlights the use of POSIX real-time signals, which support queueing and data transmission, making them suitable for real-time systems and advanced IPC scenarios.

---

## Project Structure

```
real_time_signals_project/
├── sender.c          # Implementation of the Sender program
├── receiver.c        # Implementation of the Receiver program
├── Makefile          # Build automation for the project
├── README.md         # Project documentation
```

---

## Features

- **Real-Time Signal Communication**: Utilizes `SIGRTMIN` for inter-process communication.
- **Data Transmission**: Sends integer data with signals (`0` for termination, `1` for abort with core dump).
- **Signal Queueing**: Supports multiple signal transmissions without overwriting.
- **Custom Signal Handling**: Processes attached data in the receiver to determine actions.
- **Graceful Termination and Debugging**:
  - Clean shutdown for `data = 0`.
  - Generates core dumps for debugging when `data = 1`.

---

## Usage

### Build the Project

Compile the Sender and Receiver programs using the provided `Makefile`:
```bash
make
```

This generates the following executables:
- `sender`
- `receiver`

### Running the Programs

1. **Start the Receiver**:
   ```bash
   ./receiver
   ```
   The receiver prints its **PID** to the terminal.

2. **Run the Sender**:
   Pass the receiver's PID and data (0 or 1) as arguments:
   ```bash
   ./sender <receiver_pid> <data>
   ```

3. **Observe Receiver Behavior**:
   - Data = `0`: Receiver terminates gracefully.
   - Data = `1`: Receiver aborts and generates a core dump.

---
## Flowcharts
### Sender
```sql
START
 |
 |---> Parse command-line arguments (receiver_pid, data)
 |
 |---> Validate input (data == 0 or 1)
 |
 |---> Use sigqueue() to send SIGRTMIN with data to receiver_pid
 |
 |---> Print confirmation
 |
END
```

### Recevier
```sql
START
 |
 |---> Print receiver PID
 |
 |---> Setup SIGRTMIN handler using sigaction()
 |
 |---> WAIT for signals (pause in infinite loop)
      |
      |---> On SIGRTMIN:
              |---> Extract data (info->si_value.sival_int)
              |---> If data == 0: Terminate gracefully (exit(0))
              |---> If data == 1: Abort with core dump (abort())
END
```
---
## Implementation Details

### Real-Time Signals Overview

Real-time signals (e.g., `SIGRTMIN` to `SIGRTMAX`) enhance traditional UNIX signals by:
- Allowing queueing of multiple signals.
- Supporting data transmission with `sigqueue()`.

### Sender Program

The Sender program:
1. Retrieves the receiver's PID and data as command-line arguments.
2. Sends a `SIGRTMIN` signal using `sigqueue()`.
3. Attaches data (`0` or `1`) to the signal.

#### Code Snippet (Sender)
```c
union sigval value;
value.sival_int = data;
sigqueue(receiver_pid, SIGRTMIN, value);
```

### Receiver Program

The Receiver program:
1. Registers a signal handler for `SIGRTMIN` using `sigaction()`.
2. Extracts data from the signal using `info->si_value.sival_int`.
3. Executes actions based on the data:
   - **0**: Terminates gracefully.
   - **1**: Calls `abort()` to generate a core dump.

#### Code Snippet (Receiver Handler)
```c
void handle_signal(int sig, siginfo_t *info, void *context) {
    int data = info->si_value.sival_int;
    if (data == 0) {
        printf("Terminating gracefully.\n");
        exit(0);
    } else if (data == 1) {
        printf("Aborting with core dump.\n");
        abort();
    }
}
```

---

## Signal Workflow

1. The Sender sends `SIGRTMIN` to the Receiver with attached data.
2. The Receiver's signal handler is invoked.
3. Based on the data:
   - **0**: Terminates with `exit()`.
   - **1**: Aborts with `abort()` (causing a core dump).

---

## Configuration and Setup

### Core Dump Configuration

1. **Enable Core Dumps**:
   ```bash
   ulimit -c unlimited
   ```

2. **Check Core Dump Location**:
   ```bash
   cat /proc/sys/kernel/core_pattern
   ```
   Common patterns include:
   - `core`: Current working directory.
   - `/var/lib/systemd/coredump`: Managed by `systemd`.

3. **Retrieve Core Dumps**:
   If managed by `systemd`:
   ```bash
   coredumpctl list
   coredumpctl dump <program_name> > core
   ```

---

## Example Execution

1. **Start the Receiver**:
   ```bash
   ./receiver
   ```
   Output:
   ```
   Receiver PID: 12345
   ```

2. **Send Signals**:
   - **Terminate Signal**:
     ```bash
     ./sender 12345 0
     ```
   - **Abort Signal**:
     ```bash
     ./sender 12345 1
     ```

3. **Result**:
   - Data = 0: Receiver terminates.
   - Data = 1: Receiver aborts, and a core dump is generated.

---

## Error Handling

- **Invalid Arguments**:
  The Sender checks if the PID and data are provided.
  ```c
  if (argc != 3) {
      fprintf(stderr, "Usage: %s <receiver_pid> <data>\n", argv[0]);
      return 1;
  }
  ```

- **Signal Sending Errors**:
  If `sigqueue()` fails, an error is printed:
  ```c
  if (sigqueue(receiver_pid, SIGRTMIN, value) == -1) {
      perror("sigqueue");
      return 1;
  }
  ```

---

## Future Enhancements

1. **Extended Signal Handling**:
   - Support additional real-time signals.
   - Add more complex data handling.

2. **Cross-Platform Support**:
   - Ensure compatibility with non-Linux POSIX systems.

3. **Logging**:
   - Add logging to record signal events and actions.

4. **Core Dump Analysis**:
   - Automate analysis of generated core dumps.

---

This project serves as an educational example of using real-time signals for IPC and demonstrates their advantages over traditional signals in Linux.
