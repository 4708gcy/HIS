# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Quick Start

```bash
# Prerequisites: MySQL 8.0 running on localhost:3307 (root/123456)
# Start MySQL service if not running:
net start MySQL80

# Initialize database (first time only)
mysql -u root -p123456 -P 3307 < Data/Schema/schema.sql
python Data/Schema/migrate_csv_to_mysql.py  # import CSV data if migrating

# Configure (run once, or when CMakeLists.txt changes)
mkdir build && cd build && cmake ..

# Build
cmake --build build --config Debug --target his

# Run — MUST run from build/ (config file uses ../Data/ relative path)
cp E:/MySQL/8-0/lib/libmysql.dll build/Debug/   # ensure DLL is present
cd build && ./Debug/his.exe
```

First run without admin data forces admin creation (requires API Key: `88888888` by default, overridable via `HIS_ADMIN_API_KEY` env var or `Data/AdminAPIKey.txt`).

## Gotchas

- **MySQL must be running.** The program connects to MySQL on startup (`127.0.0.1:3307`). If the connection fails, it prints a warning and exits. Database config is in `Data/DatabaseConfig.txt`.
- **`libmysql.dll` must be next to `his.exe`.** Copy from `E:/MySQL/8-0/lib/libmysql.dll` to `build/Debug/`. The program will crash with a DLL-not-found error if missing.
- **Working directory is critical.** The config file path `../Data/DatabaseConfig.txt` is relative — the executable MUST run from `build/`.
- **VS Code `postDebugTask` destroys the build directory.** `launch.json` runs `Remove-Item -Recurse -Force build/*` after every debug session. This also deletes `libmysql.dll` — you'll need to recopy it.
- **Stale `his_server` task.** `.vscode/tasks.json` references a non-existent target; ignore it.
- **Emergency save uses double pointers.** Global pointers like `static Admin **g_adminHead` point to the local `adminHead` variable's address. MySQL C API is NOT signal-safe — `emergencySave()` in signal handlers may be unreliable.
- **`#` is the empty-field sentinel.** Text fields default to `"#"`, which maps to SQL `NULL` in the database layer (`dbStr()` helper maps `"#"` → empty string → SQL NULL; `dbDateTime()` uses `\x01` prefix convention to inject raw SQL NULL, recognized by `buildSql()`).
- **`clearScreen()` uses safe Win32 API** (`FillConsoleOutputCharacter` + `SetConsoleCursorPosition`) on Windows, ANSI escape `\033[2J\033[1;1H` on Linux. No `system("cls")` call.
- **EntityRepository ID fields are immutable via the repo.** `getIdField()` returns `const std::string&` — modifying entity IDs externally will cause `idMap` index to go stale. Call `repo.reindex()` after any external ID change.
- **API Key has 3-level priority:** Environment variable `HIS_ADMIN_API_KEY` → config file `Data/AdminAPIKey.txt` → hardcoded default `88888888`. Same pattern applies to AI service (`HIS_API_KEY` env var).
- **No test suite.** Verify changes by building and running the program interactively.
- **New `.cpp` files require CMake reconfigure.** `CMakeLists.txt` uses `file(GLOB)` for source discovery across `Source/Core/`, `Source/Roles/`, `Source/Modules/`. After creating a new `.cpp` file, run `cd build && cmake ..`.
- **All monetary values are in cents (int).** Amount fields in MySQL are `INT` columns (`fee_cents`, `deposit_cents`, `balance_cents`, etc.). Divide by 100.0 for yuan display. Never use `double` for money comparisons.
- **`patient->isHospitalized` is set in `assignBed` not `createHospitalization`.** The flag flips to `true` only when a nurse explicitly assigns a bed.
- **AI service is optional.** Admin menu option 9 handles the Python service being offline gracefully.
- **CSV data files have been removed.** Data persistence is entirely MySQL-based. The original CSV files in `Data/UserData/` and `Data/RecordData/` are deleted — use `Data/Schema/migrate_csv_to_mysql.py` if you need to re-import legacy data.

## Project Overview

HIS — Hospital Information System. A C++17 console application backed by MySQL 8.0 for a university course design project. Dependencies: MySQL C client library (`libmysql`), WinSock2 (Windows built-in). Manages 5 user roles (admin, doctor, nurse, pharmacist, patient) and 8 business record types (registration, consultation, examination, hospitalization, medication record, medicine inventory, beds, nursing records) via a Chinese-language terminal UI. Includes a Python Flask AI microservice for predictive analytics and natural language querying.

