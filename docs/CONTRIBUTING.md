# 📘 Developer Contribution Guide

This repository provides reusable implementations of **picolibc-backed standard I/O** for Renesas RA devices, including:

* ✅ UART-backed stdio (`syscalls_picolib_stdio_to_uart.c`)
* ✅ COMMS-backed stdio (`syscalls_picolib_stdio_to_comms.c`)
* ✅ Host-based unit tests (Unity + fakes)
* ✅ Target-based unit tests (Unity over COMMS)
* ✅ Meson-based build system
* ✅ Reusable queue library (`SBB-queues`)
* ✅ e² studio integration examples

This guide describes how to contribute in two primary areas:

1. ➕ Adding new example projects
2. 🔧 Adding new syscall implementations

## ✅ One-line Summary

> Contributors should make changes in `src/`, validate them using host-side and target-side tests, synchronize example projects, and confirm successful hardware execution before submission.

***

# 🧭 Repository Model (Mental Model)

```text
src/         → source of truth (shared implementations)
tests/       → host and target validation
examples/    → target integration projects
docs/        → user and developer documentation
subprojects/ → external reusable dependencies
```

> **Key principle:**  
> Source code is developed in `src/`, validated in `tests/`, documented in `docs/`, and then propagated into example projects.

***

# ✅ Prerequisites

Before contributing:

* Meson
* Ninja
* Python 3.x
* LLVM Embedded Toolchain
* Visual Studio (for host testing)
* e² studio (recommended for example validation)

***

# 🔄 Standard Developer Workflow

1. Modify source files under `src/`
2. Run host unit tests
3. Run developer workflow automation
4. Perform target build
5. Validate on hardware
6. Update documentation as required

Recommended workflow:

```bash
meson test -C build-msvc --print-errorlogs
python scripts/dev_flow.py
meson compile -C build-ra0l1
```

***

# 🧪 Adding a New Syscall Implementation

This applies when introducing a new backend such as:

* UART
* COMMS
* USB CDC
* Semihosting
* RTT
* Custom transports

***

## ✅ Step 1 — Add Source Files

Create:

```text
src/syscalls_picolib_stdio_to_<feature>.c
```

Optional:

```text
src/syscalls_picolib_stdio_to_<feature>_demo.c
```

Example:

```text
src/syscalls_picolib_stdio_to_usb.c
```

***

## ✅ Step 2 — Design Guidelines

### Required

Implement support for:

```c
putc()
getc()
```

and where appropriate:

```c
flush()
```

The implementation should:

* Be usable with picolibc
* Isolate FSP-specific dependencies
* Support host-side testing

***

### Strongly Recommended

Provide test support:

```c
void picolibc_stdio_to_<feature>_test_reset(void);
```

Support:

```c
UNIT_TEST_<FEATURE>
```

for host-side testing.

***

### Avoid

* Hidden state that cannot be reset
* Hardware access outside abstraction points
* Direct coupling to Unity

Use:

```c
UNIT_TEST_<FEATURE>
```

instead of:

```c
UNITY_FRAMEWORK_H
```

***

## ✅ Step 3 — Add Host Test Infrastructure

Create:

```text
tests/test_picolib_stdio_to_<feature>.c
```

If required:

```text
tests/stubs/<feature>/
```

containing:

```text
fake_<feature>.c
fake_<feature>.h
```

and any API stubs required for host execution.

Example:

```text
tests/stubs/comms/
├── fake_comms.c
├── fake_comms.h
└── rm_comms_api.h
```

***

## ✅ Step 4 — Update Meson

Update:

```text
tests/meson.build
```

Add:

* New test executable
* New fake implementation
* New test source

Where appropriate define:

```meson
-DUNIT_TEST_<FEATURE>
```

***

## ✅ Step 5 — Run Host Tests

```bash
meson test -C build-msvc --print-errorlogs
```

All tests must pass.

***

## ✅ Step 6 — Add Documentation

Create:

```text
docs/README-<FEATURE>.md
```

Document:

* Configuration
* Integration
* Limitations
* Usage examples
* Troubleshooting

***

## ✅ Step 7 — Integrate Example Projects

Synchronize source files:

```bash
python scripts/dev_flow.py
```

Ensure examples contain updated implementations.

***

## ✅ Step 8 — Hardware Validation

Validate:

* Cross build success
* Flashing success
* Runtime behavior

on actual hardware.

***

# ➕ Adding a New Example Project

