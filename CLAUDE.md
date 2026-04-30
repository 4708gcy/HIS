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

# Preview production build locally
cd frontend && npm run preview
```

**Full-stack development**: Start backend first (`his_server.exe` on 8080), then frontend (`npm run dev` on 3000). Shutdown order: close frontend first, then backend.

No test framework exists; testing is manual via interactive console.

**Important:** The program MUST be launched from the `build/` directory because all data file paths use `../Data/` relative paths. Running from the project root will fail to find data files. First console run forces admin account registration before anything else.

## Architecture

C++17 console application for a Hospital Information System. Windows-only (uses MSVC, `SetConsoleOutputCP(CP_UTF8)`).

### Class Hierarchy

```text
User (base class) — holds all shared auth, enums, fee calculators, status-to-string converters
  ├── Admin    — manages all records and all user types (~7,785 lines, the largest file)
  ├── Doctor   — registration, consultation, examination
  ├── Nurse    — examination, hospitalization, bed management
  ├── Pharmacist — medication records, medicine inventory
  └── Patient  — appointment, payment, personal info
```

All subclasses inherit from `User` so they share the same in-memory record chains (registrations, consultations, etc.). Without this shared base, cross-role data sync would be impossible.

### Data Structures

All records use **doubly-linked lists** (each node has `prev`/`next` pointers). Key structs: `Registration`, `Consultation` (contains `Prescription` sub-struct + exam list), `Examination` (contains `VitalSigns`), `Hospitalization`, `bedInfo`, `MedicationRecord` (contains `MedicationLine` items), `Medicine` (has `genericName` and `aliases` fields), `MedicineFlow` (tracks stock in/out with type, quantity, operator, reason, timestamp).

### ID Scheme

6-digit IDs: first digit = role (`0`=Admin, `1`=Doctor, `2`=Nurse, `3`=Pharmacist, `4`=Patient), remaining 5 digits sequential (max 99,999 per role). Department is a separate attribute, not encoded in ID.

### Role Mapping (Backend ↔ Frontend)

| Backend (0-based) | Frontend (1-based) | Role |
| --- | --- | --- |
| 0 | 1 | 管理员 (Admin) |
| 1 | 2 | 医生 (Doctor) |
| 2 | 3 | 护士 (Nurse) |
| 3 | 4 | 药剂师 (Pharmacist) |
| 4 | 5 | 患者 (Patient) |

The API server bridges the two numbering schemes.

### Fee Standards

| Type | Basis | Price |
| --- | --- | --- |
| Registration | Doctor title | 实习10 / 住院20 / 主治30 / 副高40 / 正高50 元 |
| Examination | Item name | 2–30 元 (see `User.h`) |
| Hospitalization | Ward type × days | 普通50 / 隔离100 / VIP200 / ICU500 元/天 |

### Data Persistence

Plain text CSV in `Data/` (relative path `../Data/` from build). Split into `UserData/` and `RecordData/` subdirectories. File paths are `#define` macros in `User.h`.
Null/empty fields use `"#"` sentinel. All deletions are logical (`isDeleted`), never physical.
**Important:** All `Data/` subdirectories must exist before the program runs — missing directories cause silent load failures.

| Entity | File path |
| --- | --- |
| Admin | `Data/UserData/AdminChainData/admin_users.txt` |
| Doctor | `Data/UserData/DoctorChainData/doctor_users.txt` |
| Nurse | `Data/UserData/NurseChainData/nurse_users.txt` |
| Pharmacist | `Data/UserData/PharmacistChainData/pharmacist_users.txt` |
| Patient | `Data/UserData/PatientChainData/patient_users.txt` |
| Registration | `Data/RecordData/RegistrationChainData/registrations.txt` |
| Consultation | `Data/RecordData/ConsultationChainData/consultations.txt` |
| Examination | `Data/RecordData/ExaminationChainData/examinations.txt` |
| Hospitalization | `Data/RecordData/HospitalizationChainData/hospitalizations.txt` |
| Bed | `Data/RecordData/HospitalizationChainData/bed_info.txt` |
| Medication record | `Data/RecordData/MedicineChainData/medication_records.txt` |
| Medicine | `Data/RecordData/MedicineChainData/medicines.txt` |
| Medicine flow | `Data/RecordData/MedicineChainData/medicine_flow.txt` |
| Operation log | `Data/OperationLog/his_YYYY_MM_DD.log` |

