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

- Configure: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: 配置"
- Build: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: 编译"
- Build server: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: his_server编译"
- Clean: `Ctrl+Shift+P` → `Tasks: Run Task` → "CMake: build清理"
- Debug: `F5` (MSVC debugger, external terminal via `.vscode/launch.json`)

**Caveat:** `launch.json` has a `postDebugTask` that clears the entire `build/` directory after every debug session.

### REST API Server

```bash
# Build server
cd build && cmake --build . --config Debug --target his_server

# Run server (from build/ directory)
cd build && ./Debug/his_server.exe
# Listens on http://localhost:8080, Ctrl+C to save data and exit
```

### Frontend (Vue 3 + Element Plus)

```bash
# Install dependencies (first time only)
cd frontend && npm install

# Development server (port 3000, auto-proxies /api to localhost:8080)
cd frontend && npm run dev

# Production build
cd frontend && npm run build
# Output: frontend/dist/
```

**Full-stack development**: Start backend first (`his_server.exe` on 8080), then frontend (`npm run dev` on 3000).

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
| ------ | ------ |
| Admin | `Data/UserData/AdminChainData/admin_users.txt` |
| Doctor | `Data/UserData/DoctorChainData/doctor_users.txt` |
| Nurse | `Data/UserData/NurseChainData/nurse_users.txt` |
| Pharmacist | `Data/UserData/PharmacistChainData/pharmacist_users.txt` |
| Patient | `Data/UserData/PatientChainData/patient_users.txt` |

**Record data files:**

| File | Path |
| ------ | ------ |
| Registration | `Data/RecordData/RegistrationChainData/registrations.txt` |
| Consultation | `Data/RecordData/ConsultationChainData/consultations.txt` |
| Examination | `Data/RecordData/ExaminationChainData/examinations.txt` |
| Hospitalization | `Data/RecordData/HospitalizationChainData/hospitalizations.txt` |
| Bed Info | `Data/RecordData/HospitalizationChainData/bed_info.txt` |
| Medication Record | `Data/RecordData/MedicineChainData/medication_records.txt` |
| Medicine | `Data/RecordData/MedicineChainData/medicines.txt` |

Null/empty string fields use `"#"` as sentinel value. All deletions are **logical** (`isDeleted` flag), never physical removal.

**Important:** All `Data/` subdirectories must exist before the program runs. The program does not create directories automatically; missing directories cause silent load failures.

### Zero External Build Dependencies

Third-party libraries (`httplib.h`, `json.hpp`) are bundled directly in `Head/`. No vcpkg, Conan, or FetchContent needed — only a C++17 compiler. Note that `his_server` compiles all `Source/*.cpp` files including console UI code (`UI.cpp`, etc.), so the server binary carries console-specific code that is simply unused at runtime.

### Key Conventions

- **Naming**: Headers = PascalCase (`Doctor.h`), functions = camelCase (`addDoctor()`), matching source file per header
- **Password security**: SHA-256 with salt + 1,000 iterations. Stored format: `salt$hash`. Uses volatile XOR-based timing-safe comparison to prevent timing attacks. Account locks after 5 failed attempts (`failedLoginLimit` constant in `User.h`), resets on successful login.
- **Time**: Use `MyTime` singleton from `GetTime.h` — format `YYYY-MM-DD hh:mm:ss`
- **Departments**: 内科, 外科, 儿科, 妇产科, 急诊科
- **Fee calculators**: `calculateRegistrationFee()` by doctor title (10–50 yuan), `calculateExaminationFee()` by item name (2–30 yuan), `calculateHospitalizationFee()` by ward type + days (50–500 yuan/day). Exact prices defined in `User.h`.
- **Generic programming**: `Login.h` has `AccountManageGeneric<UserType>` template for account activation/lockdown across all user types
- **UI/input validation**: All console I/O and validation functions are in `UI.h`/`UI.cpp`
- **Global ID counters**: `main.cpp` declares global counters (`adminIDCount`, `doctorIDCount`, etc.) that track the number of used IDs per role. These are populated during `load*Data()` calls and passed to `signUp()` methods to generate new unique IDs.

### UI/UX Features

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

### Startup Flow

1. Load admin data first; if none exists, force admin registration before system starts
2. Load all other user data (doctor, nurse, pharmacist, patient) and all record data
3. Enter main loop: login/register selection → role selection → role-specific menu
4. On exit, save all data back to files (both user data and record data)

### File Layout

- `Head/` — all header files (23 .h files, including `httplib.h` and `json.hpp` third-party headers)
- `Source/` — implementations (15 .cpp files)
- `Data/` — persistent storage (user data in `UserData/`, record data in `RecordData/`, operation logs in `OperationLog/`)
- `main.cpp` — console app entry point with nested while-loops per role
- `server_main.cpp` — REST server entry point
- `frontend/` — Vue 3 web frontend
- `CMakeLists.txt` — CMake config (C++17, MSVC `/utf-8`, include `Head/`). Two targets: `his` (console) and `his_server` (REST)

