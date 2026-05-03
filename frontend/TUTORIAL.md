# HIS 前后端打通教程

本文档从零记录如何搭建 Vue 3 前端项目并与 C++ REST API 后端联调的完整步骤，所有内容均基于本项目实际代码。

---

## 一、后端准备

前端依赖后端已运行，确保 C++ 后端服务器正在监听 `http://localhost:8080`：

```bash
cd build
cmake --build . --config Debug --target his_server
./Debug/his_server.exe
```

后端提供 REST API，例如 `GET http://localhost:8080/api/departments` 返回科室列表。前端需要调用这些接口。

---

## 二、创建前端项目目录结构

不使用 `npm create vue` 脚手架，手动创建项目，便于理解每一层的作用：

```bash
mkdir -p frontend/src/{api,store,router,styles,components}
mkdir -p frontend/src/views/{admin,doctor,nurse,pharmacist,patient}
```

目录职责：

| 目录 | 用途 |
|------|------|
| `src/api/` | 封装所有 HTTP 请求（axios 实例 + 各角色 API 模块） |
| `src/store/` | Pinia 状态管理（用户登录态、JWT token、角色信息） |
| `src/router/` | Vue Router 路由配置（角色分流 + 路由守卫） |
| `src/styles/` | 全局 CSS 变量和样式（蓝白医疗主题） |
| `src/views/` | 页面组件，按角色子目录分类 |

---

## 三、创建 package.json

手动编写依赖声明：

```json
{
  "name": "his-frontend",
  "private": true,
  "version": "1.0.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview"
  },
  "dependencies": {
    "vue": "^3.5.13",
    "vue-router": "^4.5.0",
    "pinia": "^2.3.0",
    "axios": "^1.7.9",
    "element-plus": "^2.9.1",
    "@element-plus/icons-vue": "^2.3.1"
  },
  "devDependencies": {
    "@vitejs/plugin-vue": "^5.2.1",
    "vite": "^6.0.0"
  }
}
```

然后安装依赖：

```bash
cd frontend
npm install
```

**各包作用：**

| 包 | 用途 |
|----|------|
| `vue` | 前端框架（Composition API + `<script setup>`） |
| `vite` | 构建工具，提供开发服务器（热更新）和生产构建 |
| `vue-router` | 路由管理，实现页面跳转和角色分流 |
| `pinia` | 状态管理，替代 Vuex，更简洁的 API |
| `axios` | HTTP 客户端，用于发送请求到后端 |
| `element-plus` | UI 组件库（表格、表单、对话框等），内置中文 locale |
| `@element-plus/icons-vue` | Element Plus 图标库 |

**为什么选择 Element Plus？**

- 内置 `zh-cn` locale，日期/分页等组件原生中文支持
- `el-table`/`el-form`/`el-dialog` API 简洁，适合表格密集型管理系统
- 图标库开箱即用，无需额外引入

---

## 四、创建 vite.config.js（前后端关联的关键）

```js
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  plugins: [vue()],
  server: {
    port: 3000,
    proxy: {
      '/api': {
        target: 'http://localhost:8080',
        changeOrigin: true
      }
    }
  }
})
```

**这是前后端打通的核心配置。** 原理：

1. 前端代码中请求 `/api/auth/login`（相对路径）
2. Vite 开发服务器拦截以 `/api` 开头的请求
3. 将请求转发到 `http://localhost:8080/api/auth/login`
4. 后端响应返回给 Vite，Vite 再返回给前端浏览器

**好处**：开发时无需处理 CORS（跨域），因为浏览器看到的是同源请求（都是 localhost:3000）。生产环境则通过 Nginx 反向代理实现相同效果。

---

## 五、创建 index.html（SPA 入口）

```html
<!DOCTYPE html>
<html lang="zh-CN">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>HIS 医院信息系统</title>
  </head>
  <body>
    <div id="app"></div>
    <script type="module" src="/src/main.js"></script>
  </body>
</html>
```

`<script type="module">` 指向 `src/main.js`，Vite 从这里开始打包。

---

## 六、创建 src/main.js（应用入口）

```js
import { createApp } from 'vue'
import { createPinia } from 'pinia'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import zhCn from 'element-plus/es/locale/lang/zh-cn'
import * as ElementPlusIconsVue from '@element-plus/icons-vue'
import App from './App.vue'
import router from './router'
import './styles/global.css'

const app = createApp(App)

// 全局注册所有 Element Plus 图标
for (const [key, component] of Object.entries(ElementPlusIconsVue)) {
  app.component(key, component)
}

app.use(createPinia())
app.use(router)
app.use(ElementPlus, {
  locale: zhCn,
  size: 'default'
})
app.mount('#app')
```

