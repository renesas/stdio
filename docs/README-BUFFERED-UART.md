# UART Console (Picolibc + FSP) – User Manual

## Summary

This module provides a reusable UART-based stdio implementation featuring:

* ByteQ buffering
* Interrupt-driven communication
* Automatic initialization
* UART configuration validation
* Multi-board support
* Minimal application integration effort

The solution is suitable for:

* Debug consoles
* CLI interfaces
* Evaluation boards
* Training material
* Application demonstrations


### What Problem Does This SBB Solve?

This SBB demonstrates how to redirect standard C library console I/O (stdin, stdout, and stderr) to a Renesas FSP UART peripheral using picolibc.

Typical use cases include:

- Debug console output
- Interactive command-line interfaces (CLI)
- Application logging
- Manufacturing or service interfaces
- Evaluation board demonstrations
- Customer reference designs
- Training and educational examples

### What Does the SBB Core Code Do?

The reusable SBB code provides:

- UART-backed implementations of:
    - printf()
    - fprintf()
    - scanf()
    - getchar()
    - putchar()
- Interrupt-driven UART communication
- ByteQ-based transmit and receive buffering
- Automatic one-time initialization
- UART configuration validation
- Board-specific reference configuration validation

The SBB is intended to be reusable across multiple RA MCU families and target boards with minimal modification.

***

## Table of Contents

