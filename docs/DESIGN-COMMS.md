# SBB – Picolibc STDIO over COMMS

## Project Statement

### Overview

This SBB provides a reusable COMMS-backed implementation of the standard C I/O streams (`stdin`, `stdout`, and `stderr`) for applications using **picolibc**.

Unlike the UART STDIO SBB, this implementation operates on the higher-level **RM_COMMS** interface, allowing the underlying transport to be abstracted from the application.

The solution:

- Redirects standard C I/O to an RM_COMMS instance.
- Implements blocking character-based transmit and receive operations.
- Automatically initializes the COMMS stack on first use.
- Supports RTOS and non-RTOS operation.
- Uses RM_COMMS_UART as the default transport layer.
- Reuses the RS_STDIO_UART configuration architecture when UART is the COMMS lower layer.

The implementation is designed as a reusable SBB rather than a board-specific application.

---

# Hardware Requirements

## Supported MCU Families

The architecture supports:

| Family | COMMS Transport |
|----------|----------|
| RA8 | RM_COMMS_UART over SCI_B |
| RA2/RA4/RA6 | RM_COMMS_UART over SCI |
| RA0 | RM_COMMS_UART over UARTA |

through the abstraction:

```c
rm_comms_instance_t
```

---

## Required Middleware

The project requires:

```text
RM_COMMS
```

and currently assumes:

```text
RM_COMMS_UART
```

as the transport implementation.

---

## Reference Hardware

Current reference implementation:

```text
FPB-RA8E1
SCI9
Virtual COM Port
RM_COMMS_UART
```

Using:

```c
g_comms_jlob_vcom
```

---

# Functional Decomposition

## Architectural Overview

The solution is divided into three layers.

### Layer 1 — UART Configuration

Files:

```text
rs_stdio_uart.h
stdio_uart_config_ra8e1_fpb.c
```

Responsibilities:

- Configure UART peripheral.
- Validate UART settings.
- Define UART transport instance.

---

### Layer 2 — COMMS Configuration

Files:

```text
rs_stdio_comms.h
stdio_comms_config_ra8e1_fpb.c
```

Responsibilities:

- Configure RM_COMMS transport.
- Associate COMMS transport with UART transport.
- Manage RTOS synchronization objects.
- Validate COMMS application configuration.

---

### Layer 3 — STDIO Redirection

Files:

```text
syscalls_picolib_stdio_to_comms.c
```

Responsibilities:

- Redirect stdin/stdout/stderr.
- Perform blocking read operations.
- Perform blocking write operations.
- Handle COMMS callbacks.
- Perform lazy initialization.

---

# Software Architecture

```text
                printf()
                    |
                    V

              comms_putc()
                    |
                    V

         RM_COMMS_Write(1 byte)
                    |
                    V

           RM_COMMS_UART
                    |
                    V

                UART

------------------------------------------------

                 scanf()
                    |
                    V

              comms_getc()
                    |
                    V

         RM_COMMS_Read(1 byte)
                    |
                    V

           RM_COMMS_UART
                    |
                    V

                UART
```

---

# Modules / Dependencies

## Required Middleware

### RM_COMMS

Core interface:

```c
rm_comms_instance_t
rm_comms_cfg_t
rm_comms_api_t
```

Provides:

```text
Open
Read
Write
Callback registration
```

---

### RM_COMMS_UART

Provides:

```text
UART transport for RM_COMMS
```

Used through:

```c
rm_comms_uart_extended_cfg_t
rm_comms_uart_instance_ctrl_t
```

---

### UART Driver

Lower layer transport:

```text
SCI_B UART
SCI UART
UARTA
```

through:

```c
rs_stdio_uart_cfg_t
```

---

### Picolibc

The implementation requires:

```c
__PICOLIBC__
```

except when:

```c
UNIT_TEST_COMMS
```

is enabled.

---

# Configuration Requirements

## Public Configuration Structure

```c
typedef struct
{
    rm_comms_instance_t comms_instance;
    rm_comms_cfg_t      comms_cfg;

#if defined(RM_COMMS_UART_H)
    rs_stdio_uart_cfg_t stdio_uart_cfg;
    rm_comms_uart_instance_ctrl_t
        comms_uart_instance_ctrl;
#endif

} rs_stdio_comms_cfg_t;
```

---

## COMMS Configuration Components

### COMMS Instance

```c
rm_comms_instance_t
```

Contains:

```text
API pointer
Control block
Configuration block
```

---

### COMMS Configuration

```c
rm_comms_cfg_t
```

Contains:

```text
Semaphore timeout
Callback pointer
Extended configuration pointer
Lower-level configuration pointer
```

---

### COMMS UART Extended Configuration

```c
rm_comms_uart_extended_cfg_t
```

Contains:

```text
Underlying UART instance
Optional RTOS synchronization objects
```

---

# Configuration Validation

## Application Configuration Validation

Implemented by:

```c
RS_STDIO_COMMS_CheckApplicationConfiguration()
```

Purpose:

```text
Validate generated Smart Configurator COMMS configuration.
Validate UART transport configuration.
Ensure COMMS->UART ownership relationships are correct.
```

Validation flow:

1. Build temporary UART configuration.
2. Build temporary COMMS configuration.
3. Associate UART transport with COMMS transport.
4. Execute:

```c
RS_STDIO_COMMS_CheckConfig()
```

---

## Ownership Relationships

For COMMS UART operation:

```text
COMMS instance
    |
    +-- COMMS configuration
    |
    +-- COMMS UART transport
            |
            +-- UART instance
```

The configuration guarantees:

```text
COMMS references the expected UART transport.
UART configuration remains internally consistent.
```

