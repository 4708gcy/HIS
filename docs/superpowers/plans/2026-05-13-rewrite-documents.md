# Rewrite Project Documents Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Completely rewrite 题签.md and 总结报告.md based on actual project code, following the course template and senior report style.

**Architecture:** Two documentation files rewritten from scratch based on code analysis of 19 header files, 13 source files, and main.cpp. The 总结报告 follows the 2023级 template structure (cover table + report body) and senior report style (Roman numeral sections with numbered sub-sections and screenshots).

**Tech Stack:** Markdown documentation, Mermaid diagrams

---

### Task 1: Rewrite 题签.md

**Files:**
- Rewrite: `Document/题签.md`

- [ ] **Step 1: Write 题签.md from scratch based on actual code**

Key changes from current version:
- Remove all MedicineFlow references (struct, `mfl` prefix, `medicine_flow.txt`)
- Add `LinkedList<T>` template class for nested collections
- Add VitalSigns struct description in Examination
- Fix examination item names to match code (e.g., "体温测量" not "体温")
- Fix doctor title names to match code (e.g., "实习医生" not "实习医师")
- Add bed ID format description (科室代码-区域-病房类型-病房号-床位号)
- Update medical/resource data count from 8 to 7 types
- Update file path table from 13 to 12 entries
- Add emergency save dual-pointer mechanism description

- [ ] **Step 2: Verify accuracy**

Cross-check fee tables, struct fields, enum values, file paths against actual code.

---

### Task 2: Rewrite 总结报告.md

**Files:**
- Rewrite: `Document/总结报告.md`

- [ ] **Step 1: Write 总结报告.md following template + senior report style**

Structure:
- Cover page with member table (template format)
- I. 小组分工合作情况 (Git workflow, CMake, development log)
- II. 程序特色功能概述 (flowchart, 7 key features)
- III. 程序完整功能详述 (7 sub-sections for each module)
- IV. 测试与验证 (6 test categories with tables)
- V. 总结 (technical summary table)

Screenshots: Leave as visible placeholders with step-by-step instructions.

- [ ] **Step 2: Verify completeness**

Ensure all modules are covered, all features described accurately.

---

### Task 3: Delete obsolete files

**Files:**
- Delete: `Document/总结报告_精简版.md`
- Delete: `Document/总结报告_精简版.docx`

- [ ] **Step 1: Delete both files**
