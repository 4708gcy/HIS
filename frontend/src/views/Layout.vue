<template>
  <el-container class="layout-container">
    <!-- Sidebar -->
    <el-aside :width="isCollapse ? '64px' : '240px'" class="sidebar">
      <div class="logo" :class="{ 'logo--collapsed': isCollapse }">
        <svg viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
          <rect x="15" y="6" width="10" height="28" rx="2.5" fill="#1e88e5"/>
          <rect x="6" y="15" width="28" height="10" rx="2.5" fill="#1e88e5"/>
        </svg>
        <span v-if="!isCollapse" class="logo-text">HIS</span>
      </div>

      <el-menu
        :default-active="$route.path"
        :collapse="isCollapse"
        class="sidebar-menu"
        router
        unique-opened
      >
        <el-menu-item index="/dashboard" class="menu-item">
          <el-icon><DataAnalysis /></el-icon>
          <template #title>首页概览</template>
        </el-menu-item>

        <!-- Admin menu -->
        <template v-if="store.role === 1">
          <el-sub-menu index="admin-users">
            <template #title><el-icon><User /></el-icon><span>人员管理</span></template>
            <el-menu-item index="/admin/doctors">医生管理</el-menu-item>
            <el-menu-item index="/admin/nurses">护士管理</el-menu-item>
            <el-menu-item index="/admin/pharmacists">药剂师管理</el-menu-item>
            <el-menu-item index="/admin/patients">患者管理</el-menu-item>
          </el-sub-menu>
          <el-sub-menu index="admin-records">
            <template #title><el-icon><Document /></el-icon><span>医疗记录</span></template>
            <el-menu-item index="/admin/registrations">挂号记录</el-menu-item>
            <el-menu-item index="/admin/consultations">看诊记录</el-menu-item>
            <el-menu-item index="/admin/examinations">检查记录</el-menu-item>
            <el-menu-item index="/admin/hospitalizations">住院记录</el-menu-item>
            <el-menu-item index="/admin/medication-records">用药记录</el-menu-item>
          </el-sub-menu>
          <el-sub-menu index="admin-resources">
            <template #title><el-icon><Box /></el-icon><span>资源管理</span></template>
            <el-menu-item index="/admin/medicines">药品管理</el-menu-item>
            <el-menu-item index="/admin/beds">床位管理</el-menu-item>
          </el-sub-menu>
          <el-sub-menu index="admin-system">
            <template #title><el-icon><Setting /></el-icon><span>系统管理</span></template>
            <el-menu-item index="/admin/schedules">排班管理</el-menu-item>
            <el-menu-item index="/admin/reports">统计报表</el-menu-item>
          </el-sub-menu>
          <el-menu-item index="/admin/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- Doctor menu -->
        <template v-if="store.role === 2">
          <el-menu-item index="/doctor/registrations"><el-icon><List /></el-icon><template #title>挂号列表</template></el-menu-item>
          <el-menu-item index="/doctor/consultations"><el-icon><ChatDotRound /></el-icon><template #title>看诊管理</template></el-menu-item>
          <el-menu-item index="/doctor/examinations"><el-icon><Monitor /></el-icon><template #title>检查记录</template></el-menu-item>
          <el-menu-item index="/doctor/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- Nurse menu -->
        <template v-if="store.role === 3">
          <el-menu-item index="/nurse/hospitalizations"><el-icon><House /></el-icon><template #title>住院管理</template></el-menu-item>
          <el-menu-item index="/nurse/examinations"><el-icon><Monitor /></el-icon><template #title>体征录入</template></el-menu-item>
          <el-menu-item index="/nurse/beds"><el-icon><Key /></el-icon><template #title>床位管理</template></el-menu-item>
          <el-menu-item index="/nurse/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- Pharmacist menu -->
        <template v-if="store.role === 4">
          <el-menu-item index="/pharmacist/medication-records"><el-icon><Document /></el-icon><template #title>用药审核</template></el-menu-item>
          <el-menu-item index="/pharmacist/medicines"><el-icon><Box /></el-icon><template #title>药品库存</template></el-menu-item>
          <el-menu-item index="/pharmacist/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- Patient menu -->
        <template v-if="store.role === 5">
          <el-menu-item index="/patient/registrations"><el-icon><Calendar /></el-icon><template #title>预约挂号</template></el-menu-item>
          <el-menu-item index="/patient/consultations"><el-icon><ChatDotRound /></el-icon><template #title>看诊记录</template></el-menu-item>
          <el-menu-item index="/patient/examinations"><el-icon><Monitor /></el-icon><template #title>检查记录</template></el-menu-item>
          <el-menu-item index="/patient/medication-records"><el-icon><FirstAidKit /></el-icon><template #title>用药记录</template></el-menu-item>
          <el-menu-item index="/patient/hospitalizations"><el-icon><House /></el-icon><template #title>住院记录</template></el-menu-item>
          <el-menu-item index="/patient/schedules"><el-icon><Calendar /></el-icon><template #title>排班查询</template></el-menu-item>
          <el-menu-item index="/patient/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>
      </el-menu>
    </el-aside>

    <!-- Main area -->
    <el-container class="main-area">
      <el-header class="layout-header">
        <div class="header-left">
          <el-icon :size="20" class="collapse-btn" @click="isCollapse = !isCollapse">
            <Fold v-if="!isCollapse" /><Expand v-else />
          </el-icon>
          <el-breadcrumb separator-class="el-icon-arrow-right">
            <el-breadcrumb-item>{{ store.roleName }}</el-breadcrumb-item>
            <el-breadcrumb-item>{{ $route.meta.title || currentPageTitle }}</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
        <div class="header-right">
          <el-tag type="info" size="small" effect="plain">{{ store.roleName }}</el-tag>
          <span class="user-info">{{ store.username }}<span class="user-id">{{ store.userID }}</span></span>
          <el-button type="danger" text @click="handleLogout">
            <el-icon><SwitchButton /></el-icon>
            退出
          </el-button>
        </div>
      </el-header>

      <el-main class="layout-main">
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