- [UART Console (Picolibc + FSP) – User Manual](#uart-console-picolibc--fsp--user-manual)
  - [Summary](#summary)
    - [What Problem Does This SBB Solve?](#what-problem-does-this-SBB-solve)
    - [What Does the SBB Core Code Do?](#what-does-the-SBB-core-code-do)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Demo Evaluation](#demo-evaluation)
    - [Demonstrated Behavior](#demonstrated-behavior)
      - [Demonstrated Configuration](#demonstrated-configuration)
      - [Performance Notes](#performance-notes)
    - [Demo Code Versus Reusable SBB Code](#demo-code-versus-reusable-SBB-code)
    - [Reusable SBB Components](#reusable-SBB-components)
  - [Key Features](#key-features)
  - [Dependencies](#dependencies)
    - [Hardware and Software Dependencies](#hardware-and-software-dependencies)
      - [FSP Components](#fsp-components)
      - [SBB Components](#SBB-components)
      - [Runtime Components](#runtime-components)
      - [Hardware Resources](#hardware-resources)
    - [Mandatory Components](#mandatory-components)
    - [Compile-Time Requirement](#compile-time-requirement)
  - [FSP Configuration](#fsp-configuration)
    - [Reference Board Assumptions](#reference-board-assumptions)
    - [Add UART Instance](#add-uart-instance)
    - [Configure UART Parameters](#configure-uart-parameters)
    - [Callback Configuration](#callback-configuration)
      - [Preferred](#preferred)
      - [Alternative](#alternative)
    - [Configure Heap Size](#configure-heap-size)
    - [Enable ByteQ](#enable-byteq)
    - [UART Configuration Validation](#uart-configuration-validation)
    - [DMA/DTC Restrictions](#dmadtc-restrictions)
  - [Recommended Starting Points](#recommended-starting-points)
    - [Public APIs To Review First](#public-apis-to-review-first)
      - [Validate Configuration](#validate-configuration)
      - [Initialize Through First Use](#initialize-through-first-use)
      - [Read Console Input](#read-console-input)
      - [Write Console Output](#write-console-output)
      - [Flush Output](#flush-output)
  - [Integration Steps](#integration-steps)
    - [Step 1: Add UART Sources](#step-1-add-uart-sources)
    - [Step 2: Add ByteQ Sources](#step-2-add-byteq-sources)
    - [Step 3: Configure UART in FSP](#step-3-configure-uart-in-fsp)
    - [Step 4: Configure Heap Size](#step-4-configure-heap-size)
    - [Step 5: Configure UART Routing](#step-5-configure-uart-routing)
    - [Step 6: Build with Picolibc](#step-6-build-with-picolibc)
    - [Step 7: Validate UART Configuration](#step-7-validate-uart-configuration)
    - [Step 8: No Explicit Initialization Required](#step-8-no-explicit-initialization-required)
  - [Integration Checklist](#integration-checklist)
  - [UART Configuration Architecture](#uart-configuration-architecture)
  - [Usage](#usage)
    - [Writing to Console](#writing-to-console)
    - [Reading from Console](#reading-from-console)
    - [Flush Output](#flush-output-1)
  - [Data Flow Architecture](#data-flow-architecture)
    - [TX Flow](#tx-flow)
    - [RX Flow](#rx-flow)
  - [Callback Behavior](#callback-behavior)
    - [Special Handling](#special-handling)
  - [Configuration Parameters](#configuration-parameters)
    - [Buffer Sizes](#buffer-sizes)
  - [Error Handling](#error-handling)
    - [Return Conventions](#return-conventions)
    - [Common Failure Conditions](#common-failure-conditions)
  - [Limitations](#limitations)
    - [Blocking RX](#blocking-rx)
    - [Heap Usage](#heap-usage)
    - [No Timeout Support](#no-timeout-support)
    - [Single UART Instance](#single-uart-instance)
    - [Supported Boards](#supported-boards)
  - [Recommended Best Practices](#recommended-best-practices)
    - [Production Systems](#production-systems)
    - [Debug Applications](#debug-applications)
  - [Source Modification Guidance](#source-modification-guidance)
    - [Safe To Modify](#safe-to-modify)
    - [Modify With Care](#modify-with-care)
    - [Generated Files](#generated-files)
  - [Updating To A Newer SBB Release](#updating-to-a-newer-SBB-release)
    - [Recommended Approach](#recommended-approach)
      - [Customize:](#customize)
      - [Avoid modifying:](#avoid-modifying)
    - [Benefits](#benefits)
  - [Troubleshooting](#troubleshooting)


***

## Overview

This module provides a **UART-backed stdio interface** (`stdin`, `stdout`, `stderr`) for Renesas RA MCUs using **picolibc** and the Renesas Flexible Software Package (FSP).

It enables:

* `printf()` → UART TX
* `fprintf()` → UART TX
* `scanf()` → UART RX
* `getchar()` → UART RX
* Buffered, interrupt-driven communication using ByteQ
* Automatic UART initialization
* UART configuration validation


***

## Demo Evaluation

A demonstration application (`syscalls_picolib_stdio_to_uart_demo.c`) is included to verify correct UART console integration and validate end-to-end operation.

The demo performs the following:

1. Initializes the system normally without explicit UART console initialization.
2. Prints a startup message using `printf()`.
3. Waits for characters to be received from the UART terminal.
4. Echoes received characters back to the terminal.

Example:

```c
printf("System Init\r\n");

while (1)
{
    int c = getchar();

    printf(
        "You typed: %c\r\n",
        c);
}
```

Expected terminal output:

```text
System Init
```

When the user types:

```text
A
```

The console should display:

```text
You typed: A
```

Successful operation demonstrates:

* Automatic UART console initialization
* Correct UART TX functionality
* Correct UART RX functionality
* ByteQ transmit and receive buffering
* Interrupt-driven UART communication
* Proper `printf()` and `getchar()` integration with picolibc
* Correct UART routing and baud-rate configuration

This demo is recommended as the first validation step after integrating the UART Console SBB into a new project.

### Demonstrated Behavior

The supplied demo application demonstrates:

- Successful UART-backed printf()
- Successful UART-backed getchar()
- Interrupt-driven UART operation
- Automatic initialization
- ByteQ buffering
- UART configuration validation

#### Demonstrated Configuration

The reference implementation was validated using:

```
115200 baud
8 data bits
No parity
1 stop bit
```

#### Performance Notes

The implementation is designed for interactive console workloads.

Demonstrated characteristics include:

- Buffered transmit operation
- Buffered receive operation
- Interrupt-driven communication
- Reduced application blocking during output generation

No throughput, latency, CPU utilization, or memory benchmarks are currently provided by this SBB.

***

### Demo Code Versus Reusable SBB Code

The repository contains both reusable SBB components and demonstration/reference application code.

### Reusable SBB Components

These files are intended to be integrated into customer applications:
```
syscalls_picolib_stdio_to_uart.c
rs_stdio_uart.c
rs_stdio_uart.h

configs/
├── stdio_uart_config_ra0l1.c
├── stdio_uart_config_ra2e3.c
├── stdio_uart_config_ra4e2.c
├── stdio_uart_config_ra6e2.c
└── stdio_uart_config_ra8e1.c
```

These files implement:

UART stdio redirection
UART initialization
ByteQ buffering
UART interrupt processing
Configuration validation
Demo / Reference Application Code

The following files exist solely to demonstrate SBB usage:
```
syscalls_picolib_stdio_to_uart_demo.c
```

The demo application:

Exercises the SBB APIs
Demonstrates expected behavior
Provides a known-good validation example

The demo application is not intended to be production code.

## Key Features

* Fully integrates with picolibc `FILE` streams
* Buffered UART TX and RX
* Interrupt-driven communication
* ByteQ-backed buffering
* Automatic initialization on first use
* Line-buffered output
* Explicit flush support
* UART configuration validation framework
* Board-specific validated UART reference configurations
* Cross-family support:
  * FPB-RA0L1
  * FPB-RA2E3
  * FPB-RA4E2
  * FPB-RA6E2
  * FPB-RA8E1

***

## Dependencies

### Hardware and Software Dependencies

This SBB depends upon the following components:

#### FSP Components

- UART driver
- BSP
- Interrupt manager

#### SBB Components

- ByteQ

#### Runtime Components

- picolibc
- Heap allocation (malloc())

#### Hardware Resources

- One UART peripheral
- UART TX interrupt
- UART RX interrupt
- UART pins
- Reference clock source suitable for configured baud rate

### Mandatory Components

| Component | Requirement |
|------------|-------------|
| Toolchain | LLVM ARM Embedded Toolchain with picolibc |
| FSP Modules | UART driver appropriate for device |
| ByteQ | SBB-queues ByteQ |
| BSP | Supported Renesas RA MCU |

***

### Compile-Time Requirement

```c
#if !defined (__PICOLIBC__)
#error "Use this file only with picolibc. Typically packaged with LLVM ARM Embedded Toolchain."
#endif
```

***

## FSP Configuration

### Reference Board Assumptions

The validation configuration files supplied with this SBB were created and tested using Renesas Fast Prototyping Boards (FPBs).

Reference assumptions include:

| Item             | Assumption                              |
| ---------------- | --------------------------------------- |
| UART Instance    | `g_uart_jlob_vcom`                      |
| Board            | FPB reference board                     |
| VCOM Routing     | Board-specific J-Link OB VCOM interface |
| UART Mode        | Asynchronous                            |
| DMA/DTC          | Disabled                                |
| Interrupt Driven | Enabled                                 |
| Toolchain        | LLVM Embedded Toolchain with picolibc   |
| ByteQ            | Enabled                                 |
| Heap             | Non-zero heap configured                |

The supplied validation files represent known-good reference configurations and should be used as a starting point when porting to custom hardware.

### Add UART Instance

Create a UART instance in the FSP Configurator.

The instance name must be:

```c
g_uart_jlob_vcom
```

Supported UART drivers:

| Device Family | Driver |
|---------------|---------|
| RA0L1 | UARTA |
| RA2E3 | SCI UART |
| RA4E2 | SCI UART |
| RA6E2 | SCI UART |
| RA8E1 | SCI-B UART |

UART operation is interrupt-driven.

***

### Configure UART Parameters

Recommended settings:

| Parameter | Value |
|-----------|--------|
| Mode | Asynchronous UART |
| Baud Rate | 115200 |
| Data Bits | 8 |
| Parity | None |
| Stop Bits | 1 |

***

### Callback Configuration

#### Preferred

Leave the callback unset.

The console driver automatically installs its callback during initialization.

#### Alternative

Configure:

```c
uart_jlob_vcom_callback
```

manually.

***

### Configure Heap Size

The implementation allocates temporary transmit buffers using:

```c
malloc()
```

Configure a non-zero heap size:

```text
BSP Properties
    → Heap Size
```

Recommended:

```text
1024 bytes minimum
```

Failure to configure heap memory may result in:

* Failed UART flushes
* Missing output
* `malloc()` failures

***

### Enable ByteQ

This component depends on ByteQ.

The following queues are managed internally:

* RX queue
* TX queue

No explicit FSP configuration is required.

***

### UART Configuration Validation

The UART stdio implementation contains a reusable UART configuration validation framework.

The following files are provided:

```text
rs_stdio_uart.h
rs_stdio_uart.c

configs/
├── stdio_uart_config_ra0l1.c
├── stdio_uart_config_ra2e3.c
├── stdio_uart_config_ra4e2.c
├── stdio_uart_config_ra6e2.c
└── stdio_uart_config_ra8e1.c
```

Each board-specific source file contains:

* Validated UART reference settings
* Pin mapping requirements
* Baud configuration requirements
* Driver-specific validation logic

The framework validates:

* UART driver type
* UART ownership relationships
* UART interrupt configuration
* UART extended configuration
* UART baud configuration
* UART pin routing
* DMA/DTC restrictions

***

### DMA/DTC Restrictions

This component supports interrupt-driven UART communication only.

DMA and DTC transfer instances are not supported.

The following fields must remain:

```c
.p_transfer_tx = NULL
.p_transfer_rx = NULL
```

Validation will fail if transfer instances are configured.

***

## Recommended Starting Points
### Public APIs To Review First

Customers evaluating this SBB should begin with the following public APIs:

#### Validate Configuration

```
RS_STDIO_UART_CheckApplicationConfiguration()
```

Purpose:

Verifies that the application UART configuration matches the validated reference configuration.

#### Initialize Through First Use

```
printf(...)
fprintf(...)
getchar()
```

Purpose:

Demonstrates how the SBB automatically initializes itself without explicit startup code.

#### Read Console Input

```
getchar()
```
Purpose:

Demonstrates blocking UART receive functionality.

#### Write Console Output

```
printf(...)
fprintf(...)
```
Purpose:

Demonstrates UART-backed standard I/O output.


#### Flush Output

```
fflush(stdout)
```

Purpose:

Forces transmission of queued output data.

## Integration Steps

### Step 1: Add UART Sources

Add:

```text
syscalls_picolib_stdio_to_uart.c
rs_stdio_uart.c
rs_stdio_uart.h
```

Add the configuration file corresponding to your target:

```text
configs/stdio_uart_config_ra0l1.c
```

or

```text
configs/stdio_uart_config_ra2e3.c
```

or

```text
configs/stdio_uart_config_ra4e2.c
```

or

```text
configs/stdio_uart_config_ra6e2.c
```

or

```text
configs/stdio_uart_config_ra8e1.c
```

***

### Step 2: Add ByteQ Sources

Include:

```text
r_byteq.c
r_byteq.h
```

### Step 3: Configure UART in FSP

Create a UART instance:

```c
g_uart_jlob_vcom
```

### Step 4: Configure Heap Size

Configure:

```text
BSP Properties
    → Heap Size
```

Recommended:

```text
1024 bytes or larger
```

### Step 5: Configure UART Routing

Ensure the correct VCOM routing is configured.

Reference VCOM routing:

| Board | RXD | TXD |
|---------|------|------|
| FPB-RA0L1 | P105 | P106 |
| FPB-RA2E3 | P110 | P109 |
| FPB-RA4E2 | P410 | P411 |
| FPB-RA6E2 | P410 | P411 |
| FPB-RA8E1 | P101 | P102 |

### Step 6: Build with Picolibc

Build using:

* LLVM Embedded Toolchain
* picolibc

The implementation will fail compilation if picolibc is not enabled.

***

### Step 7: Validate UART Configuration

The project may validate the configured UART during startup.

```c
fsp_err_t err =
    RS_STDIO_UART_CheckApplicationConfiguration();

FSP_ASSERT(FSP_SUCCESS == err);
```

This validates:

* UART driver type
* UART configuration
* UART pin routing
* Baud configuration
* Interrupt configuration
* Extended UART settings

***

### Step 8: No Explicit Initialization Required

The UART layer initializes automatically when one of the following is first used:

```c
printf(...)
fprintf(...)
getchar()
fflush(...)
```

Initialization occurs internally through:

```c
jlob_uart_init_once()
```

***

## Integration Checklist

Before running your application:

* [ ] LLVM Embedded Toolchain is used.
* [ ] picolibc is enabled.
* [ ] syscalls_picolib_stdio_to_uart.c included.
* [ ] rs_stdio_uart.c included.
* [ ] rs_stdio_uart.h included.
* [ ] Correct stdio_uart_config_raXYZ.c included.
* [ ] r_byteq.c included.
* [ ] r_byteq.h included.
* [ ] UART instance g_uart_jlob_vcom configured.
* [ ] UART routing matches board requirements.
* [ ] UART interrupts enabled.
* [ ] Heap size configured.
* [ ] DMA/DTC not configured.
* [ ] RS_STDIO_UART_CheckApplicationConfiguration() passes.
* [ ] UART terminal connected.

***

## UART Configuration Architecture

```text
stdio_uart_config_raXYZ.c
            │
            ▼
    rs_stdio_uart_ref
            │
            ▼
RS_STDIO_UART_CheckConfig()
            │
            ▼
RS_STDIO_UART_CheckApplicationConfiguration()
            │
            ▼
        UART Console
```

***

## Usage

### Writing to Console

```c
printf("Hello UART\n");

fprintf(stderr, "Error message\n");
```

Output is:

* Buffered through ByteQ
* Automatically flushed on:
  * newline (`'\n'`)
  * buffer full
  * explicit flush

***

### Reading from Console

```c
int c = getchar();
```

Behavior:

* Blocks until a character arrives
* Characters are sourced from the RX queue

***

### Flush Output

```c
fflush(stdout);
```

Behavior:

* TX queue content transmitted
* UART write started
* Completion handled through ISR

***

## Data Flow Architecture

### TX Flow

```text
printf()
    ↓
uart_putc()
    ↓
TX ByteQ
    ↓
uart_flush()
    ↓
malloc buffer
    ↓
UART write()
    ↓
ISR → free buffer
```

***

### RX Flow

```text
UART RX ISR
    ↓
RX ByteQ
    ↓
uart_getc()
    ↓
getchar()
```

***

## Callback Behavior

| Event | Action |
|---------|-----------|
| UART_EVENT_RX_CHAR | Add character to RX queue |
| UART_EVENT_TX_DATA_EMPTY | Release TX buffer |
| UART_EVENT_TX_COMPLETE | Clear TX busy status |

***

### Special Handling

Backspace support:

```c
R_BYTEQ_PopLast()
```

removes the most recently entered character.

***

## Configuration Parameters

### Buffer Sizes

```c
static uint8_t rx_memory_buffer[256];
static uint8_t tx_memory_buffer[256];
```

Recommended sizing:

| Parameter | Recommendation |
|------------|----------------|
| RX Buffer | ≥ expected line length |
| TX Buffer | ≥ maximum print burst |

***

## Error Handling

### Return Conventions

| Function | Return |
|-----------|---------|
| uart_putc | Character or -1 |
| uart_getc | Character or -1 |
| uart_flush | 0 or -1 |

***

### Common Failure Conditions

| Condition | Cause |
|------------|---------|
| putc returns -1 | Queue full |
| getc returns -1 | UART initialization failure |
| flush returns -1 | malloc failure |
| flush returns -1 | UART write failure |

***
## Limitations

### Blocking RX

`getchar()` blocks indefinitely.

### Heap Usage

The current implementation allocates TX buffers using:
```c
malloc()
```

during flushing.

### No Timeout Support

Infinite waits exist for:

* RX receive
* TX completion

### Single UART Instance

The implementation assumes exactly one UART instance:
```c
g_uart_jlob_vcom
```

Multiple simultaneous UART console instances are not supported.

***

### Supported Boards

Validated configurations currently exist for:
* FPB-RA0L1
* FPB-RA2E3
* FPB-RA4E2
* FPB-RA6E2
* FPB-RA8E1

***

## Recommended Best Practices

### Production Systems

Consider:

* Replacing heap allocation with static buffers
* Adding timeout support
* Adding RTOS synchronization
* Protecting shared state from ISR/task access
* Increasing buffer sizes

### Debug Applications

The default implementation works well for:

* Debug logging
* CLI interaction
* Evaluation boards
* Example projects

***

## Source Modification Guidance
### Safe To Modify

The following files are typically customer-owned:

```
Application source files
Board-specific UART configuration files
Demo applications
```

Examples:

```
syscalls_picolib_stdio_to_uart_demo.c
stdio_uart_config_my_board.c
```

### Modify With Care

The following files contain reusable SBB implementation logic:

```
syscalls_picolib_stdio_to_uart.c
rs_stdio_uart.c
rs_stdio_uart.h
```

Modifications to these files may complicate future SBB upgrades.

### Generated Files

FSP-generated files should be treated as generated content.

Examples include:

```
hal_data.c
hal_data.h
configuration.xml
```

Changes to generated files may be overwritten when regenerating FSP projects.

***

## Updating To A Newer SBB Release

Customers are encouraged to keep board-specific customizations separate from reusable SBB source files whenever possible.

### Recommended Approach

#### Customize:

```
Application code
Board-specific validation files
FSP configuration
```

#### Avoid modifying:

```
rs_stdio_uart.c
rs_stdio_uart.h
syscalls_picolib_stdio_to_uart.c
```

unless a product-specific requirement exists.

### Benefits

Keeping modifications isolated helps:

- Simplify upgrades
- Reduce merge conflicts
- Allow newer SBB releases to be adopted more easily
- Preserve access to bug fixes and enhancements
- If Core SBB Files Were Modified

When upgrading:

- Import the newer SBB release.
- Compare locally modified files against the new release.
- Re-apply only required changes.
- Re-run configuration validation.
- Re-test UART TX and RX operation.

This minimizes divergence from the maintained SBB baseline.

## Troubleshooting

| Issue | Check |
|---------|---------|
| No output | Verify UART pin routing |
| No output | Verify baud rate |
| No output | Verify terminal settings |
| getchar() blocks forever | Verify incoming UART data |
| UART validation fails | Verify board-specific config file |
| UART validation fails | Verify DMA/DTC disabled |
| UART validation fails | Verify UART interrupt configuration |
| UART validation fails | Verify UART routing matches reference board |
| Flush failure | Verify heap configuration |
| Missing interrupts | Verify IPL and IRQ settings |

***