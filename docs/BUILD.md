# Recommended Workflow

1. Develop and validate logic using **host-based unit tests**
2. Execute the developer workflow automation script
3. Perform a cross-build to generate firmware
4. Flash and validate on hardware

***

# 🧪 Host Build (MSVC) + Unit Tests

Host testing provides rapid validation of both supported stdio backends:

* UART-backed stdio (`syscalls_picolib_stdio_to_uart.c`)
* COMMS-backed stdio (`syscalls_picolib_stdio_to_comms.c`)

No target hardware is required.

***

## Prerequisites

* Visual Studio (Desktop C++ workload)
* Meson
* Ninja
* Python 3.x

***

## Step 1: Open Developer Command Prompt

Use:

```text
x64 Native Tools Command Prompt for VS
```

***

## Step 2: Configure Native Build

```bash
meson setup --wipe build-msvc \
  --buildtype=debug
```

> No cross file is required for native builds.

***

## Step 3: Build

```bash
meson compile -C build-msvc
```

***

## Step 4: Run Unit Tests

```bash
meson test -C build-msvc --print-errorlogs
```

***

## Test Coverage

The native build validates:

### UART Backend

```text
syscalls_picolib_stdio_to_uart.c
```

Coverage includes:

* Automatic initialization
* RX path
* TX path
* Flush operations
* ByteQ integration
* UART callback handling
* Error paths

Using:

```text
fake_uart.c
```

***

### COMMS Backend

```text
syscalls_picolib_stdio_to_comms.c
```

Coverage includes:

* Automatic initialization
* RX path
* TX path
* Callback handling
* COMMS open/read/write operations
* Error handling

Using:

```text
fake_comms.c
```

***

## Test Environment

| Component | UART Tests | COMMS Tests |
|------------|------------|------------|
| UART Driver | Fake UART | N/A |
| COMMS Driver | N/A | Fake COMMS |
| FSP | Stubbed | Stubbed |
| ByteQ | Host Dependency | Not Required |
| libc | Host | Host |

***

## Developer Workflow Automation

After modifying source code, run:

```bash
meson compile -C build-msvc dev_flow
```

or:

```bash
python scripts/dev_flow.py
```

The workflow:

1. Runs host unit tests
2. Verifies test success
3. Synchronizes shared source files into e² studio examples
4. Helps keep example projects aligned with the source tree

***

# 🔁 Switching Between Builds

| Build Type | Command |
|------------|---------|
| Host (MSVC) | `meson setup build-msvc` |
| Cross (Target) | `meson setup build-ra0l1 --cross-file=...` |

> Always use separate build directories for host and target builds.

***

## Key Differences

| Feature | Host Build | Cross Build |
|----------|-----------|------------|
| Compiler | MSVC (`cl.exe`) | LLVM Clang ARM |
| Execution | PC | MCU |
| UART | Fake UART | Real FSP UART |
| COMMS | Fake COMMS | Real FSP COMMS |
| Output | Test Executables | `.elf`, `.srec` |
| Purpose | Verification | Deployment |

***

# 🚀 Cross Build Instructions (LLVM Embedded Toolchain)

This project uses Meson to build firmware for Renesas RA devices using:

* LLVM Embedded Toolchain
* picolibc
* e² studio generated sources

***

## Prerequisites

* LLVM Embedded Toolchain installed
* Meson
* Ninja
* Valid cross file
* Example project present under:

```text
examples/e2studio/
```

***

## Supported Examples

Examples are resolved using:

```text
<example>_on_<board>_<mcu>
```

Examples:

```text
stdio_uart_on_fpb_ra0l1
stdio_uart_on_fpb_ra2e3
```

Additional examples may be added as the project grows.

***

## Step 1: Configure Build

Example:

```bash
meson setup --wipe build-ra0l1 \
  --cross-file toolchains/llvm-cortex-m23.ini \
  -Dmcu=ra0l1 \
  -Dboard=fpb \
  -Dide=e2studio \
  -Dexample=stdio_uart
```

***

## Option Description

| Option | Description |
|----------|------------|
| `--wipe` | Removes previous build configuration |
| `--cross-file` | Selects compiler and linker settings |
| `-Dmcu=ra0l1` | Selects MCU |
| `-Dboard=fpb` | Selects board |
| `-Dide=e2studio` | Selects example project format |
| `-Dexample=stdio_uart` | Resolves example project path |

Resolved example:

```text
examples/e2studio/stdio_uart_on_fpb_ra0l1
```

***

## Step 2: Build Firmware

```bash
meson compile -C build-ra0l1
```

***

## Step 3: Generated Artifacts

| Artifact | Description |
|-----------|------------|
| `.elf` | Debuggable firmware image |
| `.srec` | Flashable image |
| `.map` | Linker memory map |

***

## Example Build Output

```text
build-ra0l1/
└── examples/
    └── e2studio/
        └── stdio_uart_on_fpb_ra0l1/
```

***

## Step 4: Flash Target

Use one of:

* Renesas Flash Programmer (RFP)
* J-Link
* e² studio debugger

***

## Recommended Validation Sequence

For every source modification:

```text
Edit source
    ↓
Host build
    ↓
Host unit tests
    ↓
Run dev_flow.py
    ↓
Cross build
    ↓
Flash target
    ↓
Hardware validation
```

This workflow provides rapid feedback while ensuring both host and target behavior remain correct.

***