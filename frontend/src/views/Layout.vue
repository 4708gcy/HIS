<template>
  <el-container style="height: 100vh">
    <!-- 侧边栏 -->
    <el-aside :width="isCollapse ? '64px' : '220px'" style="transition: width 0.3s; background: #304156">
      <div class="logo">
        <span v-if="!isCollapse">HIS 系统</span>
        <span v-else>H</span>
      </div>
      <el-menu
        :default-active="$route.path"
        :collapse="isCollapse"
        background-color="#304156"
        text-color="#bfcbd9"
        active-text-color="#409EFF"
        router
        unique-opened
      >
        <el-menu-item index="/dashboard">
          <el-icon><DataAnalysis /></el-icon>
          <template #title>首页概览</template>
        </el-menu-item>

        <!-- 管理员菜单 -->
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
          <el-menu-item index="/admin/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- 医生菜单 -->
        <template v-if="store.role === 2">
          <el-menu-item index="/doctor/registrations"><el-icon><List /></el-icon><template #title>挂号列表</template></el-menu-item>
          <el-menu-item index="/doctor/consultations"><el-icon><ChatDotRound /></el-icon><template #title>看诊管理</template></el-menu-item>
          <el-menu-item index="/doctor/examinations"><el-icon><Monitor /></el-icon><template #title>检查记录</template></el-menu-item>
          <el-menu-item index="/doctor/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- 护士菜单 -->
        <template v-if="store.role === 3">
          <el-menu-item index="/nurse/hospitalizations"><el-icon><House /></el-icon><template #title>住院管理</template></el-menu-item>
          <el-menu-item index="/nurse/examinations"><el-icon><Monitor /></el-icon><template #title>体征录入</template></el-menu-item>
          <el-menu-item index="/nurse/beds"><el-icon><Key /></el-icon><template #title>床位管理</template></el-menu-item>
          <el-menu-item index="/nurse/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- 药剂师菜单 -->
        <template v-if="store.role === 4">
          <el-menu-item index="/pharmacist/medication-records"><el-icon><Document /></el-icon><template #title>用药审核</template></el-menu-item>
          <el-menu-item index="/pharmacist/medicines"><el-icon><Box /></el-icon><template #title>药品库存</template></el-menu-item>
          <el-menu-item index="/pharmacist/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>

        <!-- 患者菜单 -->
        <template v-if="store.role === 5">
          <el-menu-item index="/patient/registrations"><el-icon><Calendar /></el-icon><template #title>预约挂号</template></el-menu-item>
          <el-menu-item index="/patient/consultations"><el-icon><ChatDotRound /></el-icon><template #title>看诊记录</template></el-menu-item>
          <el-menu-item index="/patient/examinations"><el-icon><Monitor /></el-icon><template #title>检查记录</template></el-menu-item>
          <el-menu-item index="/patient/medication-records"><el-icon><FirstAidKit /></el-icon><template #title>用药记录</template></el-menu-item>
          <el-menu-item index="/patient/hospitalizations"><el-icon><House /></el-icon><template #title>住院记录</template></el-menu-item>
          <el-menu-item index="/patient/profile"><el-icon><User /></el-icon><template #title>个人信息</template></el-menu-item>
        </template>
      </el-menu>
    </el-aside>

    <!-- 右侧主区域 -->
    <el-container>
      <el-header style="display: flex; align-items: center; justify-content: space-between; border-bottom: 1px solid #e6e6e6; background: #fff">
        <div style="display: flex; align-items: center; gap: 12px">
          <el-icon :size="20" style="cursor: pointer" @click="isCollapse = !isCollapse">
            <Fold v-if="!isCollapse" /><Expand v-else />
          </el-icon>
          <el-breadcrumb separator="/">
            <el-breadcrumb-item>{{ store.roleName }}</el-breadcrumb-item>
            <el-breadcrumb-item>{{ $route.meta.title || currentPageTitle }}</el-breadcrumb-item>
          </el-breadcrumb>
        </div>
        <div style="display: flex; align-items: center; gap: 16px">
          <el-tag type="info">{{ store.roleName }}</el-tag>
          <span>{{ store.username }}（{{ store.userID }}）</span>
          <el-button type="danger" text @click="handleLogout">退出登录</el-button>
        </div>
      </el-header>

      <el-main style="background: #f0f2f5; padding: 20px; overflow-y: auto">
        <router-view />
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
    '/admin/profile': '个人信息',
    '/doctor/registrations': '挂号列表', '/doctor/consultations': '看诊管理',
    '/doctor/examinations': '检查记录', '/doctor/profile': '个人信息',
    '/nurse/hospitalizations': '住院管理', '/nurse/examinations': '体征录入',
    '/nurse/beds': '床位管理', '/nurse/profile': '个人信息',
    '/pharmacist/medication-records': '用药审核', '/pharmacist/medicines': '药品库存',
    '/pharmacist/profile': '个人信息',
    '/patient/registrations': '预约挂号', '/patient/consultations': '看诊记录',
    '/patient/examinations': '检查记录', '/patient/medication-records': '用药记录',
    '/patient/hospitalizations': '住院记录', '/patient/profile': '个人信息'
  }
  return map[route.path] || ''
})

function handleLogout() {
  store.logout()
  router.push('/login')
}
</script>

<style scoped>
.logo {
  height: 60px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  font-size: 20px;
  font-weight: bold;
  background: #263445;
}
.el-menu {
  border-right: none;
}
</style>