**关键点：**

- 全局注册所有图标：后续页面中可以直接使用 `<User />`、`<Delete />` 等标签，无需每个页面单独 import
- `locale: zhCn`：确保 `el-pagination`、`el-date-picker` 等组件显示中文
- `size: 'default'`：设置 Element Plus 组件默认尺寸
- `import './styles/global.css'`：引入自定义 CSS 变量主题

---

## 七、创建 src/App.vue（根组件）

```vue
<template>
  <router-view />
</template>
```

最简单的根组件，只做一件事：渲染当前路由匹配的页面。

---

## 八、创建 src/styles/global.css（全局主题）

本项目定义了 167 行的完整 CSS 变量主题系统：

```css
/* === HIS Theme: Pure & Clinical === */

:root {
  /* Primary palette — soft medical blue */
  --his-primary: #1e88e5;
  --his-primary-light: #e8f1fb;
  --his-primary-dark: #1565c0;
  --his-primary-hover: #1976d2;

  /* Neutrals */
  --his-bg: #f6f8fc;
  --his-surface: #ffffff;
  --his-surface-alt: #f0f4fa;
  --his-border: #e4e9f2;
  --his-text: #1a2b4a;
  --his-text-secondary: #5a6e8a;
  --his-text-muted: #96a3b8;

  /* Shadows */
  --his-shadow-sm: 0 1px 3px rgba(30, 136, 229, 0.06);
  --his-shadow-md: 0 4px 12px rgba(30, 136, 229, 0.08);
  --his-shadow-lg: 0 8px 24px rgba(30, 136, 229, 0.12);

  /* Transitions */
  --his-transition: 0.25s cubic-bezier(0.4, 0, 0.2, 1);

  /* Sidebar */
  --his-sidebar-bg: #ffffff;
  --his-sidebar-text: #3d5a80;
  --his-sidebar-active-bg: #e8f1fb;
  --his-sidebar-active-text: #1e88e5;
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

html, body, #app {
  height: 100%;
  font-family: 'Microsoft YaHei', 'PingFang SC', sans-serif;
  color: var(--his-text);
  background: var(--his-bg);
}

/* Element Plus 组件覆盖 */
.el-button--primary {
  --el-button-bg-color: var(--his-primary) !important;
  --el-button-border-color: var(--his-primary) !important;
  --el-button-hover-bg-color: var(--his-primary-hover) !important;
  --el-button-active-bg-color: var(--his-primary-dark) !important;
}

.el-table {
  --el-table-header-bg-color: var(--his-surface-alt) !important;
  --el-table-border-color: var(--his-border) !important;
  --el-table-row-hover-bg-color: var(--his-primary-light) !important;
}

.el-card {
  border-radius: 10px !important;
  border: 1px solid var(--his-border) !important;
  transition: box-shadow var(--his-transition);
}

.el-card:hover {
  box-shadow: var(--his-shadow-md);
}

/* 工具类 */
.fade-in {
  animation: fadeIn 0.4s ease-out both;
}

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(8px); }
  to { opacity: 1; transform: translateY(0); }
}
```

CSS 变量的好处：修改一处变量，所有使用它的组件自动更新。

---

## 九、创建 src/api/index.js（axios 实例 + JWT 拦截器）

**这是前后端打通的第二步（第一步是 Vite proxy）。**

```js
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { useUserStore } from '../store/user'
import router from '../router'

// 创建 axios 实例
const request = axios.create({
  baseURL: '/api',       // 所有请求自动加上 /api 前缀
  timeout: 10000         // 10 秒超时
})

// 请求拦截器：自动附加 JWT token
request.interceptors.request.use(config => {
  const store = useUserStore()
  if (store.token) {
    config.headers.Authorization = `Bearer ${store.token}`
  }
  return config
})

// 响应拦截器：统一处理响应和错误
request.interceptors.response.use(
  response => {
    const data = response.data
    // 业务错误（后端返回 code !== 200）
    if (data?.code !== 200) {
      ElMessage.error(data?.message || '请求失败')
      return Promise.reject(new Error(data?.message || '请求失败'))
    }
    return data  // 自动解包，业务代码直接拿到 {code, message, data}
  },
  error => {
    if (error.response?.status === 401) {
      // token 过期或无效，清除登录态并跳转登录页
      const store = useUserStore()
      store.logout()
      router.push('/login')
      return Promise.reject(new Error('登录已过期，请重新登录'))
    } else if (error.response?.status === 404) {
      return Promise.reject(new Error('请求的资源不存在'))
    } else if (error.response?.status === 403) {
      return Promise.reject(new Error('没有权限访问该资源'))
    } else if (error.code === 'ECONNABORTED') {
      return Promise.reject(new Error('请求超时，请稍后重试'))
    }
    return Promise.reject(new Error(error.response?.data?.message || error.message || '网络错误'))
  }
)

export default request
```