## Architecture

### Directory Layout

```
main.cpp              — entry point, signal handlers, main loop (connect DB → load → login/menu → save → disconnect)
Head/                 — 25 header files in 4 subdirs: Core(7)/Entities(7)/Roles(5)/Modules(6)
Source/               — 18 source files in 3 subdirs: Core(6)/Roles(8)/Modules(5)
Data/
  DatabaseConfig.txt  — MySQL connection config (host, port, user, password, database)
  Schema/
    schema.sql         — CREATE TABLE statements for 21 tables + constraints
    migrate_csv_to_mysql.py — CSV-to-MySQL migration script
  DrugData/interactions.txt — 30 drug-drug interaction pairs (still file-based)
  OperationLog/*.log   — daily operation logs
Document/             — project documentation
ai_service/           — Python FastAPI AI v2.0 (LangChain + MySQL + matplotlib)
  main.py             — FastAPI entry point (uvicorn on port 5001)
  schemas.py          — Pydantic request/response models (Strategy enum, HISResponse)
  config.yaml         — YAML configuration center (app/llm/database/security/rag/analysis/charts)
  Dockerfile          — Docker deployment (python:3.11-slim + uvicorn)
  requirements.txt    — fastapi, uvicorn, pydantic, langchain, langchain-openai, faiss-cpu, sentence-transformers, jieba, scikit-learn, pymysql, matplotlib, pandas, numpy, pyyaml, pdfplumber, python-docx
  core/               — Foundation layer
    config.py         — Settings singleton (YAML load + env var overrides: HIS_LLM_API_KEY, HIS_API_KEY, HIS_DB_HOST, etc.)
    database.py       — MySQL connection pool (DatabasePool, DictCursor, contextmanager)
    security.py       — API Key auth via FastAPI Depends (X-API-Key header)
    response.py       — ResponseBuilder (unified JSON format: request_id, data, response_code, response_msg, process_status, processing_time)
    exceptions.py     — HISException, LLMException, DataLoadException
    llm_provider.py   — LangChain ChatOpenAI factory (OpenAI-compatible API, default: Qwen on Aliyun Bailian)
  routers/            — API route layer (all /api/v2/ prefix, all require X-API-Key except health/root)
    predictions.py    — POST /api/v2/predictions (demand forecasting with traditional/llm/auto strategy)
    anomalies.py      — POST /api/v2/anomalies (Z-score anomaly detection + LLM interpretation)
    beds.py           — POST /api/v2/beds (bed allocation optimization + LLM report)
    dashboard.py      — POST /api/v2/dashboard (comprehensive dashboard with LLM executive summary)
    charts.py         — GET /api/v2/charts/{type} (PNG chart generation)
    rag.py            — POST /api/v2/rag/chat + /api/v2/rag/upload (RAG knowledge base Q&A + async document upload)
  services/           — Business logic layer
    analyzer.py       — Traditional algorithms (Holt-Winters, Z-score, bed utilization, medicine inventory)
    predictor.py      — Prediction engine (traditional / llm / auto strategy dispatch)
    anomaly_detector.py — Anomaly detection engine (traditional stats + LLM interpretation)
    bed_optimizer.py  — Bed optimization engine (utilization analysis + LLM report)
    report_generator.py — Dashboard report generator (multi-source aggregation + LLM summary)
    data_loader.py    — MySQL data loader (monthly stats, bed info, hospitalizations, medicine inventory)
    llm_client.py     — LangChain LLM client wrapper (invoke, chat, invoke_with_prompt)
    prompt_builder.py — Dynamic Few-shot Prompt builder (TF-IDF similar case retrieval + ChatPromptTemplate)
    rag_engine.py     — RAG engine (sentence-transformers embedding + faiss vector store + sliding-window chunking)
  tests/
    test_services.py  — pytest unit tests (TraditionalAnalyzer, PromptBuilder, LLMProvider)
  charts.py           — Matplotlib chart generators (prediction bars, bed utilization, anomaly scatter, medicine pie)
  charts/             — Generated PNG output
```

### MySQL Database (`his_db`)

**21 tables** organized as 14 main tables + 7 sub-tables:

**Main tables:** `admins`, `doctors`, `nurses`, `pharmacists`, `patients`, `registrations`, `consultations`, `examinations`, `hospitalizations`, `medication_records`, `medicines`, `bed_info`, `nursing_records`, `operation_logs`

