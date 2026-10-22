# Picolibc COMMS Stdio Backend User Manual

## Overview

The `syscalls_picolib_stdio_to_comms.c` module provides a **blocking standard input/output (stdio) implementation** for applications using:

* **picolibc**
* **Renesas Flexible Software Package (FSP)**
* **FSP COMMS Framework (`rm_comms`)**

The module redirects standard C library streams:

```c
stdin
stdout
stderr
```

to a COMMS instance named:

```c
g_comms_jlob_vcom
```

allowing standard C functions such as:

```c
printf()
puts()
getchar()
scanf()
```

to communicate through an FSP COMMS transport.

***

# Features

## Supported Capabilities

| Feature                    | Supported |
| -------------------------- | --------- |
| `stdin` redirection        | ✅         |
| `stdout` redirection       | ✅         |
| `stderr` redirection       | ✅         |
| Blocking reads             | ✅         |
| Blocking writes            | ✅         |
| One-time initialization    | ✅         |
| Thread-safe initialization | ✅         |
| Unit testing support       | ✅         |
| RTOS operation             | Partial   |
| Non-RTOS operation         | ✅         |

***

# Architecture

The module integrates picolibc stream handling with an FSP COMMS instance.

```text
printf()
    |
    v
stdout
    |
    v
comms_putc()
    |
    v
rm_comms_api_t::write()
    |
    v
COMMS Driver
```

Likewise for input:

```text
stdin
    |
    v
comms_getc()
    |
    v
rm_comms_api_t::read()
    |
    v
COMMS Driver
```

***

# Requirements

## Compiler

This module requires:

```c
__PICOLIBC__
```

to be defined.

Attempting to use the module without picolibc results in:

```c
#error "Use this file only with picolibc."
```

The intended toolchain is:

* LLVM ARM Embedded Toolchain
* picolibc

***

## FSP Components

The project must include:

```c
#include "rm_comms_api.h"
```

and an instantiated COMMS object:

```c
extern rm_comms_instance_t g_comms_jlob_vcom;
```

***

# Stream Mapping

The module creates custom FILE objects:

```c
g_stdin_file
g_stdout_file
```

and redirects:

```c
stdin
stdout
stderr
```

to those internal streams.

### Output Streams

Both:

```c
stdout
stderr
```

use:

```c
comms_putc()
```

### Input Stream

```c
stdin
```

uses:

```c
comms_getc()
```

***

# Initialization

## Automatic Initialization

No explicit initialization API is required.

The first call to:

```c
comms_putc()
```

or

```c
comms_getc()
```

automatically initializes the COMMS subsystem.

Internally:

```c
jlob_comms_init_once()
```

is invoked.

***

## One-Time Initialization

The initialization sequence executes only once.

Subsequent operations return immediately:

```c
if (g_stdio_comms_initialized)
{
    return FSP_SUCCESS;
}
```

***

## Initialization Activities

During initialization the module:

1. Selects the COMMS instance.
2. Verifies required pointers.
3. Opens the COMMS driver.
4. Registers a callback if required.
5. Marks the backend initialized.

***

# Blocking Behavior

This implementation is intentionally blocking.

## Write Operation

When:

```c
printf("Hello");
```

is executed:

```c
comms_putc()
```

waits until the underlying COMMS transfer completes.

```text
write()
    ↓
wait for callback
    ↓
return
```

***

## Read Operation

When:

```c
getchar();
```

is executed:

```c
comms_getc()
```

waits until a character is received.

```text
read()
    ↓
wait for callback
    ↓
return character
```

***

## Implications

### Advantages

* Simple implementation
* Deterministic behavior
* Easy to debug
* Easy to unit test

### Limitations

* Lower throughput
* CPU busy-waits while transfers are active
* Not optimized for high-bandwidth logging

***

# Callback Processing

For non-RTOS systems:

```c
BSP_CFG_RTOS == 0
```

the module registers:

```c
comms_jlob_vcom_callback()
```

***

## Transmission Complete

When the COMMS layer reports:

```c
RM_COMMS_EVENT_TX_OPERATION_COMPLETE
```

the module executes:

```c
tx_busy = false;
```

allowing the blocking transmit operation to continue.

***

## Reception Complete

When:

```c
RM_COMMS_EVENT_RX_OPERATION_COMPLETE
```

occurs:

```c
rx_busy = false;
```

allowing the receive operation to complete.

***

# Error Handling

## Initialization Errors

The module validates:

```c
p_api
p_ctrl
p_cfg
```

before use.

If any pointer is invalid:

```c
FSP_ERR_ASSERTION
```

is returned.

***

## Driver Errors

Any failure from:

```c
open()
read()
write()
callbackSet()
```

is propagated upward.

Examples:

```c
if(FSP_SUCCESS != err)
{
    return -1;
}
```

***

# Thread Safety

## Supported

Initialization is protected using:

```c
FSP_CRITICAL_SECTION_ENTER()
```

and:

```c
FSP_CRITICAL_SECTION_EXIT()
```

preventing simultaneous initialization from multiple contexts.

***

## Not Supported

Data transfers themselves are not protected.

Multiple concurrent tasks writing to:

```c
stdout
```

may serialize on:

```c
tx_busy
```

but message ordering is not guaranteed.

***

# Unit Testing

The module contains built-in support for host-side testing.

Enable:

```c
UNIT_TEST_COMMS
```

during compilation.

***

## Test COMMS Instance

Unit tests use:

```c
g_comms_jlob_vcom_fake
```

instead of:

```c
g_comms_jlob_vcom
```

***

## Resetting State

The following API is available:

```c
void picolibc_stdio_to_comms_test_reset(void);
```

It resets:

```c
tx_busy
rx_busy
g_stdio_comms_initialized
p_comms_jlob_vcom
```

and should be called from Unity test setup functions.

Example:

```c
TEST_SETUP(picolibc_stdio_to_comms)
{
    fake_comms_reset();
    picolibc_stdio_to_comms_test_reset();
}
```

***

# Example Usage

## Output

```c
printf("System initialized\n");
```

Output is transmitted through:

```c
g_comms_jlob_vcom
```

using:

```c
rm_comms_api_t::write()
```

***

## Input

```c
int c = getchar();

if ('y' == c)
{
    printf("Confirmed\n");
}
```

Input is received through:

```c
rm_comms_api_t::read()
```

***

# Known Limitations

1. Single-character transfers are used.
2. No buffering is provided.
3. No timeout mechanism is implemented.
4. Transfers are blocking.
5. No explicit flush operation is implemented.

***

# Integration Checklist

Before using this module:

* [ ] Build with picolibc.
* [ ] Include `rm_comms_api.h`.
* [ ] Instantiate `g_comms_jlob_vcom`.
* [ ] Configure an operational COMMS transport.
* [ ] Ensure callback support is enabled for bare-metal systems.
* [ ] Verify COMMS open/read/write operations function correctly.

***