**拦截器的价值：**

- **请求拦截器**：每次请求自动带 JWT token，业务代码不需要手动处理认证
- **响应拦截器**：
  - 自动解包 `response.data`，业务代码直接拿到 `{code, message, data}`
  - 后端 `code !== 200` 时自动弹出错误消息并拒绝 Promise
  - 401 自动清除登录态并跳转登录页
  - 403/404/超时 细分错误处理

---

## 十、创建各角色 API 模块

按角色拆分，每个文件封装该角色的所有 API 调用。

### src/api/auth.js（登录/注册/修改密码）

```js
import request from './index'

export function login(data) {
  return request.post('/auth/login', data)
}

export function register(data) {
  return request.post('/auth/register', data)
}

export function changePassword(oldPassword, newPassword) {
  return request.put('/auth/change-password', { oldPassword, newPassword })
}
```

### src/api/common.js（公共接口）

```js
import request from './index'

export function getDepartments() {
  return request.get('/departments')
}

export function getFeeStandards() {
  return request.get('/fee-standards')
}

export function getExaminationItems() {
  return request.get('/examination-items')
}
```

### src/api/admin.js（管理员 API — 完整清单）

```js
import request from './index'

// 医生 CRUD
export function getDoctors(params) {
  return request.get('/admin/doctors', { params })
}
export function getDoctor(id) {
  return request.get(`/admin/doctors/${id}`)
}
export function createDoctor(data) {
  return request.post('/admin/doctors', data)
}
export function updateDoctor(id, data) {
  return request.put(`/admin/doctors/${id}`, data)
}
export function deleteDoctor(id) {
  return request.delete(`/admin/doctors/${id}`)
}

// 护士 CRUD（模式同上）
export function getNurses(params) { return request.get('/admin/nurses', { params }) }
export function getNurse(id) { return request.get(`/admin/nurses/${id}`) }
export function createNurse(data) { return request.post('/admin/nurses', data) }
export function updateNurse(id, data) { return request.put(`/admin/nurses/${id}`, data) }
export function deleteNurse(id) { return request.delete(`/admin/nurses/${id}`) }

// 药剂师 CRUD
export function getPharmacists(params) { return request.get('/admin/pharmacists', { params }) }
export function getPharmacist(id) { return request.get(`/admin/pharmacists/${id}`) }
export function createPharmacist(data) { return request.post('/admin/pharmacists', data) }
export function updatePharmacist(id, data) { return request.put(`/admin/pharmacists/${id}`, data) }
export function deletePharmacist(id) { return request.delete(`/admin/pharmacists/${id}`) }

// 患者 CRUD
export function getPatients(params) { return request.get('/admin/patients', { params }) }
export function getPatient(id) { return request.get(`/admin/patients/${id}`) }
export function createPatient(data) { return request.post('/admin/patients', data) }
export function updatePatient(id, data) { return request.put(`/admin/patients/${id}`, data) }
export function deletePatient(id) { return request.delete(`/admin/patients/${id}`) }

// 管理员列表
export function getAdmins(params) { return request.get('/admin/admins', { params }) }

// 挂号记录（查看/创建/状态修改/删除）
export function getRegistrations(params) { return request.get('/admin/registrations', { params }) }
export function getRegistration(id) { return request.get(`/admin/registrations/${id}`) }
export function createRegistration(data) { return request.post('/admin/registrations', data) }
export function updateRegistrationStatus(id, status) {
  return request.put(`/admin/registrations/${id}/status`, { status })
}
export function deleteRegistration(id) { return request.delete(`/admin/registrations/${id}`) }

// 看诊记录
export function getConsultations(params) { return request.get('/admin/consultations', { params }) }
export function getConsultation(id) { return request.get(`/admin/consultations/${id}`) }
export function createConsultation(data) { return request.post('/admin/consultations', data) }
export function updateConsultationStatus(id, status) {
  return request.put(`/admin/consultations/${id}/status`, { status })
}
export function deleteConsultation(id) { return request.delete(`/admin/consultations/${id}`) }

// 检查记录
export function getExaminations(params) { return request.get('/admin/examinations', { params }) }
export function getExamination(id) { return request.get(`/admin/examinations/${id}`) }
export function createExamination(data) { return request.post('/admin/examinations', data) }
export function deleteExamination(id) { return request.delete(`/admin/examinations/${id}`) }

// 住院记录
export function getHospitalizations(params) { return request.get('/admin/hospitalizations', { params }) }
export function getHospitalization(id) { return request.get(`/admin/hospitalizations/${id}`) }
export function createHospitalization(data) { return request.post('/admin/hospitalizations', data) }
export function deleteHospitalization(id) { return request.delete(`/admin/hospitalizations/${id}`) }

// 用药记录
export function getMedicationRecords(params) { return request.get('/admin/medication-records', { params }) }
export function createMedicationRecord(data) { return request.post('/admin/medication-records', data) }
export function deleteMedicationRecord(id) { return request.delete(`/admin/medication-records/${id}`) }

// 药品管理
export function getMedicines(params) { return request.get('/admin/medicines', { params }) }
export function getMedicine(id) { return request.get(`/admin/medicines/${id}`) }
export function createMedicine(data) { return request.post('/admin/medicines', data) }
export function updateMedicine(id, data) { return request.put(`/admin/medicines/${id}`, data) }
export function deleteMedicine(id) { return request.delete(`/admin/medicines/${id}`) }

// 床位管理
export function getBeds(params) { return request.get('/admin/beds', { params }) }
export function createBed(data) { return request.post('/admin/beds', data) }
export function deleteBed(id) { return request.delete(`/admin/beds/${id}`) }

// 账号状态管理（激活/封锁）
export function updateAccountStatus(id, role, active) {
  return request.put(`/admin/account/${id}/status`, { role, active })
}

// 管理员个人信息
export function updateAdminProfile(data) {
  return request.put('/admin/profile', data)
}
```

