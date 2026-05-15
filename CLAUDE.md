# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Quick Start

```bash
# Configure (run once, or when CMakeLists.txt changes)
cd build && cmake ..

# Build
cmake --build build --config Debug --target his

# Run — MUST run from build/ (data files use ../Data/ relative paths)
cd build && ./Debug/his.exe
```

First run without admin data forces admin creation (requires API Key: `88888888`).

## Gotchas

- **Working directory is critical.** All data file paths (`User.h` macros like `ADMIN_FILE`, `REGISTRATION_FILE`, etc.) are relative `../Data/...` — the executable MUST run from `build/`. Running from the wrong directory causes silent load failures or data loss on save.
- **VS Code `postDebugTask` destroys the build directory.** `launch.json` runs `Remove-Item -Recurse -Force build/*` after every debug session. If you need to preserve build artifacts, remove or comment out `postDebugTask`.
- **Emergency save uses double pointers.** Global pointers like `static Admin **g_adminHead` point to the local `adminHead` variable's address, not its value. This ensures that after head-insertion changes the local head, `emergencySave()` still dereferences the latest head. If you change this pattern, crash recovery will silently lose newly inserted nodes.
- **No database — all data is flat text files.** Corruption in any file breaks the corresponding entity chain on next load.
- **Signal safety.** `emergencySave()` writes files directly (no atomic rename). Ctrl+C during a write can produce truncated files.
- **No commas in text input.** Validation rejects both `,` and `，` to prevent breaking CSV field structure in data files.
- **`#` is the empty-field sentinel.** All text fields default to `"#"`, and empty user input is saved as `"#"`. When parsing loaded data, treat `"#"` as empty/null.
- **`count:N` is the last line of every data file.** Load functions read this to set the global ID counter so new IDs don't collide with existing ones.
- **No test suite.** There are no tests. Verify changes by building and running the program interactively.

## Project Overview

HIS — Hospital Information System. A C++17 console application for a university course design project. No external dependencies beyond the C++ standard library. Manages 5 user roles (admin, doctor, nurse, pharmacist, patient) and 7 business record types (registration, consultation, examination, hospitalization, medication record, medicine inventory, beds) via a Chinese-language terminal UI.

## Architecture

### Directory Layout

```
main.cpp              — entry point, signal handlers, main loop (load → login/menu → save/cleanup)
Head/*.h              — 19 header files (data structures, class declarations, enums)
Source/*.cpp          — 13 implementation files (one per major header)
Data/                 — persisted flat-file data (loaded at startup, saved at shutdown)
  UserData/{Role}ChainData/*.txt   — 5 per-role user chains
  RecordData/{Type}ChainData/*.txt — 7 business record chains
  OperationLog/*.log               — daily operation logs (format: his_YYYY_MM_DD.log)
Document/             — project documentation, reports, and development log
```

### Data File Paths (defined in `User.h`)

All paths are relative to the working directory (must be `build/`):

```
../Data/UserData/AdminChainData/admin_users.txt
../Data/UserData/DoctorChainData/doctor_users.txt
../Data/UserData/NurseChainData/nurse_users.txt
../Data/UserData/PharmacistChainData/pharmacist_users.txt
../Data/UserData/PatientChainData/patient_users.txt
../Data/RecordData/RegistrationChainData/registrations.txt
../Data/RecordData/ConsultationChainData/consultations.txt
../Data/RecordData/ExaminationChainData/examinations.txt
../Data/RecordData/HospitalizationChainData/hospitalizations.txt
../Data/RecordData/MedicineChainData/medication_records.txt
../Data/RecordData/MedicineChainData/medicines.txt
../Data/RecordData/HospitalizationChainData/bed_info.txt
```

### Data Model

**12 doubly-linked list chains** (5 user + 7 business record). Each node has `prev`/`next` raw pointers. New nodes use head insertion. Deletion sets `isDeleted` flag (logical delete, never physical). Nested sub-collections (prescriptions, examination items, aliases, medication lines, attachments, related IDs) use `std::vector<T>`.

**User hierarchy:**
```
User (base, virtual destructor)
├── Admin     — global management (accounts, records, medicines, beds, reports)
├── Doctor    — department, title, specialty, schedule, workload counters
├── Nurse     — department, level, schedule, patient care/bed manage counters
├── Pharmacist — department, level, schedule, review/dispense/inventory counters
└── Patient   — department, ID card, address, emergency contacts, allergies,
                medical history, balance, marital status, visit counters
```

Roles are NOT dispatched polymorphically — `main()` uses role-specific pointers directly.

**Business record structs:** `Registration`, `Consultation`, `Examination`, `Hospitalization`, `MedicationRecord`, `Medicine`, `bedInfo` — plain structs with `prev`/`next` and `isDeleted`.

**VitalSigns struct** (in `Examination` and `bedInfo`): 15 indicators — temperature, systolic/diastolic BP, heart rate, respiratory rate, SpO2, height, weight, BMI, pain score, waist circumference, blood glucose, body fat, uric acid, cholesterol.

### ID Scheme