### REST API Server Architecture

C++ REST backend using cpp-httplib (single-header, in `Head/httplib.h`) + nlohmann/json (single-header, in `Head/json.hpp`).

| File | Purpose |
| ------ | --------- |
| `server_main.cpp` | Entry point. Loads data, starts server on port 8080, Ctrl+C handler saves data |
| `Head/ApiServer.h` | `DataManager` singleton (thread-safe via mutex, owns all linked list heads + ID counters) + `registerApiRoutes()` + `authenticateRequest()` |
| `Head/ApiResponse.h` | Unified JSON response builder: `success()`, `error()`, `badRequest()`, `unauthorized()`, `forbidden()`, `notFound()`, `serverError()` |
| `Head/JWTAuth.h` | JWT token generation/validation, Bearer token extraction |
| `Head/JsonHelper.h` | All struct-to-JSON converters + enum-to-string converters |
| `Source/ApiServer.cpp` | All route handlers (115+ route registrations) |
| `Source/JWTAuth.cpp` | JWT implementation |
| `Source/JsonHelper.cpp` | JSON serialization implementation |

`DataManager` replaces the console app's global variables and `main.cpp` global ID counters, wrapping everything in a `std::mutex` for thread safety. API response format: `{ "code": int, "message": string, "data": object }`.

Key API endpoint groups: `/api/auth/*` (login/register), `/api/admin/*` (CRUD all entities), `/api/doctor/*`, `/api/nurse/*`, `/api/patient/*`, `/api/pharmacist/*` (role-specific), `/api/departments`, `/api/fee-standards`, `/api/examination-items` (reference data).

### Frontend Architecture

Vue 3 + Vite 6 + Pinia (state) + Vue Router (routing) + Element Plus (UI, `zh-cn` locale) + Axios (HTTP). Dev server on port 3000, proxies `/api` to `localhost:8080`.

| Directory | Purpose |
| ----------- | --------- |
| `frontend/src/api/` | 8 modules: `index.js` (Axios with Bearer token interceptor + 401 redirect), `auth.js`, `admin.js`, `doctor.js`, `nurse.js`, `patient.js`, `pharmacist.js`, `common.js` (shared helpers including `getDoctors` for any-role access) |
| `frontend/src/store/` | Pinia user store (`user.js`) — token, userID, username, role persisted to localStorage |
| `frontend/src/router/` | Vue Router with role-based route groups + navigation guard |
| `frontend/src/views/` | 33 `.vue` files: `admin/` (13 — includes Dashboard, all entity CRUD, Profile), `doctor/` (4), `nurse/` (4), `pharmacist/` (3), `patient/` (6), plus `Layout.vue`, `Login.vue`, `Register.vue` |
| `frontend/src/styles/` | `global.css` (reset, Microsoft YaHei font) |

**Role number discrepancy:** The C++ console app uses 0-based roles in IDs (0=Admin, 1=Doctor, 2=Nurse, 3=Pharmacist, 4=Patient). The frontend Pinia store uses 1-based role mapping (1=管理员, 2=医生, 3=护士, 4=药剂师, 5=患者). The API server bridges these.

### Known Gotchas

**Enums:** `bedStatus` uses lowercase 'b' (not `BedStatus`). `ConsultationStatus` has `COMPLETED` (not `FINISHED`). `Examination` has `reportSummary` field (not `report`).

**Linked lists:** Always use head-insertion (`newNode->next = head; if(head) head->prev = newNode; head = newNode;`) for O(1). Tail-insertion with while-loop is O(n) and was a confirmed bug.

**Conditional nesting:** Watch for `else if` blocks accidentally nested inside `if` branches of a preceding `else if`. This caused menu choice 4 to be unreachable in `Admin::manageConsultations`.

**Pointer reuse:** When checking "are there any X available?", use a boolean flag set during iteration. Checking if the original head pointer is `nullptr` after iteration is always false (head is never modified by read-only iteration).

**ID prefixes:** Medication record IDs use `"mrd"` prefix, not `"med"`. Medicine IDs use `"med"` prefix. Bed IDs use `autoGenerateBedID()` format.

**Admin struct:** Has `prev` pointer (doubly-linked). All entities use doubly-linked lists.

### `"#"` Sentinel Convention

All null/empty string fields use `"#"` as sentinel (not empty string or `null`). This applies to every text field across all data files — gender, telephone, email, department, note, productionDate, expiryDate, etc. API handlers and console code both check `field == "#"` to determine emptiness and convert `"#"` to display-friendly strings.

### Language

Code comments, UI strings, documentation, and commit messages are primarily in Chinese. Commit messages use date-based versioning (e.g., "4.24.1").