**为什么按角色拆分？**
后端 API 本身按角色前缀隔离（`/api/admin/*`、`/api/doctor/*` 等），前端按角色拆分保持一致性。每个角色对应的页面只需 import 自己角色的 API 文件。

---

## 十一、创建 src/store/user.js（Pinia 状态管理）

```js
import { defineStore } from 'pinia'
import { ref, computed } from 'vue'

export const useUserStore = defineStore('user', () => {
  // 状态（从 localStorage 初始化，刷新页面不丢失登录态）
  const token = ref(localStorage.getItem('token') || '')
  const userID = ref(localStorage.getItem('userID') || '')
  const username = ref(localStorage.getItem('username') || '')
  const role = ref(parseInt(localStorage.getItem('role') || '0'))
  const roleStr = ref(localStorage.getItem('roleStr') || '')

  // 计算属性
  const isLoggedIn = computed(() => !!token.value)

  const roleNames = { 1: '管理员', 2: '医生', 3: '护士', 4: '药剂师', 5: '患者' }
  const roleName = computed(() => roleNames[role.value] || '')

  // 登录：保存 token 和用户信息到状态和 localStorage
  function setLogin(data) {
    token.value = data.token
    userID.value = data.userID
    username.value = data.username
    role.value = data.role
    roleStr.value = data.roleStr
    localStorage.setItem('token', data.token)
    localStorage.setItem('userID', data.userID)
    localStorage.setItem('username', data.username)
    localStorage.setItem('role', String(data.role))
    localStorage.setItem('roleStr', data.roleStr)
  }

  // 登出：清空状态和 localStorage
  function logout() {
    token.value = ''
    userID.value = ''
    username.value = ''
    role.value = 0
    roleStr.value = ''
    localStorage.removeItem('token')
    localStorage.removeItem('userID')
    localStorage.removeItem('username')
    localStorage.removeItem('role')
    localStorage.removeItem('roleStr')
  }

  return { token, userID, username, role, roleStr, isLoggedIn, roleName, setLogin, logout }
})
```

**关键点：**

