# Market Requirements Document (MRD)

## Buffered Standard I/O over UART (picolibc + FSP UART)

***

# 1. Executive Summary

A reusable software component that provides buffered, interrupt-driven standard C input/output (`stdin`, `stdout`, `stderr`) over UART for Renesas RA microcontrollers using picolibc.

The solution allows developers to use familiar ANSI C APIs such as:

```c
printf();
fprintf();
getchar();
scanf();
fflush();
```

without requiring application-level UART transmit and receive management.

***

# 2. Market Opportunity

Debug consoles and command-line interfaces are among the most common requirements in embedded systems.

Developers frequently spend project time implementing custom UART logging, console handling, and user command interfaces. A reusable, validated stdio solution accelerates development and improves consistency across Renesas-based applications.

## 2.1 Use Cases

### 2.1.1 Embedded Debug Consoles

Runtime diagnostics, boot messages, and debug logging.

Examples:

* Industrial controllers
* Motor drives
* Power systems

### 2.1.2 Command Line Interfaces (CLI)

Interactive configuration and monitoring.

Examples:

* Network devices
* IoT gateways
* Sensor hubs

### 2.1.3 Product Evaluation and Demonstration

Rapid creation of proof-of-concept projects and customer demonstrations.

Examples:

* Evaluation kits
* Explorer projects
* Application examples

### 2.1.4 Manufacturing and Field Diagnostics

Device configuration and troubleshooting during production and deployment.

Examples:

* Factory test fixtures
* Field service terminals
* Maintenance interfaces

### 2.1.5 Educational and Training Platforms

Simple human-readable interaction for training and teaching purposes.

Examples:

* University labs
* Engineering enablement training
* Developer workshops

***

# 3. Intended Audience

The project is intended for:

* Embedded software engineers
* Application engineers
* Product demonstrators
* Evaluation kit users

Users should have:

* Familiarity with Renesas FSP
* Basic C programming knowledge
* Experience using e² studio or compatible toolchains

***

# 4. Problem Statement

Implementing a UART-based console typically requires developers to:

* Create UART interrupt handlers
* Manage transmit buffers
* Manage receive buffers
* Implement character queues
* Handle synchronization between application and interrupt context
* Integrate UART functionality with libc

As a result:

* Development effort is duplicated across projects
* Console implementations vary in quality
* Debug infrastructure becomes project-specific

The project addresses the following:

* Difficulty of integrating standard C I/O with UART
* Complexity of interrupt-driven communication
* Lack of reusable UART console infrastructure
* Need for buffered transmission and reception
* Need for portability across Renesas RA devices

***

# 5. Project Scope

The project demonstrates a reusable implementation of:

```text
stdin
stdout
stderr
```

over UART using:

* FSP UART
* ByteQ
* picolibc

The project shall:

* Automatically initialize UART on first use
* Support `printf()` output over UART
* Support `getchar()` input from UART
* Use ByteQ buffering for TX and RX paths
* Support interrupt-driven UART communication
* Support line-buffered output
* Support explicit output flushing
* Function across supported Renesas RA MCU families

***

## 5.1 Hardware Requirements

At minimum:

* FPB-RA0L1
* FPB-RA2E3

Additional RA devices may be supported.

***

## 5.2 Software Requirements

* e² studio
* FSP v6.5.0 or later
* LLVM Embedded Toolchain
* picolibc
* ByteQ (`SBB-queues`)

RTOS usage is optional.

***

## 5.3 Release Management

Deliverables shall be published through:

* Renesas Explorer GitHub
* Renesas Explorer Landing Page

***

## 5.4 Lifecycle Management

The project shall:

* Be regression tested on every FSP release
* Be validated on supported evaluation boards
* Be updated as required for toolchain or FSP changes

***

# 5.5 Functional Specifications

The project shall demonstrate the following:

## Standard C I/O Support

Ability to use:

```c
printf();
fprintf();
putchar();
getchar();
scanf();
fflush();
```

through UART.

***

## UART-Based stdin

Ability to receive characters through UART and provide them to:

```c
getchar();
scanf();
```

using ByteQ buffering.

***

## UART-Based stdout

Ability to transmit characters through UART using:

```c
printf();
fprintf();
```

with automatic buffering and interrupt-driven transmission.

***

## UART-Based stderr

Ability to route:

```c
fprintf(stderr, ...)
```

through the same UART transport.

***

## Automatic Initialization

UART and ByteQ resources shall be initialized automatically on first use.

No explicit initialization API shall be required.

***

## Buffered Transmission

Characters shall be buffered before transmission.

Transmission shall occur:

* On newline (`'\n'`)
* When buffers become full
* On explicit flush requests

***

## Interrupt-Driven Operation

UART interrupts shall:

* Receive data into RX ByteQ
* Manage TX completion
* Release transmit resources

***

## Line-Oriented Console Support

The solution shall support interactive terminal workflows including:

* Text entry
* Command line interfaces
* Diagnostic output

***

# 5.6 Out of Scope

The following are explicitly out of scope:

* Multi-UART support
* DMA acceleration
* Binary protocol transport
* Telnet support
* USB CDC transport
* Network console transport
* Persistent logging storage
* RTOS-aware thread-safe console access
* Timeout-based input APIs

***

# 6. Deliverables: Source Code

## SBB Functional Pack

The UART stdio functionality shall be delivered as reusable source.

Key requirements:

* No new public APIs are required
* Standard C stdio APIs remain the primary interface
* UART functionality remains separated from application logic
* ByteQ integration remains internal to the implementation
* The implementation remains reusable across projects

***

## AUC Application Project

The application project shall:

* Contain a fully configured `configuration.xml`
* Build directly in e² studio
* Demonstrate UART stdio operation
* Provide example console interaction

The application shall use:

```text
syscalls_picolib_stdio_to_uart.c
```

for stdio functionality.

***

## Telemetry Requirements

The UART implementation shall:

* Buffer TX and RX traffic
* Avoid polling-based communication
* Use asynchronous interrupt handling
* Minimize application overhead

***

# 7. Deliverables: Documentation

Documentation shall include:

* User guide
* Integration guide
* Build instructions
* Example walkthrough

Required topics:

* UART configuration
* ByteQ configuration
* Heap requirements
* Picolibc requirements
* Usage examples
* Troubleshooting guidance

Documentation shall also include:

* UART data-flow diagrams
* TX/RX architecture
* Integration checklist
* Reusability guidance

***

# 8. Success Criteria

The project is considered successful when:

* `printf()` successfully transmits through UART
* `getchar()` successfully receives UART characters
* `fflush()` reliably transmits buffered data
* Communication remains interrupt-driven
* Example projects run on supported boards
* Host unit tests pass
* Target unit tests pass
* Build and integration documentation enables rapid adoption

***

# 9. References

* Renesas Flexible Software Package (FSP)
* LLVM Embedded Toolchain
* picolibc
* Unity Test Framework
* ByteQ (`SBB-queues`)
* Renesas Explorer Application Use Cases (AUC)
* UART Communication Using Renesas RA Devices
