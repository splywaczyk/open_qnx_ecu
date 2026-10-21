<!--
  ~ ------------------------------------
  ~ Copyright (c) 2026 Seweryn Plywaczyk
  ~
  ~ SPDX-License-Identifier: Apache-2.0
  ~ ------------------------------------
  -->

# Open-QNX-ECU: Mixed-Criticality SDV Reference Architecture

![Build System](https://img.shields.io/badge/Build-Bazel_(Bzlmod)-43A047?logo=bazel)
![Target OS](https://img.shields.io/badge/RTOS-QNX_Neutrino-000000)
![Language](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus)
![Architecture](https://img.shields.io/badge/Target-x86__64_%7C_aarch64-blue)
![License](https://img.shields.io/badge/License-Apache_2.0-orange)

A reference platform and build infrastructure demonstrating a **Mixed-Criticality Electronic Control Unit (ECU)** for Software-Defined Vehicles (SDV).

This project showcases how to bridge modern, hermetic build engineering (**Bazel Bzlmod**) with microkernel operating system internals (**QNX Neutrino RTOS**), Type-1 embedded virtualization (**QNX Hypervisor**), architectural functional safety patterns (**ISO 26262 Freedom From Interference**), and mandatory access control (**QNX `secpol`**).

---

## 1. Architectural Overview

Modern automotive domain controllers consolidate safety-critical functions (e.g., vehicle dynamics, instrument cluster, Watchdog) and non-safety Quality Management (QM) workloads (e.g., Infotainment, cloud connectivity) onto a single System-on-Chip (SoC).

`Open-QNX-ECU` models this separation using hardware-assisted virtualization, deterministic CPU partitioning, and strict inter-process/inter-VM communication boundaries.

```text
+-----------------------------------------------------------------------------------+
|                            HARDWARE / QEMU (x86_64 / ARM64)                       |
+-----------------------------------------------------------------------------------+
                                          |
+-----------------------------------------------------------------------------------+
|                     HOST OS: QNX Neutrino RTOS (Safety Domain)                    |
|                     [APS Partition: Safety_Core | Guaranteed 40% CPU]             |
|                                                                                   |
|  +-----------------------+  +------------------------+  +----------------------+  |
|  |   ECU Resource Mgr    |  |  System Watchdog &     |  |  Security Policy     |  |
|  |  (/dev/ecu/telemetry) |  |  Safe-State Controller |  |  (secpol MAC Rules)  |  |
|  +-----------+-----------+  +-----------+------------+  +----------------------+  |
|              |                          |                                         |
|              | QNX Native IPC           | Heartbeat & Lifecycle Control           |
|              | (MsgSend / Pulses)       v                                         |
|              |              +-----------------------+                             |
|              +------------->|  QNX Hypervisor (qvm) |                             |
|                             +-----------------------+                             |
+-----------------------------+-----------+-----------+-----------------------------+
                                          |
                     vdev-shmem           |  Spatial & Temporal Isolation
               (Zero-Copy Shared Mem)     |  [APS Partition: Guest_QM | Max 60% CPU]
                                          v
+-----------------------------------------------------------------------------------+
|                        GUEST OS: Minimal Linux (QM Domain)                        |
|                                                                                   |
|    * Alpine Linux (Kernel + initramfs)                                              |
|    * Reads telemetry frames from shared physical memory region                      |
|    * Simulates non-critical IVI / Telematics consumer                               |
+-----------------------------------------------------------------------------------+
```

---

## 2. Core Technical Pillars

### Hermetic Build & System Integration (`Bazel Bzlmod`)

* **Zero-IDE Dependency:** Built entirely from the command line using modern Bazel (`MODULE.bazel`) without relying on Eclipse/Momentics project files.
* **Custom C++ Toolchain:** Full Starlark `cc_toolchain_config` mapping the `qcc` / `ntox86_64-gcc` and `ntoarm64-gcc` compiler suites, sysroots, and linker flags.
* **Declarative Image Generation:** Custom Bazel rules automating QNX Image File System (`mkifs`) compilation, security policy compilation (`secpolcompile`), and cryptographic image signing.
* **Supply Chain Readiness:** Structured dependency graph ready for automated Software Bill of Materials (SBOM) extraction and license compliance auditing.

### Idiomatic QNX Microkernel Internals

* **POSIX Resource Manager:** Implements a native QNX Resource Manager exposing `/dev/ecu/telemetry`. Clients interact via standard POSIX file descriptors (`open()`, `read()`, `write()`), while the server processes requests through `dispatch_create()` and `resmgr_attach()`.
* **Deterministic IPC:** Uses synchronous message passing (`MsgSendv` / `MsgReceivev` / `MsgReplyv`) with priority inheritance to prevent priority inversion, paired with non-blocking 4-byte `Pulses` for high-frequency hardware timer ticks.

### Functional Safety Patterns (ISO 26262 Concepts)

While safety certification requires target-specific hardware and process audits, this repository implements core **Safety-by-Architecture** mechanisms required to achieve **Freedom From Interference (FFI)**:

* **Spatial Isolation:** Memory protection enforced via MMU between microkernel processes and Stage-2 page tables (EPT/NPT) for the Guest VM.
* **Temporal Isolation:** Configured via the **QNX Adaptive Partitioning Scheduler (APS)**. Even if the Linux Guest OS experiences a runaway CPU loop (100% load) or kernel panic, the QNX Safety Partition retains its guaranteed CPU budget.
* **Health Monitoring & Safe State:** A dedicated Watchdog daemon monitors both internal Resource Managers and Guest VM heartbeats, triggering an automated transition to a deterministic `SAFE_STATE` upon timeout.

### Cybersecurity Hardening (`secpol` & Integrity Verification)

* **Mandatory Access Control (MAC):** Eliminates omnipotent `root` (UID 0) privileges using QNX Security Policies (`secpol`). Processes are confined to explicit security types (`ecu_t`, `watchdog_t`, `qvm_t`) and granted only the minimum required kernel abilities (`procmgr_ability`).
* **Chain of Trust Simulation:** Before spawning the `qvm` hypervisor instance, the Host integrity loader verifies the SHA-256 cryptographic signature of the Guest OS kernel image against an embedded public key embedded inside the read-only IFS.

---

## 3. Project Roadmap & Milestones

The project is developed iteratively through self-contained milestones:

* [ ] **Milestone 1: Hermetic Bazel Toolchain & Minimal Boot**
    * Custom `Bzlmod` toolchain configuration for QNX SDP.
    * Custom Starlark rule wrapping `mkifs` to build a bootable `.ifs` image.
    * Automated boot and serial console output in QEMU (`x86_64`).
* [ ] **Milestone 2: Native QNX IPC & Resource Manager**
    * RAII C++17 wrapper around QNX dispatch and channel APIs.
    * Implementation of `/dev/ecu/telemetry` handling POSIX I/O and periodic timer `Pulses`.
    * System logging integration via QNX `slogger2`.
* [ ] **Milestone 3: QNX Hypervisor (`qvm`) & Mixed-Criticality Setup**
    * Integration of the `qvm` hypervisor into the Host IFS.
    * Configuration of a minimal Linux Guest VM (`qvmconf`) booting from an `initramfs`.
    * Inter-domain communication via virtual shared memory (`vdev-shmem`).
* [ ] **Milestone 4: Safety Isolation (APS) & Watchdog**
    * Configuration of QNX Adaptive Partitioning Scheduler (APS) budgets in the `.build` script.
    * Watchdog daemon supervising Guest VM heartbeat and enforcing Safe-State transitions.
* [ ] **Milestone 5: Security Hardening (`secpol` & Image Signing)**
    * Authoring and compiling `secpol` mandatory access control rules.
    * Build-time signing of Guest VM artifacts in Bazel and runtime signature verification.
* [ ] **Milestone 6: SBOM, SPDX support**
    * Add autoggeneration of SBOM using SPDX format

---

## 4. Repository Structure

```text
open-qnx-ecu/
├── MODULE.bazel                 # Bazel Bzlmod dependency and toolchain registration
├── .bazelrc                     # Build configurations (host, qnx_x86_64, qnx_aarch64)
├── .bazelversion                # Specifies a Bazel version used in project
├── bazel/
│   ├── platforms/               # Target platform definitions (OS & CPU constraints)
│   ├── toolchains/              # Starlark cc_toolchain_config for QNX qcc
│   └── rules/                   # Custom Bazel rules: qnx_ifs.bzl, qnx_secpol.bzl
├── src/
│   ├── common/                  # Shared telemetry data structures & RAII wrappers
│   ├── ecu_resmgr/              # Core Resource Manager (/dev/ecu/telemetry)
│   ├── watchdog/                # Temporal supervisor & Safe-State state machine
│   └── integrity_loader/        # Pre-launch signature verification for Guest VM
├── ifs/
│   ├── ecu_host.build           # QNX OS Image File System (IFS) build specification
│   └── BUILD                    # Targets for image assembly and QEMU execution
├── hypervisor/
│   ├── linux_guest.qvmconf      # QNX Hypervisor VM configuration
│   └── guest_rootfs/            # Minimal Linux kernel & initramfs build scripts
└── security/
    ├── ecu_policy.secpol        # QNX Mandatory Access Control (secpol) source policy
    └── keys/                    # Development keypair for artifact signing (DO NOT USE IN PROD)
```

---

## 5. Prerequisites & Quick Start

### Prerequisites (Bring Your Own License)
Due to commercial licensing restrictions, **this repository does not distribute any proprietary QNX binaries, headers, or compilers**. To build and run this project, you must have:
1. A valid **QNX Software Development Platform (SDP 8.0)** installation (Commercial or Non-Commercial/Evaluation license).
2. **Bazelisk** (or Bazel >= 8.0).
3. **QEMU** (`qemu-system-x86_64` / `qemu-system-aarch64`) for local emulation.

---

## 6. Legal & Licensing Disclaimer

* **Source Code License:** All original C++ source code, Starlark build rules, scripts, and architectural documentation in this repository are licensed under the **Apache License, Version 2.0** (see [LICENSE](LICENSE)).
* **No Proprietary Artifacts:** This repository is a clean-room engineering implementation. It contains zero proprietary code, binaries, or SDK files from BlackBerry QNX. Building the project requires the user to supply their own legally obtained QNX SDP environment on the host machine.
* **Trademarks:** QNX, Neutrino, and BlackBerry are registered trademarks of BlackBerry Limited. Linux is a registered trademark of Linus Torvalds. ISO is a registered trademark of the International Organization for Standardization. This project is an independent educational and architectural showcase and is neither affiliated with nor endorsed by BlackBerry Limited or any standards organization.