### Zero External Build Dependencies

Third-party libraries (`httplib.h`, `json.hpp`) are bundled directly in `Head/`. No vcpkg, Conan, or FetchContent needed — only a C++17 compiler. Note that `his_server` compiles all `Source/*.cpp` files including console UI code (`UI.cpp`, etc.), so the server binary carries console-specific code that is simply unused at runtime.

### Key Conventions

- **Naming**: Headers = PascalCase (`Doctor.h`), functions = camelCase (`addDoctor()`), matching source file per header
- **Password security**: SHA-256 with salt + 1,000 iterations. Stored format: `salt$hash`. Uses volatile XOR-based timing-safe comparison to prevent timing attacks. Account locks after 5 failed attempts (`failedLoginLimit` constant in `User.h`), resets on successful login.
- **Time**: Use `MyTime` singleton from `GetTime.h` — format `YYYY-MM-DD hh:mm:ss`
- **Departments**: 内科, 外科, 儿科, 妇产科, 急诊科
- **Generic programming**: `Login.h` has `AccountManageGeneric<UserType>` template for account activation/lockdown across all user types
- **UI/input validation**: All console I/O and validation functions are in `UI.h`/`UI.cpp`. The `deptMatch(entityDept, filter)` utility supports "全院" (all-departments) filtering. All menus use Unicode double-line box-drawing with auto-clearing; see UI/UX Features section for function reference.
- **Global ID counters**: `main.cpp` declares global counters (`adminIDCount`, `doctorIDCount`, etc.) that track the number of used IDs per role. These are populated during `load*Data()` calls and passed to `signUp()` methods to generate new unique IDs.
- **`saveAllUnsafe()` naming convention**: In `DataManager`, `saveAllUnsafe()` saves all data without acquiring the mutex. Callers must hold the mutex before calling it (e.g., `std::lock_guard<std::mutex> lock(dm.getMutex()); dm.saveAllUnsafe();`). The "Unsafe" suffix means "unsafe to call without mutex held", not "unsafe for production". This pattern appears ~74 times in `ApiServer.cpp`.
- **Data analysis**: `DataAnalysis.h/cpp` provides statistical analysis (monthly stats, demand prediction via moving average + linear regression, bed allocation optimization, medicine profit margins). Three display formats: tabular, ASCII charts, summary. Invoked via Admin report menu option 6.

### UI/UX Features

Console I/O uses Unicode double-line box-drawing menus (`╔═╗║╚═╝`), colored output (`ConsoleColor` enum in `UI.h`), auto-clearing (`clearScreen()` before each menu), and CJK width-aware centering (`getDisplayWidth()` — CJK chars count as 2 columns). Key rendering functions:

| Function | Purpose |
| --- | --- |
| `clearScreen()` | Cross-platform screen clear (`cls`/`clear`) |
| `getDisplayWidth(str)` | String display width accounting for multi-byte CJK |
| `printMenuBorder()` | Top border `╔═══╗` |
| `printMenuBottom()` | Bottom border `╚═══╝` |
| `printMenuTitle(title)` | Centered title `╠...╣` (CJK-aware) |
| `printMenuItem(num, text)` | Numbered menu item `║  N. text ║` (auto-aligned) |
| `printMenuLine(text)` | Non-numbered menu row `║  text ║` |
| `printInfoCard(title, fields)` | Unicode card with labeled fields |
| `printWithPagination(lines, pageSize)` | Colored paginated display |
| `pause(breadcrumb)` | Wait for Enter with optional `[path]` display |
| `printSuccess/Error/Warning(msg)` | Colored status messages |

`MENU_INNER_WIDTH = 40` constant in `UI.cpp` controls menu interior width. All 80+ menu functions in `UI.cpp` and inline menus in business logic files use this system. Hidden password input uses `inputHiddenPwdCheck()` via Windows `ReadFile` API.

### Operation Logging

`LogManager` singleton in `UI.h`/`UI.cpp` — thread-safe, structured operation logs persisted to `Data/OperationLog/his_YYYY_MM_DD.log`.

### REST API Server Architecture

cpp-httplib + nlohmann/json (both single-header in `Head/`). `DataManager` singleton replaces global variables, `std::mutex` thread safety. ~125 route registrations. Response: `{ "code": int, "message": string, "data": object }`. Error codes: 200/400/401/403/404/500. All responses include `Access-Control-Allow-Origin: *` CORS header.

