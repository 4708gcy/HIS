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

On first run with no admin data, the system forces admin account creation (requires API Key: `"88888888"`) before proceeding.

## Gotchas

- **Working directory matters**: The executable uses `../Data/` relative paths. Running from the wrong directory causes silent load failures or data loss on save.
- **No database**: All data lives in flat text files under `Data/`. Corruption in any file breaks the corresponding entity chain on next load.
- **Signal safety**: `emergencySave()` fires on Ctrl-C / crash to prevent data loss, but it writes files directly (no atomic rename).
- **File format**: Empty fields use `"#"` as placeholder. Nested data (prescriptions, attachments, aliases, vital signs, related IDs) are written as additional lines with prefixes like `PRESCRIPTION:`, `ATTACHMENT:`, `EXAMINATION_ITEM:`, `ALIAS:`, `VITAL_SIGNS:`, `RELATED_...:`, `MEDICATION_LINE:`. Every record's last line is `count:N` storing the ID counter.
- **No commas in text**: Input validation rejects both English and Chinese commas to prevent breaking CSV field structure.

## Architecture

### Directory Layout

- `main.cpp` — entry point, signal handlers, main event loop loading all data, running the login/menu system, then saving and cleaning up
- `Head/*.h` — all header files (data structures + class declarations)
- `Source/*.cpp` — all implementation files (one per header)
- `Data/` — persisted flat-file data (loaded at startup, saved at shutdown)
  - `UserData/{Role}ChainData/*.txt` — per-role user chains (Admin, Doctor, Nurse, Pharmacist, Patient)
  - `RecordData/RegistrationChainData/registrations.txt` — registration records
  - `RecordData/ConsultationChainData/consultations.txt` — consultation records
  - `RecordData/ExaminationChainData/examinations.txt` — examination records
  - `RecordData/HospitalizationChainData/hospitalizations.txt` — hospitalization records
  - `RecordData/HospitalizationChainData/bed_info.txt` — bed info records
  - `RecordData/MedicineChainData/medication_records.txt` — medication records
  - `RecordData/MedicineChainData/medicines.txt` — medicine inventory
  - `OperationLog/*.log` — daily operation logs
- `Document/` — project documentation and reports

### Core Data Model

All entities are **doubly-linked lists** with `prev`/`next` raw pointers and `isDeleted` for soft deletes. New nodes use head insertion. Sub-lists within records (e.g., a Consultation's prescriptions, attachments) use `std::vector<T>`.

**User hierarchy:** `User` (base class with virtual destructor) → `Admin`, `Doctor`, `Nurse`, `Pharmacist`, `Patient`. Each role class adds role-specific fields and business methods. Roles are not dispatched polymorphically — `main()` uses role-specific pointers directly.

**Medical record structs:** `Registration`, `Consultation`, `Examination`, `Hospitalization`, `MedicationRecord`, `Medicine`, `bedInfo` — plain structs with `prev`/`next` pointers and `isDeleted` for soft deletes.

**ID scheme:** 6-digit IDs where the leading digit encodes the role: 0=admin, 1=doctor, 2=nurse, 3=pharmacist, 4=patient. Record IDs use type prefixes: `reg`, `con`, `exa`, `hos`, `mrd`, `med`, `bed` followed by 6 digits.

**Global ID counters** (defined in `main.cpp`): `adminIDCount`, `doctorIDCount`, `nurseIDCount`, `pharmacistIDCount`, `patientIDCount`, `registrationCount`, `consultationCount`, `examinationCount`, `hospitalizationCount`, `medicationRecordCount`, `medicineCount`, `bedCount` — passed by reference to load/save functions and role methods.

**VitalSigns struct:** Used in `Examination` and `bedInfo` to store 15 vital sign indicators (temperature, blood pressure, heart rate, respiration rate, SpO2, height, weight, BMI, pain score, waist circumference, blood glucose, body fat, uric acid, cholesterol).

### Key Subsystems

- **Authentication** (`Login.h`, `SHA-256.h`): Salted SHA-256 with 1000 iterations. Account lockout after 5 failed attempts. Admin registration requires API Key `"88888888"`.
- **Persistence** (`LoadData.h`, `SaveData.h`): Flat text files, one entity per chain. 11 chains total (5 user + 6 business record).
- **UI** (`UI.h`): Console menus with color output, CJK-aware width calculation (`getDisplayWidth()`), info cards (`printInfoCard()`), input validation helpers, and a `LogManager` singleton for operation logging.
- **Data Analysis** (`DataAnalysis.h`): Monthly statistics, demand prediction, bed utilization analysis.
- **Emergency save**: Signal handlers (`SIGINT`, `SIGABRT`, `SIGTERM`) trigger `emergencySave()` to persist all 11 chains before exit.
- **Generic account management** (`Login.h`): Template function `AccountManageGeneric<UserType>()` handles activation/blocking for all 5 role types.

### Data Flow

1. `main()` loads all 11 chains from `Data/` via `LoadData` functions
2. User logs in → role-specific menu loop delegates to role class methods
3. All mutations operate on in-memory linked lists directly
4. On normal exit (or crash), `SaveData` functions write all chains back to files

## Language

All UI strings, data files, comments, and documentation are in Chinese (Simplified). Keep new user-facing text in Chinese. Code identifiers are in English.

## Debugging

VS Code launch config is in `.vscode/` — "调试 HIS 程序" target builds and runs `build/Debug/his.exe` in an external terminal with CMake build as pre-launch task.