- **5 个状态字段**：`token`、`userID`、`username`、`role`（数字）、`roleStr`（字符串）
- `role` 是前端 1-based 数字（1=管理员...5=患者），与后端 0-based 不同，由后端 API 转换
- `localStorage` 持久化：刷新页面不丢失登录态，JWT 24h 过期保护安全性
- **Composition API 风格**：直接写函数，比 Options API 的 `state/mutations/actions` 更直观

---

## 十二、创建 src/router/index.js（路由 + 角色守卫）

```js
import { createRouter, createWebHistory } from 'vue-router'
import { useUserStore } from '../store/user'

const routes = [
  {
    path: '/login',
    name: 'Login',
    component: () => import('../views/Login.vue')
  },
  {
    path: '/register',
    name: 'Register',
    component: () => import('../views/Register.vue')
  },
  {
    path: '/',
    component: () => import('../views/Layout.vue'),
    redirect: '/dashboard',
    meta: { requiresAuth: true },
    children: [
      // Dashboard（仅管理员）
      { path: 'dashboard', name: 'Dashboard', component: () => import('../views/admin/Dashboard.vue'), meta: { roles: [1] } },
      // 管理员路由（13 个）
      ...[
        { path: 'admin/doctors', component: () => import('../views/admin/Doctors.vue'), meta: { roles: [1] } },
        { path: 'admin/nurses', component: () => import('../views/admin/Nurses.vue'), meta: { roles: [1] } },
        { path: 'admin/pharmacists', component: () => import('../views/admin/Pharmacists.vue'), meta: { roles: [1] } },
        { path: 'admin/patients', component: () => import('../views/admin/Patients.vue'), meta: { roles: [1] } },
        { path: 'admin/registrations', component: () => import('../views/admin/Registrations.vue'), meta: { roles: [1] } },
        { path: 'admin/consultations', component: () => import('../views/admin/Consultations.vue'), meta: { roles: [1] } },
        { path: 'admin/examinations', component: () => import('../views/admin/Examinations.vue'), meta: { roles: [1] } },
        { path: 'admin/hospitalizations', component: () => import('../views/admin/Hospitalizations.vue'), meta: { roles: [1] } },
        { path: 'admin/medication-records', component: () => import('../views/admin/MedicationRecords.vue'), meta: { roles: [1] } },
        { path: 'admin/medicines', component: () => import('../views/admin/Medicines.vue'), meta: { roles: [1] } },
        { path: 'admin/beds', component: () => import('../views/admin/Beds.vue'), meta: { roles: [1] } },
        { path: 'admin/profile', component: () => import('../views/admin/Profile.vue'), meta: { roles: [1] } }
      ],
      // 医生路由（4 个）
      ...[
        { path: 'doctor/registrations', component: () => import('../views/doctor/Registrations.vue'), meta: { roles: [2] } },
        { path: 'doctor/consultations', component: () => import('../views/doctor/Consultations.vue'), meta: { roles: [2] } },
        { path: 'doctor/examinations', component: () => import('../views/doctor/Examinations.vue'), meta: { roles: [2] } },
        { path: 'doctor/profile', component: () => import('../views/doctor/Profile.vue'), meta: { roles: [2] } }
      ],
      // 护士路由（4 个）
      ...[
        { path: 'nurse/hospitalizations', component: () => import('../views/nurse/Hospitalizations.vue'), meta: { roles: [3] } },
        { path: 'nurse/examinations', component: () => import('../views/nurse/Examinations.vue'), meta: { roles: [3] } },
        { path: 'nurse/beds', component: () => import('../views/nurse/Beds.vue'), meta: { roles: [3] } },
        { path: 'nurse/profile', component: () => import('../views/nurse/Profile.vue'), meta: { roles: [3] } }
      ],
      // 药剂师路由（3 个）
      ...[
        { path: 'pharmacist/medication-records', component: () => import('../views/pharmacist/MedicationRecords.vue'), meta: { roles: [4] } },
        { path: 'pharmacist/medicines', component: () => import('../views/pharmacist/Medicines.vue'), meta: { roles: [4] } },
        { path: 'pharmacist/profile', component: () => import('../views/pharmacist/Profile.vue'), meta: { roles: [4] } }
      ],
      // 患者路由（6 个）
      ...[
        { path: 'patient/registrations', component: () => import('../views/patient/Registrations.vue'), meta: { roles: [5] } },
        { path: 'patient/consultations', component: () => import('../views/patient/Consultations.vue'), meta: { roles: [5] } },
        { path: 'patient/examinations', component: () => import('../views/patient/Examinations.vue'), meta: { roles: [5] } },
        { path: 'patient/medication-records', component: () => import('../views/patient/MedicationRecords.vue'), meta: { roles: [5] } },
        { path: 'patient/hospitalizations', component: () => import('../views/patient/Hospitalizations.vue'), meta: { roles: [5] } },
        { path: 'patient/profile', component: () => import('../views/patient/Profile.vue'), meta: { roles: [5] } }
      ]
    ]
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

// 路由守卫：每次导航前检查登录状态和角色权限
router.beforeEach((to, from, next) => {
  const store = useUserStore()

  if (to.meta.requiresAuth && !store.isLoggedIn) {
    // 未登录 → 跳转登录页
    next('/login')
  } else if ((to.path === '/login' || to.path === '/register') && store.isLoggedIn) {
    // 已登录用户访问登录/注册页 → 重定向
    next('/dashboard')
  } else if (to.meta.roles && !to.meta.roles.includes(Number(store.role))) {
    // 角色不匹配 → 重定向到对应角色的默认页面
    const roleRedirects = {
      1: '/dashboard',
      2: '/doctor/registrations',
      3: '/nurse/hospitalizations',
      4: '/pharmacist/medication-records',
      5: '/patient/registrations'
    }
    next(roleRedirects[Number(store.role)] || '/dashboard')
  } else {
    next()
  }
})

export default router
```

