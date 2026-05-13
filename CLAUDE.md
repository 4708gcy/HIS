# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Hospital Information System (HIS) — a C++17 console application for a university course design project. It manages users (admin, doctor, nurse, pharmacist, patient) and medical records (registration, consultation, examination, hospitalization, medication, medicine inventory, beds) via a terminal UI with Chinese text.

## Build Commands

Build requires CMake 3.24+ and MSVC (Windows). The working directory is `build/`.

```bash
# Configure (run once or when CMakeLists.txt changes)
cd build && cmake ..

# Build the console app
cmake --build build --config Debug --target his
```

MSVC `/utf-8` flag is set in CMakeLists.txt to handle Chinese source and data files.

### Run

```bash
# Must run from build/ as working directory (Data files use ../Data/... relative paths)
cd build && ./Debug/his.exe
```

On first run with no data files, the system forces admin account creation before proceeding.

## Gotchas

- **Working directory matters**: The executable uses `../Data/` relative paths. Running from the wrong directory causes silent load failures or data loss on save.
- **No database**: All data lives in flat text files under `Data/`. Corruption in any file breaks the corresponding entity chain on next load.
- **Signal safety**: `emergencySave()` fires on Ctrl-C / crash to prevent data loss, but it writes files directly (no atomic rename).

## Architecture

### Directory Layout

- `main.cpp` — entry point, signal handlers, main event loop loading all data, running the login/menu system, then saving and cleaning up
- `Head/*.h` — all header files (data structures + class declarations)
- `Source/*.cpp` — all implementation files (one per header)
- `Data/` — persisted flat-file data (loaded at startup, saved at shutdown)
  - `UserData/{Role}ChainData/*.txt` — per-role user chains
  - `RecordData/{RecordType}ChainData/*.txt` — per-type medical record chains
  - `OperationLog/*.log` — daily operation logs
- `Document/` — project documentation and reports

### Core Data Model

All entities are **doubly-linked lists** with `prev`/`next` raw pointers. Sub-lists within records (e.g., a Consultation's prescriptions, attachments) use `std::vector<T>`.

**User hierarchy:** `User` (base class with virtual destructor) → `Admin`, `Doctor`, `Nurse`, `Pharmacist`, `Patient`. Each role class adds role-specific fields and business methods. Roles are not dispatched polymorphically — `main()` uses role-specific pointers directly.

**Medical record structs:** `Registration`, `Consultation`, `Examination`, `Hospitalization`, `MedicationRecord`, `Medicine`, `bedInfo` — plain structs with `prev`/`next` pointers and `isDeleted` for soft deletes.

**ID scheme:** 6-digit IDs where the leading digit encodes the role: 0=admin, 1=doctor, 2=nurse, 3=pharmacist, 4=patient. Record IDs have their own prefix conventions.

### Key Subsystems

- **Authentication** (`Login.h`, `SHA-256.h`): Salted SHA-256 with configurable iterations. Account lockout after 5 failed attempts.
- **Persistence** (`LoadData.h`, `SaveData.h`): Flat text files, one entity per chain. Global counters in `main.cpp` track ID generation.
- **UI** (`UI.h`): Console menus with color output, CJK-aware width calculation, input validation helpers, and a `LogManager` singleton for operation logging.
- **Data Analysis** (`DataAnalysis.h`): Monthly statistics, demand prediction (moving average + linear trend), bed utilization analysis, and reallocation suggestions.
- **Emergency save**: Signal handlers (`SIGINT`, `SIGABRT`, `SIGTERM`) trigger `emergencySave()` to persist all data before exit.

### Data Flow

1. `main()` loads all chains from `Data/` via `LoadData` functions
2. User logs in → role-specific menu loop delegates to role class methods
3. All mutations operate on in-memory linked lists directly
4. On normal exit (or crash), `SaveData` functions write all chains back to files

## Language

All UI strings, data files, comments, and documentation are in Chinese (Simplified). Keep new user-facing text in Chinese. Code identifiers are in English.

## Debugging

VS Code launch config is in `.vscode/` — "调试 HIS 程序" target builds and runs `build/Debug/his.exe` in an external terminal with CMake build as pre-launch task.