const currentPageTitle = computed(() => {
  const map = {
    '/dashboard': '首页概览',
    '/admin/doctors': '医生管理', '/admin/nurses': '护士管理',
    '/admin/pharmacists': '药剂师管理', '/admin/patients': '患者管理',
    '/admin/registrations': '挂号记录', '/admin/consultations': '看诊记录',
    '/admin/examinations': '检查记录', '/admin/hospitalizations': '住院记录',
    '/admin/medication-records': '用药记录', '/admin/medicines': '药品管理',
    '/admin/beds': '床位管理',
    '/admin/schedules': '排班管理',
    '/admin/reports': '统计报表',
    '/admin/profile': '个人信息',
    '/doctor/registrations': '挂号列表', '/doctor/consultations': '看诊管理',
    '/doctor/examinations': '检查记录', '/doctor/profile': '个人信息',
    '/nurse/hospitalizations': '住院管理', '/nurse/examinations': '体征录入',
    '/nurse/beds': '床位管理', '/nurse/profile': '个人信息',
    '/pharmacist/medication-records': '用药审核', '/pharmacist/medicines': '药品库存',
    '/pharmacist/profile': '个人信息',
    '/patient/registrations': '预约挂号', '/patient/consultations': '看诊记录',
    '/patient/examinations': '检查记录', '/patient/medication-records': '用药记录',
    '/patient/hospitalizations': '住院记录', '/patient/schedules': '排班查询', '/patient/profile': '个人信息'
  }
  return map[route.path] || ''
})

function handleLogout() {
  store.logout()
  router.push('/login')
}
</script>

<style scoped>
.layout-container {
  height: 100vh;
  background: var(--his-bg);
}

/* === Sidebar === */
.sidebar {
  background: var(--his-sidebar-bg);
  border-right: 1px solid var(--his-border);
  transition: width 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  box-shadow: 2px 0 8px rgba(30, 136, 229, 0.04);
  overflow: hidden;
}

.logo {
  height: 56px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 10px;
  background: var(--his-surface);
  border-bottom: 1px solid var(--his-border);
  transition: all 0.3s;
}

.logo svg {
  width: 28px;
  height: 28px;
  flex-shrink: 0;
}

.logo-text {
  font-size: 18px;
  font-weight: 700;
  color: var(--his-primary);
  letter-spacing: 1px;
  transition: opacity 0.3s;
}

/* === Menu === */
.sidebar-menu {
  border-right: none !important;
  background: transparent !important;
}

.sidebar-menu :deep(.el-menu-item) {
  color: var(--his-sidebar-text) !important;
  border-radius: 8px;
  margin: 2px 8px;
  transition: all 0.2s;
}

.sidebar-menu :deep(.el-menu-item:hover) {
  background: var(--his-surface-alt) !important;
  color: var(--his-primary) !important;
}

.sidebar-menu :deep(.el-menu-item.is-active) {
  background: var(--his-sidebar-active-bg) !important;
  color: var(--his-sidebar-active-text) !important;
  font-weight: 500;
}

.sidebar-menu :deep(.el-sub-menu__title) {
  color: var(--his-sidebar-text) !important;
  border-radius: 8px;
  margin: 2px 8px;
}

.sidebar-menu :deep(.el-sub-menu__title:hover) {
  background: var(--his-surface-alt) !important;
  color: var(--his-primary) !important;
}

.sidebar-menu :deep(.el-sub-menu .el-menu-item) {
  margin: 1px 4px;
  font-size: 13px;
}

/* === Header === */
.layout-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  background: var(--his-surface);
  border-bottom: 1px solid var(--his-border);
  padding: 0 24px;
  height: 56px;
  box-shadow: 0 1px 4px rgba(30, 136, 229, 0.04);
}

.header-left {
  display: flex;
  align-items: center;
  gap: 16px;
}

.collapse-btn {
  cursor: pointer;
  color: var(--his-text-secondary);
  transition: color 0.2s;
}

.collapse-btn:hover {
  color: var(--his-primary);
}

.header-left :deep(.el-breadcrumb__item) {
  color: var(--his-text-secondary);
}

.header-left :deep(.el-breadcrumb__item:last-child .el-breadcrumb__inner) {
  color: var(--his-text);
  font-weight: 500;
}

.header-right {
  display: flex;
  align-items: center;
  gap: 14px;
}

.user-info {
  font-size: 14px;
  color: var(--his-text);
  font-weight: 500;
}

.user-id {
  color: var(--his-text-muted);
  font-weight: 400;
  margin-left: 4px;
  font-size: 12px;
}

/* === Main === */
.layout-main {
  background: var(--his-bg);
  padding: 16px 20px;
  overflow-y: auto;
}

/* === Page transition === */
.fade-view-enter-active,
.fade-view-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.fade-view-enter-from {
  opacity: 0;
  transform: translateY(6px);
}

.fade-view-leave-to {
  opacity: 0;
  transform: translateY(-4px);
}
</style>
