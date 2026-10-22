# Market Requirements Document (MRD)

## Simple Blocking C I/O over RM\_COMMS (picolibc + FSP COMMS)

***

# 1. Executive Summary

A reusable software component that provides simple, blocking standard C input/output (`stdin`, `stdout`, `stderr`) over the Renesas FSP RM\_COMMS framework for picolibc-based embedded applications.

The solution enables developers to use familiar standard C APIs such as:

```c
printf();
fprintf();
getchar();
scanf();
```

without implementing transport-specific communication logic.

The implementation leverages the FSP RM\_COMMS abstraction layer, allowing the same application code to operate across multiple underlying communication transports supported by RM\_COMMS.

***

# 2. Market Opportunity

Many embedded systems require a simple console interface for debugging, diagnostics, configuration, manufacturing, and field support.

While UART remains the most common console transport, many modern applications utilize communication abstractions such as RM\_COMMS to support multiple physical transports.

Providing a reusable standard I/O implementation over RM\_COMMS reduces development effort and improves portability.

## 2.1 Use Cases

### 2.1.1 Embedded Debug Consoles

Runtime logging and diagnostics.

Examples:

* Industrial controllers
* Motor drives
* Power supplies
* Building automation systems

### 2.1.2 Command-Line Configuration Interfaces

Interactive user configuration and monitoring.

Examples:

* Sensor gateways
* Energy management systems
* Wireless devices

### 2.1.3 Product Demonstrations and Evaluation Kits

Rapid deployment of examples and proof-of-concept projects.

Examples:

* Renesas Explorer projects
* Customer evaluation boards
* Training demonstrations

### 2.1.4 Manufacturing and Field Service Tools

Device provisioning and diagnostics during production and maintenance.

Examples:

* Factory test stations
* Calibration tools
* Service terminals

### 2.1.5 Transport-Agnostic Communication Interfaces

Applications designed to operate independently of a specific physical communication transport.

Examples:

* USB CDC virtual COM ports
* UART-backed COMMS transports
* Proprietary communication channels integrated through RM\_COMMS

***

# 3. Intended Audience

The project is intended for:

* Embedded software developers
* Application engineers
* Product evaluation teams
* Renesas ecosystem partners

Users should have:

* Knowledge of FSP drivers and middleware
* Familiarity with RM\_COMMS
* Familiarity with e² studio
* Basic understanding of standard C I/O

***

# 4. Problem Statement

Developers frequently need a command-line or logging interface but must often implement:

* Read and write routines
* Transport abstraction layers
* Callback registration
* Initialization handling
* Synchronization between application and communication drivers

When multiple physical transports are possible, application portability becomes difficult.

The project addresses the following:

* Repetitive implementation of stdio backends
* Coupling of application code to specific communication drivers
* Complexity of integrating stdio with RM\_COMMS
* Need for a lightweight console interface for diagnostics and configuration

***

# 5. Project Scope

The project demonstrates an implementation of:

```text
stdin
stdout
stderr
```

using:

```text
picolibc
+
RM_COMMS
```

The project shall:

* Automatically initialize RM\_COMMS on first use
* Support `printf()` output
* Support `getchar()` input
* Support interactive CLI implementations
* Use blocking communication semantics
* Support callback-based completion notification
* Function with RM\_COMMS-compatible transports

***

## 5.1 Hardware Requirements

Any Renesas RA device supporting:

* RM\_COMMS
* A compatible communication transport

Examples:

* FPB-RA0L1
* FPB-RA2E3
* FPB-RA4E2
* FPB-RA6E2

***

## 5.2 Software Requirements

* e² studio
* FSP v6.5.0 or later
* LLVM Embedded Toolchain
* picolibc
* RM\_COMMS

RTOS usage is optional.

***

## 5.3 Release Management

Deliverables shall be published through:

* Renesas Explorer GitHub
* Renesas Explorer Landing Page

***

## 5.4 Lifecycle Management

The project shall:

* Be regression tested on each FSP release
* Be maintained alongside supported RM\_COMMS revisions
* Be validated on supported evaluation boards

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
```

through RM\_COMMS.

***

## COMMS-Based stdin

Ability to receive characters through:

```c
rm_comms_api_t::read()
```

and provide them through:

```c
getchar();
scanf();
```

***

## COMMS-Based stdout

Ability to transmit characters through:

```c
rm_comms_api_t::write()
```

using:

```c
printf();
fprintf();
```

***

## COMMS-Based stderr

Ability to redirect:

```c
fprintf(stderr, ...)
```

through the same RM\_COMMS transport.

***

## Automatic Initialization

Ability to initialize RM\_COMMS automatically upon first use.

No explicit initialization API shall be required.

***

## Callback-Based Synchronization

The implementation shall utilize RM\_COMMS callbacks to determine transfer completion.

Events used:

```c
RM_COMMS_EVENT_TX_OPERATION_COMPLETE
RM_COMMS_EVENT_RX_OPERATION_COMPLETE
```

***

## Transport Agnostic Design

The implementation shall operate independently of the underlying communication transport.

The application shall interact only through:

```c
rm_comms_instance_t
```

interfaces.

***

## Blocking Operation

The implementation shall provide deterministic blocking behavior.

A call to:

```c
putchar()
```

shall return only after transmission is complete.

A call to:

```c
getchar()
```

shall return only after a character is received.

***

# 5.6 Out of Scope

The following are explicitly out of scope:

* DMA acceleration
* Buffered transmission queues
* Multiple COMMS instances
* Simultaneous multi-channel operation
* Timeout-based APIs
* Non-blocking console APIs
* Persistent data logging
* RTOS-aware console arbitration
* Terminal emulation
* Protocol framing

***

# 6. Deliverables: Source Code

## SBB Functional Pack

The core COMMS-backed stdio functionality shall be provided as reusable source code.

Key requirements:

* No new public APIs shall be introduced
* Standard C stdio APIs remain the primary interface
* RM\_COMMS integration shall remain internal
* The implementation shall be transport agnostic
* The implementation shall remain reusable across applications

***

## AUC Application Project

The project shall:

* Demonstrate COMMS-backed standard I/O
* Build directly in e² studio
* Contain a complete `configuration.xml`
* Be fully operational upon import

The application shall utilize:

```text
syscalls_picolib_stdio_to_comms.c
```

for console operations.

***

## Telemetry Requirements

The implementation shall:

* Use callback-driven transfer completion
* Avoid application polling
* Minimize integration complexity
* Support simple CLI-style communication

***

# 7. Deliverables: Documentation

Documentation shall include:

* User guide
* Integration guide
* Build instructions
* Example usage

Required topics:

* RM\_COMMS configuration
* FSP configuration
* Transport selection
* Usage examples
* Integration checklist
* Troubleshooting guidance

Documentation should additionally include:

* COMMS flow diagrams
* Data flow architecture
* Reusability guidance
* Transport abstraction discussion

***

# 8. Success Criteria

The project is considered successful when:

* `printf()` successfully transmits through RM\_COMMS
* `getchar()` successfully receives through RM\_COMMS
* Console interaction functions correctly
* Initialization occurs automatically
* Communication is transport independent
* Example projects build successfully
* Host unit tests pass
* Target unit tests pass
* Developers can integrate the solution with minimal effort

***

# 9. References

* Renesas Flexible Software Package (FSP)
* RM\_COMMS Framework Documentation
* LLVM Embedded Toolchain
* picolibc
* Unity Test Framework
* Renesas Explorer Application Use Cases (AUC)
* Renesas RA Family User Manuals
* FSP Communication Middleware Documentation
