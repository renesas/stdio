# SBB – Picolibc STDIO over UART

## Project Statement

### Overview

This SBB provides a reusable UART-backed implementation of the standard C I/O streams (`stdin`, `stdout`, and `stderr`) for applications using **picolibc**.

The solution:

- Redirects standard C I/O to a Renesas FSP UART driver.
- Provides buffered transmit and receive functionality using ByteQ.
- Automatically initializes the UART on first use.
- Supports multiple UART peripherals through a common abstraction layer.
- Supports SCI_B UART, SCI UART, UARTA, and host-side fake UART implementations through a common configuration structure.

The implementation is designed as a reusable SBB rather than a board-specific application.

---

## Hardware Requirements

### Supported MCU Families

The architecture supports:

| Family | Driver |
|----------|----------|
| RA8 | R_SCI_B_UART |
| RA2/RA4/RA6 | R_SCI_UART |
| RA0 | R_UARTA |

through conditional compilation in:

```c
rs_stdio_uart.h
```

### Required Peripheral

The project requires:

```text
UART Peripheral
```

configured as:

```text
115200 baud (or target baud rate)
8 data bits
No parity
1 stop bit
No DMA/DTC
```

### Reference Hardware

Current reference implementation:

```text
FPB-RA8E1
SCI9
Virtual COM Port
```

Using:

```c
g_uart_jlob_vcom
```

---

# Functional Decomposition

## Architectural Overview

The solution is divided into two primary layers.

### Layer 1 — Configuration Validation

Files:

```text
rs_stdio_uart.h
rs_stdio_uart.c
stdio_uart_config_raxxx_fpb.c
```

Responsibilities:

- Store UART configuration.
- Generate runtime configuration.
- Verify application configuration.
- Verify board-specific UART settings.
- Validate ownership relationships.

### Layer 2 — STDIO Redirection

Files:

```text
syscalls_picolib_stdio_to_uart.c
```

Responsibilities:

- Redirect stdin/stdout/stderr.
- Buffer UART transmit traffic.
- Buffer UART receive traffic.
- Provide getchar()/putchar() functionality.
- Handle UART interrupt callbacks.
- Perform lazy initialization.

---

## Software Architecture

```text
                 printf()
                    |
                    V

              uart_putc()
                    |
                    V

              TX Byte Queue
                    |
                    V

             uart_flush()
                    |
                    V

         uart_instance->write()

------------------------------------------------

         UART RX Interrupt Callback
                    |
                    V

             RX Byte Queue
                    |
                    V

              uart_getc()
                    |
                    V

                 scanf()
```

---

# Modules / Dependencies

## FSP Modules

### Required

```text
R_UART_API
```

through:

```c
uart_instance_t
uart_cfg_t
uart_api_t
```

### Required Queue Module

```text
R_BYTEQ
```

Used for:

```text
RX buffering
TX buffering
```

Internal queue size:

```c
256 bytes RX
256 bytes TX
```

---

## Picolibc

The implementation requires:

```c
__PICOLIBC__
```

and intentionally rejects other C libraries:

```c
#if !defined (__PICOLIBC__)
#error ...
#endif
```

---

# Configuration Requirements

## Public Configuration Structure

```c
typedef struct
{
    ioport_port_pin_t txd;
    ioport_port_pin_t rxd;

    uart_instance_t uart_instance;
    uart_cfg_t uart_cfg;

    ...
} rs_stdio_uart_cfg_t;
```

The structure contains:

### Generic UART Information

```text
TX pin
RX pin
UART instance
UART configuration
```

### Driver-Specific Configuration

#### SCI_B

```c
sci_b_baud_setting_t
sci_b_uart_extended_cfg_t
sci_b_uart_instance_ctrl_t
```

#### SCI

```c
baud_setting_t
sci_uart_extended_cfg_t
sci_uart_instance_ctrl_t
```

#### UARTA

```c
uarta_baud_setting_t
uarta_extended_cfg_t
uarta_instance_ctrl_t
```

---

# Configuration Validation

## Generic Validation

Implemented by:

```c
check_implementation_agnostic_config()
```

Validation includes:

### Instance Validation

```text
p_api != NULL
p_cfg != NULL
p_ctrl != NULL
```

### Ownership Validation

```text
uart_instance.p_cfg == &uart_cfg
```

### Configuration Consistency

```text
memcmp(instance cfg, local cfg) == 0
```

### Interrupt Validation

```text
RXI IPL valid
TXI IPL valid
TEI IPL valid (except UARTA)

RXI IRQ valid
TXI IRQ valid
TEI IRQ valid
```

### Pin Validation

```text
TXD != RXD
```

### DMA Validation

```text
Transfer drivers are not supported.
```

Required:

```text
p_transfer_rx == NULL
p_transfer_tx == NULL
```

### Extended Configuration Validation

```text
p_extend != NULL
```

---

## Exact Validation

Implemented by:

```c
rs_stdio_uart_check_config()
```

Board-specific implementations verify:

### Pin Assignments

Example RA8E1:

```text
RXD -> P101
TXD -> P102
```

### UART Driver Selection

Example:

```c
g_uart_on_sci_b
```

### Extended Configuration

Verifies:

```text
Clock source
Flow control
FIFO settings
RS485 settings
Delay cycles
```