**Sub-tables** (normalized from CSV prefix lines):
- `prescription_items` — consultation prescriptions
- `examination_items` — consultation examination items
- `attachment_files` — consultation/examination attachments
- `medication_lines` — medication record detail lines
- `medicine_aliases` — medicine alternative names
- `vital_signs` — 15-indicator vital signs for examinations and beds
- `related_record_ids` — cross-record references

**Foreign keys:** `ON DELETE SET NULL` for patient/doctor/nurse references; `ON DELETE CASCADE` for sub-tables.

**Check constraints:** Status ranges, monetary `>= 0`, stock `>= 0` enforced at DB level.

### Data Model (In-Memory)

**13 doubly-linked list chains** (5 user + 8 business record). Each node has `prev`/`next` raw pointers. New nodes use head insertion. Deletion sets `isDeleted` flag (logical delete). Nested sub-collections use `std::vector<T>`.

`NursingRecord` chain is also managed by `EntityRepository<NursingRecord>` (template class in `EntityRepository.h`) providing O(1) ID lookup. `Patient` and `Registration` chains are now also managed by `EntityRepository` via template specializations (`getIdField` returns `patientID` / `registrationID` respectively).

**User hierarchy:**
```
User (base, virtual destructor)
├── Admin     — global management
├── Doctor    — department, title, specialty, schedule, workload counters
├── Nurse     — department, level, schedule, patient care/bed manage counters
├── Pharmacist — department, level, schedule, review/dispense/inventory counters
└── Patient   — department, ID card, address, emergency contacts, allergies,
                medical history, balance, marital status, visit counters
```

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
| Nursing Record | `nur` + 6 digits | `nur000001` |
| Bed | auto-generated | `N-03-P-001-01` |

**12 global ID counters** defined in `main.cpp`. In MySQL mode, initialized via `SELECT MAX()` queries.

### Key Subsystems

- **Database** (`Database.h`, `Database.cpp`): MySQL C API wrapper. Singleton via `GetDB()`. Provides `connect()`, `query()`, `execute()`, `queryPrepared()`, `executePrepared()`, transaction support (`beginTransaction/commit/rollback`), and row-reading helpers.
- **Persistence** (`LoadData.h`, `SaveData.h`): 13 load functions read from MySQL into linked lists. 13 save functions sync linked lists to MySQL via `INSERT ... ON DUPLICATE KEY UPDATE`. Save wraps each chain in a transaction with rollback on error.
- **Authentication** (`Login.h`, `SHA-256.h`): Salted SHA-256 with 10000 iterations (backward-compatible with legacy 1000-iteration hashes via `verifyPasswordCompat()`), account lockout after 5 failures. Admin registration API Key has 3-level priority: env var `HIS_ADMIN_API_KEY` → `Data/AdminAPIKey.txt` → default `88888888`.
- **UI** (`UI.h/cpp`): 80+ menu functions, 20+ input validation functions, `LogManager` singleton, Unicode box-drawing, CJK width, color output. Safe `clearScreen()` using Win32 `FillConsoleOutputCharacter`+`SetConsoleCursorPosition` (no `system()`). Generic `displayChainByDept<T>` and `displayChainByFilter<T>` templates for reusable linked-list traversal. Six `printXxxCard()` functions for consistent entity display.
- **Admin Reports** (`AdminReports.cpp`): 6 report functions (`showDepartmentReport`, `showDoctorWorkloadReport`, `showPatientReport`, `showBedUtilizationReport`, `showMedicineInventoryReport`, `showDataAnalysisReport`) use SQL aggregation when DB is connected, with linked-list fallback. `Admin.cpp` split into 4 files: `Admin.cpp` (auth+shared), `AdminRecords.cpp` (7 record-type CRUD), `AdminUsers.cpp` (4 user-type CRUD), `AdminReports.cpp` (reports).
- **Data Analysis** (`DataAnalysis.h`): Holt-Winters prediction, Z-score anomaly detection, bed utilization analysis.
- **Drug Safety** (`DrugSafety.h`): Tokenized drug interaction detection (30 pairs in `Data/DrugData/interactions.txt`). `tokenizeDrugName()` splits drug names by delimiters (`/`, `+`, `-`, etc.) for exact constituent matching — prevents false positives like "阿莫西林克拉维酸钾" matching "阿莫西林". Also checks patient allergy history against prescribed drug constituents.
- **Entity Repository** (`EntityRepository.h`): Generic template for O(1) ID lookup via `std::unordered_map`. `getIdField()` returns `const std::string&` (ID immutability through repo). Template specializations for `Patient` (`patientID`), `Registration` (`registrationID`), and `NursingRecord` (`recordID`). Manages lifecycle for 3 chains: NursingRecord, Patient, Registration.
- **Python AI Service** (`ai_service/`): FastAPI REST API v2.0 on port 5001. LangChain + OpenAI-compatible LLM (default: Qwen on Aliyun Bailian). Multi-strategy routing (traditional / llm / auto) for predictions, anomaly detection, and bed optimization. RAG knowledge base with faiss vector search and async document upload. Pydantic validation, YAML config (`config.yaml`) with environment variable overrides. Dockerfile for containerized deployment. Start with `cd ai_service && pip install -r requirements.txt && python main.py`.
- **AI Query Client** (`AIQueryClient.h`): C++ HTTP client via WinSock2. Supports both GET and POST (`httpGet`/`httpPost`) requests to AI service v2.0. Provides `getPredictions()`, `getAnomalies()`, `getMedicines()`, `getBedOptimization()`, `getDashboard()`, `ragChat()`, and `downloadChart()` methods. Overloaded methods accept strategy and department parameters. Admin menu option 9.