**关键点：**

- `...[]` 展开语法：将同角色路由分组，便于阅读和维护
- `meta: { roles: [...] }`：路由级别的角色权限控制
- `redirect: '/'` → `/dashboard`：根路径默认跳转到 Dashboard
- **路由守卫三层检查**：
  1. 未登录 → `/login`
  2. 已登录访问登录/注册页 → `/dashboard`
  3. 角色不匹配 → 重定向到角色默认页面
- `Number(store.role)`：确保比较时类型一致（localStorage 读回是字符串）
- **动态导入 `() => import(...)`**：路由懒加载，只有访问到对应页面时才加载对应的 JS chunk

---

## 十三、创建 src/views/Layout.vue（主布局）

采用 Element Plus 的 `el-container` 布局，侧边栏可折叠：

```text
┌──────────────────────────────────────────────────┐
│  Header（折叠按钮 + 面包屑 | 角色标签 用户名 退出） │
├─────────┬────────────────────────────────────────┤
│         │                                        │
│ Sidebar │  Main Content（router-view + 动画）     │
│ (可折叠)│                                        │
│         │                                        │
└─────────┴────────────────────────────────────────┘
```

**核心功能：**

- **侧边栏折叠**：`isCollapse` 状态控制宽度（240px ↔ 64px），菜单项只显示图标
- **角色菜单隔离**：`v-if="store.role === X"` 控制不同角色看到不同菜单
- **面包屑导航**：Header 左侧显示角色名 + 当前页面名
- **页面切换动画**：`<transition name="fade-view">` 实现平滑过渡
- **用户信息展示**：Header 右侧显示角色标签、用户名、用户 ID

**关键代码结构：**