Example projects are used for:

* Hardware validation
* Demonstrations
* Customer reference implementations

***

## ✅ Step 1 — Create e² studio Project

Under:

```text
examples/e2studio/
```

create:

```text
<example>_on_<board>_<mcu>
```

Examples:

```text
stdio_uart_on_fpb_ra0l1
stdio_uart_on_fpb_ra2e3
stdio_comms_on_fpb_ra0l1
```

***

## ✅ Step 2 — Required Contents

Each example should contain:

```text
ra/
ra_cfg/
ra_gen/
src/
script/
```

including:

```text
script/fsp.lld
```

***

## ✅ Step 3 — Add meson.build

Every example must supply:

```text
meson.build
```

Use an existing project as a template.

Keep naming consistent.

***

## ✅ Step 4 — Include Shared Logic

Use dependencies provided by the root project.

Examples should build against:

* UART stdio implementation
* COMMS stdio implementation
* ByteQ as required

***

## ✅ Step 5 — Synchronize Sources

Run:

```bash
python scripts/dev_flow.py
```

to copy shared source files into example projects.

This is intentional because e² studio projects require physical source files.

***

## ✅ Step 6 — Validate Build

Example:

```bash
meson setup build-ra0l1 \
  --cross-file toolchains/llvm-cortex-m23.ini \
  -Dmcu=ra0l1 \
  -Dboard=fpb \
  -Dexample=stdio_uart

meson compile -C build-ra0l1
```

***

# 🧱 Coding Guidelines

## ✅ General

Use:

* C11 preferred
* C99 acceptable

Follow naming conventions:

```c
uart_*
comms_*
jlob_*
```

Keep implementations focused and testable.

***

## ✅ Error Handling

Prefer:

```c
if (NULL == ptr)
{
    return FSP_ERR_ASSERTION;
}
```

over relying solely on assertions.

Runtime validation should remain functional in release builds.

***

## ✅ Assertions

Assertions should detect programmer errors.

Example:

```c
FSP_ASSERT(NULL != ptr);
```

Runtime validation should still exist:

```c
if (NULL == ptr)
{
    return FSP_ERR_ASSERTION;
}
```

***

## ✅ Testability

All modules should:

* Support host-side execution
* Provide reset hooks
* Provide fake implementations where required
* Avoid persistent hidden state

***

# 🧪 Testing Philosophy

## ✅ Host Testing (Primary)

Host testing is the preferred first stage of validation.

Examples:

```text
fake_uart.c
fake_comms.c
```

Used with:

```text
Unity
MSVC
Meson
```

Goal:

> Validate implementation logic independent of hardware.

***

## ✅ Target Testing (Secondary)

Target-side tests validate:

* Driver integration
* Interrupt behavior
* Real peripheral operation
* COMMS/UART transport integration

Implemented using:

```text
tests/target/
```

and:

```text
run_ut.robot
```

***

## ✅ Example Validation (Final)

Examples must:

* Compile successfully
* Execute correctly on hardware
* Remain synchronized with source implementations

***

# 🔁 Subprojects

Located under:

```text
subprojects/
```

| Component | Purpose |
|------------|----------|
| cmsis | ARM core support |
| fsp | Renesas FSP |
| SBB-queues | ByteQ / StrQ |
| unity | Unit testing |

***

## Rules

* Do not modify subprojects unless absolutely necessary.
* Prefer changes in wrapper code or integration layers.
* Keep dependencies reusable.

***

# ✅ Documentation Requirements

Documentation updates are required when:

* New syscall backends are added
* Public behavior changes
* Configuration requirements change
* New examples are introduced

Update:

```text
docs/
README.md
BUILD.md
CONTRIBUTING.md
```

as appropriate.

***

# ✅ Pull Request Checklist

Before submitting:

- [ ] Host unit tests pass
- [ ] Target unit tests pass (if applicable)
- [ ] Cross build succeeds
- [ ] Example projects build successfully
- [ ] `dev_flow.py` completes successfully
- [ ] Documentation updated
- [ ] No new warnings introduced
- [ ] New code includes tests
- [ ] Example synchronization completed

***

## Final Reminder

The preferred contribution flow is:

```text
Modify source
        ↓
Run host tests
        ↓
Run dev_flow.py
        ↓
Cross build
        ↓
Hardware validation
        ↓
Update documentation
        ↓
Submit changes
```

This ensures correctness, maintainability, and consistency across source code, tests, and example projects.

***