| Type | Format | Example |
|------|--------|---------|
| Admin | `0` + 5 digits | `000001` |
| Doctor | `1` + 5 digits | `100042` |
| Nurse | `2` + 5 digits | `200003` |
| Pharmacist | `3` + 5 digits | `300015` |
| Patient | `4` + 5 digits | `400100` |
| Registration | `reg` + 6 digits | `reg000131` |
| Consultation | `con` + 6 digits | `con000130` |
| Examination | `exa` + 6 digits | `exa000090` |
| Hospitalization | `hos` + 6 digits | `hos000030` |
| MedicationRecord | `mrd` + 6 digits | `mrd000090` |
| Medicine | `med` + 6 digits | `med000020` |
| Bed | auto-generated | `N-03-P-001-01` |

**Bed ID format:** `{DeptCode}-{Area:02d}-{WardCode}-{Ward:03d}-{Bed:02d}`
- Dept codes: 内科=N, 外科=W, 妇产科=F, 急诊科=J, 儿科=E
- Ward codes: 普通病房=P, 隔离病房=G, VIP病房=V, ICU病房=I

**12 global ID counters** defined in `main.cpp`: `adminIDCount`, `doctorIDCount`, `nurseIDCount`, `pharmacistIDCount`, `patientIDCount`, `registrationCount`, `consultationCount`, `examinationCount`, `hospitalizationCount`, `medicationRecordCount`, `medicineCount`, `bedCount`. Passed by reference to load/save functions and role methods.

### File Format

Data files are comma-separated text. Each record is one or more lines.

**Nested data prefixes** (written as additional lines after the parent record):

| Prefix | Used In | Content |
|--------|---------|---------|
| `PRESCRIPTION:` | Consultation | `medicineID,name,dosage,frequency,duration,note` |
| `EXAMINATION_ITEM:` | Consultation | Item name string |
| `ATTACHMENT:` | Consultation, Examination | File path string |
| `VITAL_SIGNS:` | Examination, bedInfo | 15 semicolon-separated doubles |
| `MEDICATION_LINE:` | MedicationRecord | `medicineID,name,quantity,unitPrice,note` |
| `ALIAS:` | Medicine | Alias string |
| `RELATED_REGISTRATION_ID:` | Registration | Related ID string |
| `RELATED_CONSULTATION_ID:` | Consultation | Related ID string |
| `RELATED_EXAMINATION_ID:` | Examination | Related ID string |
| `RELATED_HOSPITALIZATION_ID:` | Hospitalization | Related ID string |

Every data file ends with `count:N` storing the current ID counter.

### Fee Calculation (in `User.cpp`)

| Type | Basis | Rule |
|------|-------|------|
| Registration fee | Doctor title | 实习 10 / 住院 20 / 主治 30 / 副主任 40 / 主任 50 |
| Examination fee | Item name | 2–30 元 per item (14 items, e.g. 血糖 20, 体脂 30, 疼痛评分 2) |
| Hospitalization fee | Ward type × days | 普通 50 / 隔离 100 / VIP 200 / ICU 500 (元/天, minimum 1 day) |

### Key Subsystems

- **Authentication** (`Login.h`, `SHA-256.h`): Salted SHA-256 with 1000 iterations, 16-char random salt. Constant-time comparison (volatile XOR). Account lockout after 5 failed attempts. Admin registration requires API Key `"88888888"`.
- **Persistence** (`LoadData.h`, `SaveData.h`): 12 load functions + 12 save functions, one per chain. Load uses head-insertion to build lists. Save traverses from head.
- **UI** (`UI.h`): 80+ menu functions, 20+ input validation functions, `LogManager` singleton (mutex-guarded, logs to `Data/OperationLog/his_YYYY_MM_DD.log`), Unicode box-drawing menus, CJK width calculation, color output, info cards, hidden password input.
- **Generic account management** (`Login.h`): `AccountManageGeneric<UserType>()` template handles activation/blocking for all 5 role types.
- **Emergency save** (`main.cpp`): Signal handlers for SIGINT/SIGABRT/SIGTERM. Uses double pointers to follow head-insertion changes. Windows API `ReadFile` for pause prompt (std::cin unsafe in signal context).
- **Time** (`GetTime.h`): `MyTime` singleton, returns `YYYY-MM-DD HH:MM:SS` format.
- **Data Analysis** (`DataAnalysis.h`): Monthly stats, demand prediction, bed utilization analysis. Functions exist and compile but are NOT exposed via menus. Do not reference as a user-facing feature.

### Preset Departments

内科, 外科, 妇产科, 急诊科, 儿科

Admin can query by "全院" (hospital-wide) or a specific department. Other roles are restricted to their own department and related records.

### Data Flow

1. `main()` loads all 12 chains from `Data/` via `LoadData` functions
2. If admin chain is empty, force-create first admin (API Key required)
3. User logs in → role-specific menu loop delegates to role class methods
4. All mutations operate on in-memory linked lists directly
5. On normal exit: `SaveData` functions write all chains → cleanup deletes all nodes
6. On crash/signal: `emergencySave()` writes all chains via double pointers → `_Exit()`

## Language

All UI strings, data files, comments, and documentation are in Chinese (Simplified). Code identifiers are in English. Keep new user-facing text in Chinese.

## Debugging

VS Code config in `.vscode/`:
- **"调试 HIS 程序"**: Builds with pre-launch task, runs `build/Debug/his.exe` in external terminal with `cwd: build/`
- **Warning**: `postDebugTask` runs `Remove-Item -Recurse -Force build/*` — this deletes the entire build directory after each debug session

## Build Requirements

- CMake 3.24+
- MSVC (Windows) with C++17 support
- MSVC `/utf-8` flag is set in CMakeLists.txt for Chinese source and data files
- No external libraries required