### Data Flow

1. `main()` connects to MySQL via `GetDB().connect()`
2. Loads all 13 chains from MySQL via `LoadData` functions
3. If admin chain is empty, force-create first admin (API Key required)
4. User logs in → role-specific menu loop
5. All mutations operate on in-memory linked lists
6. On normal exit: `SaveData` functions sync all chains to MySQL → cleanup → disconnect
7. On crash/signal: `emergencySave()` attempts save (MySQL C API may not be signal-safe)

## Language

All UI strings, data files, comments, and documentation are in Chinese (Simplified). Code identifiers are in English. Keep new user-facing text in Chinese.

## Debugging

VS Code config in `.vscode/`:
- **"调试 HIS 程序"**: Builds with pre-launch task, runs `build/Debug/his.exe` in external terminal with `cwd: build/`
- **Warning**: `postDebugTask` runs `Remove-Item -Recurse -Force build/*` — deletes build dir including `libmysql.dll`

- **CMakeLists.txt** supports both Debug (default) and Release builds. Release adds `/GL` (whole-program optimization) + `/LTCG` (link-time code generation) for MSVC.

## Build Requirements

- CMake 3.24+
- MSVC (Visual Studio 2017 15.8+) with C++17 support (`/std:c++17`)
- MSVC `/utf-8` flag is set in CMakeLists.txt for Chinese source and data files
- MySQL 8.0 Server (running on `localhost:3307`, user `root`, password `123456`, database `his_db`)
- MySQL C Client library: `E:/MySQL/8-0/include/` (headers) + `E:/MySQL/8-0/lib/` (`libmysql.lib`, `libmysql.dll`)
- Python 3.8+ with `fastapi`, `uvicorn`, `pydantic`, `langchain`, `langchain-openai`, `langchain-community`, `langgraph`, `faiss-cpu`, `sentence-transformers`, `jieba`, `scikit-learn`, `pymysql`, `matplotlib`, `pandas`, `numpy`, `pyyaml`, `pdfplumber`, `python-docx` (for AI service)

## Recent Improvements (2026.5.30)

### AI Service v2.0 Refactor (2026.6.6)

- **Flask → FastAPI**: Full framework migration. Entry point `app.py` → `main.py`. All endpoints now POST with JSON body and `/api/v2/` prefix. Pydantic models for request/response validation.
- **LangChain + LLM Integration**: `langchain-openai` ChatOpenAI client (default: Qwen on Aliyun Bailian). Dynamic Few-shot Prompt via TF-IDF similar case retrieval (`prompt_builder.py`). OpenAI-compatible API interface.
- **Multi-strategy Routing**: Every analysis endpoint supports `strategy` parameter: `traditional` (Holt-Winters/Z-score/stats) → `llm` (LangChain prompt → LLM inference) → `auto` (data-point threshold dispatch, default ≥12 uses traditional).
- **RAG Knowledge Base**: `rag_engine.py` using `sentence-transformers` (BAAI/bge-small-zh-v1.5) for embedding + `faiss-cpu` for vector search. Async document upload with `BackgroundTasks` (PDF via pdfplumber, TXT). Sliding-window chunking with overlap.
- **Architecture Restructure**: Flat 4-file layout → `core/` (config, database pool, security, response, exceptions, llm_provider) + `routers/` (6 route modules) + `services/` (9 service modules) + `tests/`. Unified response format: `{request_id, data, response_code, response_msg, process_status, processing_time}`.
- **Configuration Center**: `config.yaml` YAML configuration with environment variable overrides (`HIS_LLM_API_KEY`, `HIS_API_KEY`, `HIS_DB_HOST`, `HIS_DB_PORT`, `HIS_DB_PASSWORD`). Settings singleton pattern.
- **AIQueryClient C++ Upgrade**: New `httpPost()` method for POST requests. New `ragChat()` for RAG Q&A. Overloaded methods for strategy/department parameters. All calls adapted to v2 API.
- **Dockerfile**: `python:3.11-slim` + `uvicorn` deployment. Standard containerized FastAPI deployment.
- **Dependencies Updated**: Removed `flask`, `flask-cors`. Added `fastapi`, `uvicorn`, `pydantic`, `langchain`, `langchain-openai`, `langchain-community`, `langgraph`, `faiss-cpu`, `sentence-transformers`, `jieba`, `scikit-learn`, `pdfplumber`, `python-docx`, `pyyaml`, `python-multipart`.