```vue
<template>
  <el-container class="layout-container">
    <el-aside :width="isCollapse ? '64px' : '240px'" class="sidebar">
      <!-- Logo -->
      <div class="logo">
        <svg>医疗十字图标</svg>
        <span v-if="!isCollapse">HIS</span>
      </div>

      <!-- 菜单 -->
      <el-menu :default-active="$route.path" :collapse="isCollapse" router unique-opened>
        <!-- 管理员菜单 -->
        <template v-if="store.role === 1">
          <el-menu-item index="/dashboard">首页概览</el-menu-item>
          <el-sub-menu index="admin-users">
            <template #title>人员管理</template>
            <el-menu-item index="/admin/doctors">医生管理</el-menu-item>
            <!-- 护士/药剂师/患者管理 -->
          </el-sub-menu>
          <el-sub-menu index="admin-records">
            <template #title>医疗记录</template>
            <el-menu-item index="/admin/registrations">挂号记录</el-menu-item>
            <!-- 看诊/检查/住院/用药记录 -->
          </el-sub-menu>
          <el-sub-menu index="admin-resources">
            <template #title>资源管理</template>
            <el-menu-item index="/admin/medicines">药品管理</el-menu-item>
            <el-menu-item index="/admin/beds">床位管理</el-menu-item>
          </el-sub-menu>
          <el-menu-item index="/admin/profile">个人信息</el-menu-item>
        </template>

        <!-- 医生/护士/药剂师/患者菜单（类似结构，用 v-if 隔离） -->
      </el-menu>
    </el-aside>

    <el-container class="main-area">
      <el-header class="layout-header">
        <div class="header-left">
          <!-- 折叠按钮 -->
          <el-icon @click="isCollapse = !isCollapse"><Fold />/<Expand /></el-icon>
          <!-- 面包屑 -->
          <el-breadcrumb>
            <el-breadcrumb-item>{{ store.roleName }}</el-breadcrumb-item>
            <el-breadcrumb-item>{{ 当前页面名 }}</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
        <div class="header-right">
          <el-tag>{{ store.roleName }}</el-tag>
          <span>{{ store.username }}({{ store.userID }})</span>
          <el-button @click="handleLogout">退出</el-button>
        </div>
      </el-header>

      <el-main class="layout-main" style="padding: 16px 20px;">
        <router-view v-slot="{ Component }">
          <transition name="fade-view" mode="out-in">
            <component :is="Component" />
          </transition>
        </router-view>
      </el-main>
    </el-container>
  </el-container>
</template>

<script setup>
import { ref, computed } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { useUserStore } from '../store/user'

const router = useRouter()
const route = useRoute()
const store = useUserStore()
const isCollapse = ref(false)

// 路由路径到页面标题的映射
const currentPageTitle = computed(() => {
  const map = {
    '/dashboard': '首页概览',
    '/admin/doctors': '医生管理', '/admin/nurses': '护士管理',
    // ... 其他路由映射
    '/patient/profile': '个人信息'
  }
  return map[route.path] || ''
})

function handleLogout() {
  store.logout()
  router.push('/login')
}
</script>
```

**CSS 关键点：**

- 白色侧边栏 + 蓝色激活态背景（`--his-sidebar-active-bg: #e8f1fb`）
- 菜单项圆角 8px、hover 浅蓝背景、激活时蓝色文字
- Header 高度 56px，带底部阴影
- `el-main` padding 为 `16px 20px`（从默认 20px 调整，修复底部空白问题）
- `fade-view` 页面切换动画：opacity + translateY

---

## 十四、创建 src/views/Login.vue（登录页）

```vue
<template>
  <div class="login-container">
    <!-- SVG 装饰背景 -->
    <svg class="bg-pattern" viewBox="0 0 1440 900" fill="none">
      <defs>
        <linearGradient id="bg" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#e3f2fd"/>
          <stop offset="50%" stop-color="#f6f8fc"/>
          <stop offset="100%" stop-color="#e8f5e9" stop-opacity="0.3"/>
        </linearGradient>
      </defs>
      <rect width="1440" height="900" fill="url(#bg)"/>
      <!-- 半透明圆形装饰 + 医疗十字符号 + 流动曲线 -->
    </svg>

    <div class="login-card fade-in">
      <div class="login-header">
        <div class="logo-icon">
          <svg>蓝色医疗十字图标</svg>
        </div>
        <h2>HIS 医院信息系统</h2>
        <p>请登录您的账号</p>
      </div>

      <el-form ref="formRef" :model="form" :rules="rules" @submit.prevent="handleLogin">
        <el-form-item prop="userID">
          <el-input v-model="form.userID" placeholder="请输入6位用户ID" size="large" prefix-icon="User" />
        </el-form-item>
        <el-form-item prop="password">
          <el-input v-model="form.password" type="password" show-password placeholder="请输入密码" size="large" prefix-icon="Lock" />
        </el-form-item>
        <el-form-item prop="role">
          <el-select v-model="form.role" placeholder="请选择角色" size="large" style="width: 100%">
            <el-option label="管理员" :value="1" />
            <el-option label="医生" :value="2" />
            <el-option label="护士" :value="3" />
            <el-option label="药剂师" :value="4" />
            <el-option label="患者" :value="5" />
          </el-select>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" :loading="loading" size="large" style="width: 100%" native-type="submit">
            登 录
          </el-button>
        </el-form-item>
        <div class="register-link">
          <span>还没有账号？</span>
          <router-link to="/register">立即注册</router-link>
        </div>
      </el-form>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { login } from '../api/auth'
import { useUserStore } from '../store/user'

const router = useRouter()
const store = useUserStore()
const formRef = ref(null)
const loading = ref(false)

const form = reactive({ userID: '', password: '', role: null })
const rules = {
  userID: [{ required: true, message: '请输入用户ID', trigger: 'blur' }],
  password: [{ required: true, message: '请输入密码', trigger: 'blur' }],
  role: [{ required: true, message: '请选择角色', trigger: 'change' }]
}

async function handleLogin() {
  await formRef.value.validate()
  loading.value = true
  try {
    const res = await login(form)
    if (res.code === 200) {
      store.setLogin(res.data)
      ElMessage.success('登录成功')
      // 按角色重定向到不同页面
      const roleRedirects = {
        1: '/dashboard',
        2: '/doctor/registrations',
        3: '/nurse/hospitalizations',
        4: '/pharmacist/medication-records',
        5: '/patient/registrations'
      }
      router.push(roleRedirects[res.data.role] || '/dashboard')
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) {
    ElMessage.error(e.message || '登录失败')
  } finally {
    loading.value = false
  }
}
</script>
```