### Baud Configuration

Performs exact comparison against the reference baud configuration.

### Clock Requirements

Example:

```text
HOCO = 20 MHz
SCICLK = HOCO
```

---

# Public APIs

## RS_STDIO_UART_GetConfigSize()

### Purpose

Returns the required size of the configuration object.

### Inputs

```c
uint32_t * p_size
```

### Outputs

```c
sizeof(rs_stdio_uart_cfg_t)
```

### Errors

```text
FSP assertion if pointer is NULL
```

---

## RS_STDIO_UART_GenConfig()

### Purpose

Generate a runtime UART configuration.

### Inputs

```c
rs_stdio_uart_cfg_t * p_cfg
uint32_t config_size
```

### Function Flow

1. Validate size.
2. Copy reference configuration.
3. Repair ownership:

```c
p_cfg->uart_instance.p_cfg =
    &p_cfg->uart_cfg;
```

4. Validate configuration.
5. Return generated object.

### Errors

```text
FSP_ERR_INVALID_ARGUMENT
```

when size mismatch occurs.

---

## RS_STDIO_UART_CheckConfig()

### Purpose

Validate a UART configuration.

### Function Flow

1. Validate pointer.
2. Execute generic validation.
3. Execute board-specific validation.
4. Return validation result.

---

## RS_STDIO_UART_CheckApplicationConfiguration()

### Purpose

Verify that Smart Configurator generated objects match SBB expectations.

### Function Flow

1. Build a temporary runtime config.
2. Populate:
   - UART instance
   - UART configuration
   - Extended configuration
   - Baud configuration
3. Validate through:

```c
RS_STDIO_UART_CheckConfig()
```

---

# STDIO Runtime APIs

## uart_putc()

### Purpose

Queue characters for transmission.

### Behavior

1. Initialize UART if necessary.
2. Check TX queue availability.
3. Flush if queue is full.
4. Insert character into TX queue.
5. Flush automatically on newline.

---

## uart_getc()

### Purpose

Read a character from standard input.

### Behavior

1. Initialize UART if necessary.
2. Wait until RX queue contains data.
3. Return next character.

---

## uart_flush()

### Purpose

Transmit queued output.

### Behavior

1. Wait for UART idle.
2. Allocate temporary TX buffer.
3. Drain ByteQ.
4. Invoke:

```c
uart->write()
```

5. Free memory after TX complete interrupt.

---

# UART Callback Design

## uart_jlob_vcom_callback()

Processes:

### RX Character Events

```text
UART_EVENT_RX_CHAR
```

Stores incoming data into:

```text
RX ByteQ
```

Special handling:

```text
Backspace removes previous byte.
```

### TX Complete Events

```text
UART_EVENT_TX_COMPLETE
```

Actions:

```text
Free temporary TX memory.
Mark UART idle.
```

---

# Runtime Initialization

## jlob_uart_init_once()

Provides lazy initialization.

Initialization occurs only when:

```text
stdin
stdout
stderr
```

is accessed for the first time.

### Operations

1. Select UART instance.
2. Open RX queue.
3. Open TX queue.
4. Open UART driver.
5. Register callback.
6. Mark initialized.

Protected using:

```c
FSP_CRITICAL_SECTION
```

to ensure thread-safe initialization.

---

# Data Structures

## rs_stdio_uart_cfg_t

Purpose:

```text
Persistent UART application configuration.
```

Contains:

```text
Pin routing
UART instance
Driver config
Extended driver config
Driver control block
```

---

## Byte Queues

### RX Queue

```text
256 bytes
```

Used for:

```text
UART receive buffering
```

### TX Queue

```text
256 bytes
```

Used for:

```text
printf buffering
```

---

# Reusability & Portability

This SBB is intentionally designed to be:

### MCU Portable

Supports:

```text
RA0
RA2
RA4
RA6
RA8
```

through backend-specific conditional compilation.

### UART Peripheral Portable

Supports:

```text
SCI_B UART
SCI UART
UARTA
```

through the common:

```c
rs_stdio_uart_cfg_t
```

interface.

### Board Portable

Board-specific requirements are isolated into:

```text
stdio_uart_config_raxxx_xxx.c
```

allowing multiple board implementations without changes to:

```text
syscalls_picolib_stdio_to_uart.c
rs_stdio_uart.c
```

---

# Limitations

### Unsupported Features

DMA/DTC transfers are intentionally unsupported.

Required:

```text
p_transfer_rx == NULL
p_transfer_tx == NULL
```

### Dynamic Allocation

Transmit operations currently allocate memory dynamically:

```c
malloc()
free()
```

during flush operations.

### Reference Clock Assumptions

The reference RA8E1 configuration assumes:

```text
HOCO = 20 MHz
SCICLK = HOCO
```

Baud settings must be regenerated if clock settings change.

---

# Conclusion

This SBB provides a portable architecture for integrating **picolibc standard I/O** with Renesas UART peripherals. The design cleanly separates:

1. UART configuration and validation.
2. Board-specific UART definitions.
3. Runtime STDIO redirection.
4. UART buffering and callback processing.

The result is a reusable component that allows application code to use:

```c
printf(...)
fprintf(...)
getchar(...)
scanf(...)
```

without direct awareness of the underlying UART peripheral implementation.