### API Endpoint Mapping (v1 → v2)

| v1 (Flask GET) | v2 (FastAPI POST) |
|---|---|
| `GET /api/stats` | `POST /api/v2/dashboard` |
| `GET /api/predictions` | `POST /api/v2/predictions` |
| `GET /api/anomalies` | `POST /api/v2/anomalies` |
| `GET /api/medicines` | `POST /api/v2/dashboard` (medicine data in dashboard) |
| `GET /api/dashboard` | `POST /api/v2/dashboard` |
| `GET /api/beds` | `POST /api/v2/beds` |
| `GET /api/charts/<type>` | `GET /api/v2/charts/{type}` |
| _(new)_ | `POST /api/v2/rag/chat` |
| _(new)_ | `POST /api/v2/rag/upload` |
| — | `GET /health`, `GET /` |

### Security
- **Safe clearScreen**: Replaced `system("cls")` with Win32 `FillConsoleOutputCharacter` + `SetConsoleCursorPosition` (Windows) / ANSI `\033[2J\033[1;1H` (Linux).
- **API Key 3-level priority**: `HIS_ADMIN_API_KEY` env var → `Data/AdminAPIKey.txt` → default `88888888`. Same pattern for AI service (`HIS_API_KEY`).
- **Removed `CLIENT_MULTI_STATEMENTS`**: MySQL connection no longer allows multi-statement execution.
- **SHA-256 10000 iterations**: With backward-compatible `verifyPasswordCompat()` fallback to legacy 1000.
- **AI service**: FastAPI `host='127.0.0.1'`, `Depends(verify_api_key)` on all non-health endpoints via `X-API-Key` header.

### Bug Fixes
- **PNG download binary-safe**: `AIQueryClient::downloadChart()` uses `Content-Length` parsing, binary file I/O (`std::ios::binary`), and larger buffer (8KB).
- **EntityRepository ID immutability**: `getIdField()` returns `const std::string&` for all 3 specializations.
- **Drug interaction tokenized matching**: `tokenizeDrugName()` + `isConstituentOf()` for exact constituent matching, fixes false positives from substring matching.
- **dbDateTime SQL NULL**: Uses `\x01` prefix convention so `buildSql()` injects raw SQL NULL without quoting.

### Code Quality
- **Admin.cpp split**: 8586-line monolith → 4 files: `Admin.cpp` (64 lines, auth/shared), `AdminRecords.cpp` (~5200 lines, 7 record types), `AdminUsers.cpp` (~4000 lines, 4 user types), `AdminReports.cpp` (~1000 lines, 6 reports).
- **printXxxCard() functions**: 6 centralized entity display functions in `UI.h/cpp` — `printRegistrationCard`, `printConsultationCard`, `printExaminationCard`, `printMedicationRecordCard`, `printHospitalizationCard`, `printMedicineCard`.
- **displayChainByFilter<T>**: Generic linked-list traversal template accepting lambda predicates. Complements existing `displayChainByDept<T>`.
- **NursingRecord.h**: Moved `NursingRecord` struct from `User.h` to dedicated `Head/Entities/NursingRecord.h`.
- **CMake Release build**: Supports `-DCMAKE_BUILD_TYPE=Release` with MSVC `/GL` + `/LTCG`.
- **Dead code removed**: `fillUserBase()` in `LoadData.cpp`.