**登录流程梳理（前后端打通的完整链路）：**

```text

1. 用户输入 userID(6位) + 密码 + 选择角色(1-5)
2. 点击登录 → formRef.validate() 校验表单
3. 调用 login(form) 函数
4. login() → axios 发送 POST /api/auth/login { userID, password, role }
5. Vite proxy 转发到 http://localhost:8080/api/auth/login
6. C++ 后端验证密码(SHA-256 + 盐值) → 生成 JWT token → 返回:
   {
     code: 200,
     data: {
       token: "eyJ...",
       userID: "000001",
       username: "张三",
       role: 1,          // 前端 1-based
       roleStr: "管理员"  // 角色名称
     }
   }
7. axios 响应拦截器自动解包 response.data
8. store.setLogin(res.data) → 保存全部 5 个字段到 Pinia + localStorage
9. ElMessage.success('登录成功')
10. router.push(roleRedirects[res.data.role]) → 按角色跳转不同页面
```

---

## 十五、前后端数据流总结

```text
用户操作（点击按钮/提交表单）
        ↓
Vue 组件调用 API 函数（如 login(form)）
        ↓
axios 实例（自动附加 Bearer JWT token 到请求头）
        ↓
Vite proxy 代理（/api/* → http://localhost:8080/api/*）
        ↓
C++ 后端（JWT 验证 → 角色权限检查 → 业务逻辑 → 链表操作）
        ↓
后端返回 JSON { code: 200, message: "", data: {...} }
        ↓
axios 响应拦截器：
  - 业务错误（code !== 200）→ ElMessage.error + 拒绝 Promise
  - 401 → 清除登录态 + 跳转 /login
  - 403 → "没有权限"
  - 404 → "资源不存在"
  - 超时 → "请求超时"
  - 成功 → 自动解包返回 data
        ↓
Vue 组件拿到数据 → 更新响应式状态 → 视图自动更新
```

---

## 十六、运行与调试

```bash
# 1. 先启动后端（端口 8080）
cd build && ./Debug/his_server.exe

# 2. 再启动前端（端口 3000）
cd frontend && npm run dev

# 3. 浏览器访问 http://localhost:3000
```

**调试技巧：**

- **Network 面板**：F12 → Network，查看每个请求的 URL、请求头（Bearer token）、响应体（JSON）
- **Console 面板**：查看 axios 拦截器的错误消息输出
- **Vue DevTools**：浏览器插件，查看 Pinia store 状态（token/role/userName）、组件树、路由信息
- **VS Code 断点**：在 `.vue` 文件的 `<script setup>` 中设置断点，通过 VS Code Debug 面板调试
- **验证 proxy 是否生效**：Network 面板中请求的 Remote Address 应该是 `127.0.0.1:3000`（Vite 代理），而不是 `127.0.0.1:8080`（直连后端）

---

## 十七、生产构建

```bash
cd frontend && npm run build
# 输出到 frontend/dist/

# 预览生产构建
cd frontend && npm run preview
```

生产环境不再需要 Vite proxy，需要通过 Nginx 反向代理将 `/api` 请求转发到后端 8080 端口：

```nginx
server {
    listen 80;
    server_name localhost;
    root /path/to/frontend/dist;
    index index.html;

    # API 请求转发到 C++ 后端
    location /api {
        proxy_pass http://localhost:8080;
        proxy_set_header Host $host;
    }

    # SPA 路由支持：找不到文件时返回 index.html
    location / {
        try_files $uri $uri/ /index.html;
    }
}
```