---

# Public APIs

## RS_STDIO_COMMS_GetConfigSize()

### Purpose

Returns required configuration size.

### Inputs

```c
uint32_t * p_size
```

### Outputs

```c
sizeof(rs_stdio_comms_cfg_t)
```

---

## RS_STDIO_COMMS_GenConfig()

### Purpose

Generates a runtime COMMS configuration.

### Function Flow

1. Validate requested size.
2. Copy reference configuration.
3. Repair internal ownership pointers.
4. Validate generated configuration.
5. Return generated object.

---

## RS_STDIO_COMMS_CheckConfig()

### Purpose

Validate the COMMS configuration.

### Validation Categories

```text
COMMS ownership
COMMS API pointers
COMMS transport pointers
UART transport configuration
RTOS synchronization objects
```

---

## RS_STDIO_COMMS_CheckApplicationConfiguration()

### Purpose

Validate Smart Configurator generated COMMS configuration against the SBB reference architecture.

---

# STDIO Runtime APIs

## comms_putc()

### Purpose

Transmit a single character to the COMMS transport.

### Behavior

1. Initialize COMMS stack if required.
2. Wait for previous TX operation to complete.
3. Call:

```c
p_comms->write()
```

4. Wait for transmission completion.
5. Return transmitted character.

### Dependencies

```c
RM_COMMS_Write()
```

---

## comms_getc()

### Purpose

Read a single character from the COMMS transport.

### Behavior

1. Initialize COMMS stack if required.
2. Wait until receive operation is available.
3. Call:

```c
p_comms->read()
```

4. Wait for completion.
5. Return received character.

### Dependencies

```c
RM_COMMS_Read()
```

---

# COMMS Callback Design

## comms_jlob_vcom_callback()

Used only for:

```text
Non-RTOS operation
```

Processes:

### TX Complete

```c
RM_COMMS_EVENT_TX_OPERATION_COMPLETE
```

Action:

```text
Clear TX busy state
```

---

### RX Complete

```c
RM_COMMS_EVENT_RX_OPERATION_COMPLETE
```

Action:

```text
Clear RX busy state
```

---

# Runtime Initialization

## jlob_comms_init_once()

Provides lazy initialization.

Initialization occurs only when:

```text
stdin
stdout
stderr
```

is used for the first time.

---

### Initialization Flow

1. Select COMMS instance.

```c
g_comms_jlob_vcom
```

or

```c
g_comms_jlob_vcom_fake
```

for unit testing.

2. Validate:

```text
API pointer
Control pointer
Configuration pointer
```

3. Open COMMS transport.

```c
p_comms->open(...)
```

4. Register callback if required.

5. Mark initialized.

---

### Thread Safety

Protected using:

```c
FSP_CRITICAL_SECTION
```

to prevent multiple initialization attempts.

---

# RTOS Support

## Non-RTOS

Uses:

```c
tx_busy
rx_busy
```

and callback notifications to provide blocking behavior.

Synchronization method:

```text
Busy wait loop
```

using:

```c
__NOP()
```

---

## ThreadX

Configuration supports:

```text
TX mutex
RX mutex
TX semaphore
RX semaphore
```

through:

```c
rm_comms_mutex_t
rm_comms_semaphore_t
```

---

## FreeRTOS

Configuration supports:

```text
TX mutex
RX mutex
TX semaphore
RX semaphore
```

through the same RM_COMMS abstraction layer.

---

# Data Structures

## rs_stdio_comms_cfg_t

Purpose:

```text
Persistent COMMS application configuration.
```

Contains:

```text
COMMS instance
COMMS configuration
UART transport configuration
COMMS UART control block
```

---

## RM_COMMS_UART Extended Configuration

Purpose:

```text
Bind a COMMS transport to a UART transport.
```

Contains:

```text
UART instance
Optional RTOS synchronization objects
```

---

# Reusability & Portability

## MCU Portable

Supports:

```text
RA0
RA2
RA4
RA6
RA8
```

through RS_STDIO_UART and RM_COMMS_UART.

---

## Transport Portable

Application code depends only on:

```c
rm_comms_api_t
```

allowing future COMMS transports beyond UART.

Potential future transports:

```text
USB CDC
BLE
TCP/IP
SPI
I2C
```

provided an RM_COMMS implementation exists.

---

## Board Portable

Board-specific configuration is isolated to:

```text
stdio_comms_config_raxxx_xxx.c
```

allowing multiple boards to reuse:

```text
syscalls_picolib_stdio_to_comms.c
```

without modification.

---

# Limitations

## Blocking Design

The STDIO implementation is intentionally blocking.

Each:

```c
getc()
putc()
```

waits for transfer completion before returning.

---

## Character-Oriented Transfers

Current implementation processes:

```text
1 byte TX at a time
1 byte RX at a time
```

No additional buffering layer exists above RM_COMMS.

---

## UART-Centric Reference Implementation

The current SBB implementation assumes:

```text
RM_COMMS_UART
```

as the transport.

Additional COMMS transports would require:

```text
Additional configuration validation logic.
Additional reference configuration files.
```

---

# Conclusion

This SBB provides a portable architecture for integrating **picolibc standard I/O** with the Renesas **RM_COMMS** framework.

The design cleanly separates:

1. UART transport configuration.
2. COMMS transport configuration.
3. Application configuration validation.
4. Runtime STDIO redirection.

The resulting architecture enables application code to use:

```c
printf(...)
fprintf(...)
getchar(...)
scanf(...)
```

through a transport-independent COMMS interface while still leveraging the RM_COMMS_UART transport for physical communication.