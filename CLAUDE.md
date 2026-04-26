# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

```bash
# Configure (from project root)
cd build && cmake ..

# Build
cd build && cmake --build . --config Debug

# Run (must be inside build/ directory due to relative data paths)
cd build && ./Debug/his.exe

# Clean build directory (PowerShell)
Remove-Item build\* -Recurse -Force
```

VS Code:

- Build: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: 编译"
- Clean: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: build清理"
- Debug: `F5` (MSVC debugger, external terminal via `.vscode/launch.json`)

No test framework exists; testing is manual via interactive console.

**Important:** The program MUST be launched from the `build/` directory because all data file paths use `../Data/` relative paths. Running from the project root will fail to find data files.

## Architecture

C++17 console application for a Hospital Information System. Windows-only (uses MSVC, `SetConsoleOutputCP(CP_UTF8)`).

### Class Hierarchy

```text

User (base class) — holds all shared auth, enums, fee calculators, status-to-string converters
  ├── Admin    — manages all records and all user types (7,775 lines, the largest file)
  ├── Doctor   — registration, consultation, examination
  ├── Nurse    — examination, hospitalization, bed management
  ├── Pharmacist — medication records, medicine inventory
  └── Patient  — appointment, payment, personal info
```

All subclasses inherit from `User` so they share the same in-memory record chains (registrations, consultations, etc.). Without this shared base, cross-role data sync would be impossible.

### Data Structures

All records use **doubly-linked lists** (each node has `prev`/`next` pointers). Key structs: `Registration`, `Consultation` (contains `Prescription` sub-struct + exam list), `Examination` (contains `VitalSigns`), `Hospitalization`, `bedInfo`, `MedicationRecord` (contains `MedicationLine` items), `Medicine`.

### ID Scheme

6-digit IDs: first digit = role (`0`=Admin, `1`=Doctor, `2`=Nurse, `3`=Pharmacist, `4`=Patient), remaining 5 digits sequential (max 99,999 per role). Department is a separate attribute, not encoded in ID.

### Data Persistence

Plain text CSV files in `Data/` (relative paths from build directory via `../Data/`). Split into `UserData/` and `RecordData/` subdirectories. File paths are `#define` macros in `User.h`.

**User data files:**
| File | Path |

|------|------|
| Admin | `Data/UserData/AdminChainData/admin_users.txt` |
| Doctor | `Data/UserData/DoctorChainData/doctor_users.txt` |
| Nurse | `Data/UserData/NurseChainData/nurse_users.txt` |
| Pharmacist | `Data/UserData/PharmacistChainData/pharmacist_users.txt` |
| Patient | `Data/UserData/PatientChainData/patient_users.txt` |

**Record data files:**
| File | Path |

|------|------|
| Registration | `Data/RecordData/RegistrationChainData/registrations.txt` |
| Consultation | `Data/RecordData/ConsultationChainData/consultations.txt` |
| Examination | `Data/RecordData/ExaminationChainData/examinations.txt` |
| Hospitalization | `Data/RecordData/HospitalizationChainData/hospitalizations.txt` |
| Bed Info | `Data/RecordData/HospitalizationChainData/bed_info.txt` |
| Medication Record | `Data/RecordData/MedicineChainData/medication_records.txt` |
| Medicine | `Data/RecordData/MedicineChainData/medicines.txt` |

Null/empty string fields use `"#"` as sentinel value.

All deletions are **logical** (`isDeleted` flag), never physical removal.

### Key Conventions

- **Naming**: Headers = PascalCase (`Doctor.h`), functions = camelCase (`addDoctor()`), matching source file per header
- **Password security**: SHA-256 with salt + 1,000 iterations. Stored format: `salt$hash`. Account locks after 5 failed attempts (`failedLoginLimit` constant in `User.h`).
- **Time**: Use `MyTime` singleton from `GetTime.h` — format `YYYY-MM-DD hh:mm:ss`
- **Departments**: 内科, 外科, 儿科, 妇产科, 急诊科
- **Fee calculators**: `calculateRegistrationFee()` by doctor title (10-50 yuan), `calculateExaminationFee()` by item name (2-30 yuan), `calculateHospitalizationFee()` by ward type + days (50-500 yuan/day)
  - Registration: INTERN=10, RESIDENT=20, ATTENDING=30, ASSOCIATE_CHIEF=40, CHIEF=50
  - Hospitalization: 普通病房=50/天, 隔离病房=100/天, VIP=200/天, ICU=500/天
  - Examination items: temperature=5, BP=8, heartRate=5, respiratoryRate=5, spo2=10, height=5, weight=5, bmi=5, painScore=2, waistCircumference=5, bloodSugar=20, bodyFat=30, uricAcid=25, cholesterol=25
- **Generic programming**: `Login.h` has `AccountManageGeneric<UserType>` template for account activation/lockdown across all user types
- **UI/input validation**: All console I/O and validation functions are in `UI.h`/`UI.cpp`
- **Global ID counters**: `main.cpp` declares global counters (`adminIDCount`, `doctorIDCount`, etc.) that track the number of used IDs per role. These are populated during `load*Data()` calls and passed to `signUp()` methods to generate new unique IDs.

### UI/UX Features (4.25+)

- **Console color system**: `ConsoleColor` enum (RED, GREEN, YELLOW, CYAN, WHITE, DEFAULT) in `UI.h` with `setConsoleColor()`/`resetConsoleColor()`. Supports both Windows API (`SetConsoleTextAttribute`) and ANSI escape sequences.
- **Pagination**: `printWithPagination()` for displaying long lists with page navigation (prev/next/jump to page). Uses `(std::min)` to avoid Windows `min` macro conflicts.
- **Breadcrumb navigation**: `pause()` accepts a `breadcrumb` parameter to show current path in pause prompt.
- **Hidden password input**: `inputHiddenPwdCheck()` uses Windows `ReadFile` API for masked password entry during login.
- **Quick print helpers**: `printTitle()`, `printSuccess()` (green), `printError()` (red), `printWarning()` (yellow).

### Operation Logging

`LogManager` singleton class in `UI.h`/`UI.cpp`:

- Thread-safe via `std::mutex`
- Methods: `info()`, `warn()`, `error()`, `logOperation()` (structured: userId, role, operation type, details)
- Persists to `Data/OperationLog/` directory
- Format: `[YYYY-MM-DD hh:mm:ss] [LEVEL] message`

### Security Details

- **SHA-256 passwords**: salt + 1,000 iterations, stored as `salt$hash`. Uses volatile XOR-based timing-safe comparison (not `std::string` compare) to prevent timing attacks.
- **Account lockout**: 5 failed attempts (`failedLoginLimit` in `User.h`), `loginAttempts` resets on successful login.

### Startup Flow

1. Load admin data first; if none exists, force admin registration before system starts
2. Load all other user data (doctor, nurse, pharmacist, patient) and all record data
3. Enter main loop: login/register selection → role selection → role-specific menu
4. On exit, save all data back to files (both user data and record data)

### File Layout

- `Head/` — all header files (18 .h files)
- `Source/` — implementations (12 .cpp files)
- `Data/` — persistent storage (user data in `UserData/`, record data in `RecordData/`, operation logs in `OperationLog/`)
- `main.cpp` — menu system with nested while-loops per role
- `CMakeLists.txt` — CMake config (C++17, MSVC `/utf-8`, include `Head/`)

### Language

Code comments, UI strings, documentation, and commit messages are primarily in Chinese. Commit messages use date-based versioning (e.g., "4.24.1").