| File | Purpose |
| ------ | --------- |
| `server_main.cpp` | Entry point, port 8080, Ctrl+C saves data |
| `Head/ApiServer.h` | DataManager singleton + route registration |
| `Head/ApiResponse.h` | JSON response builders |
| `Head/JWTAuth.h` | JWT (HMAC-SHA256, 24h validity) |
| `Head/JsonHelper.h` | Struct-to-JSON + enum-to-string |
| `Source/ApiServer.cpp` | All ~125 route handlers |

Endpoint groups:

| Group | Path prefix | Count | Description |
| --- | --- | --- | --- |
| Auth | `/api/auth/` | 4 | Login/register/change-pwd |
| Admin | `/api/admin/` | 30+ | CRUD all entities, user/bed/medicine management |
| Doctor | `/api/doctor/` | 11 | Registration/consultation/examination CRUD |
| Nurse | `/api/nurse/` | 11 | Hospitalization/bed assignment/discharge |
| Pharmacist | `/api/pharmacist/` | 11 | Medication review/dispensing/inventory |
| Patient | `/api/patient/` | 12 | Registration/payment/records/recharge |
| Common | `/api/` | 3 | `/departments`, `/fee-standards`, `/examination-items` |

### Frontend Architecture

Vue 3 + Vite 6 + Pinia (localStorage) + Vue Router (role guards) + Element Plus (zh-cn) + Axios (JWT interceptor). Port 3000, proxies `/api` → 8080. 34 Vue files, 8 API modules.

| Directory | Purpose |
| ----------- | --------- |
| `api/` | 8 modules — `index.js` (axios + Bearer + 401 redirect), role-specific modules, `common.js` (shared) |
| `store/user.js` | Pinia — token/userID/role, localStorage persisted |
| `router/index.js` | Role-based route groups + navigation guard |
| `views/` | 33 `.vue` files: `admin/`(13), `doctor/`(4), `nurse/`(4), `pharmacist/`(3), `patient/`(6), plus Layout/Login/Register (+ `App.vue` at `src/` root = 34 total) |
| `styles/global.css` | 167-line CSS variable theme ("Pure & Clinical", blue `#1e88e5`, frosted-glass cards, `fade-in`/`slideIn` animations) |

Route guard behavior: unauthenticated → `/login`; role mismatch → auto-redirect to role's default page.

### Known Gotchas

**Enums:** `bedStatus` uses lowercase 'b' (not `BedStatus`). `ConsultationStatus` has `COMPLETED` (not `FINISHED`). `Examination` has `reportSummary` field (not `report`).

**Linked lists:** Always use head-insertion (`newNode->next = head; if(head) head->prev = newNode; head = newNode;`) for O(1). Tail-insertion with while-loop is O(n) and was a confirmed bug.

**Conditional nesting:** Watch for `else if` blocks accidentally nested inside `if` branches of a preceding `else if`. This caused menu choice 4 to be unreachable in `Admin::manageConsultations`.

**Pointer reuse:** When checking "are there any X available?", use a boolean flag set during iteration. Checking if the original head pointer is `nullptr` after iteration is always false (head is never modified by read-only iteration).

**ID prefixes:** Medication record IDs use `"mrd"` prefix, not `"med"`. Medicine IDs use `"med"` prefix. Medicine flow IDs use `"mfl"` prefix. Bed IDs use `autoGenerateBedID()` format.

**Admin struct:** Has `prev` pointer (doubly-linked). All entities use doubly-linked lists.

**`saveAllUnsafe()`:** Caller must hold the mutex; the function itself does NOT acquire the lock.

**`printMenuSeparator()`:** Defined in `UI.cpp` but never called — dead code. Do not use it; use `printMenuLine("")` for blank rows instead.

### `"#"` Sentinel Convention

All null/empty string fields use `"#"` as sentinel (not empty string or `null`). This applies to every text field across all data files — gender, telephone, email, department, note, productionDate, expiryDate, etc. API handlers and console code both check `field == "#"` to determine emptiness and convert `"#"` to display-friendly strings.

### Language

Code comments, UI strings, documentation, and commit messages are primarily in Chinese. Commit messages use date-based versioning (e.g., "4.24.1").
