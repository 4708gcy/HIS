<template>
  <div class="login-container">
    <!-- Decorative background -->
    <svg class="bg-pattern" viewBox="0 0 1440 900" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
      <defs>
        <linearGradient id="bg" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#e3f2fd"/>
          <stop offset="50%" stop-color="#f6f8fc"/>
          <stop offset="100%" stop-color="#e8f5e9" stop-opacity="0.3"/>
        </linearGradient>
      </defs>
      <rect width="1440" height="900" fill="url(#bg)"/>
      <!-- Soft circles -->
      <circle cx="1200" cy="150" r="280" fill="#1e88e5" opacity="0.04"/>
      <circle cx="200" cy="700" r="350" fill="#1e88e5" opacity="0.03"/>
      <circle cx="800" cy="600" r="180" fill="#1e88e5" opacity="0.025"/>
      <!-- Medical cross motifs -->
      <g opacity="0.06" fill="#1e88e5">
        <rect x="100" y="120" width="12" height="36" rx="3"/>
        <rect x="94" y="132" width="24" height="12" rx="3"/>
        <rect x="1300" y="400" width="16" height="48" rx="3"/>
        <rect x="1292" y="416" width="32" height="16" rx="3"/>
        <rect x="600" y="80" width="10" height="30" rx="2.5"/>
        <rect x="595" y="90" width="20" height="10" rx="2.5"/>
      </g>
      <!-- Flowing curve -->
      <path d="M0 600 Q 400 450, 720 550 T 1440 400" stroke="#1e88e5" stroke-width="1.5" fill="none" opacity="0.08"/>
      <path d="M0 650 Q 500 500, 900 620 T 1440 480" stroke="#1e88e5" stroke-width="1" fill="none" opacity="0.05"/>
    </svg>

    <div class="login-card fade-in">
      <div class="login-header">
        <div class="logo-icon">
          <svg viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="18" y="8" width="12" height="32" rx="3" fill="#1e88e5"/>
            <rect x="8" y="18" width="32" height="12" rx="3" fill="#1e88e5"/>
          </svg>
        </div>
        <h2>HIS 医院信息系统</h2>
        <p>请登录您的账号</p>
      </div>

      <el-form ref="formRef" :model="form" :rules="rules" @submit.prevent="handleLogin" class="login-form">
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
          <el-button type="primary" :loading="loading" size="large" style="width: 100%" native-type="submit">登 录</el-button>
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
      const roleRedirects = { 1: '/dashboard', 2: '/doctor/registrations', 3: '/nurse/hospitalizations', 4: '/pharmacist/medication-records', 5: '/patient/registrations' }
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

<style scoped>
.login-container {
  height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
  position: relative;
  overflow: hidden;
}

.bg-pattern {
  position: absolute;
  inset: 0;
  width: 100%;
  height: 100%;
  z-index: 0;
}

.login-card {
  position: relative;
  z-index: 1;
  width: 420px;
  padding: 40px 36px 32px;
  background: rgba(255, 255, 255, 0.85);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.6);
  box-shadow: 0 8px 32px rgba(30, 136, 229, 0.1), 0 1px 2px rgba(30, 136, 229, 0.05);
}

.login-header {
  text-align: center;
  margin-bottom: 32px;
}

.logo-icon {
  width: 48px;
  height: 48px;
  margin: 0 auto 16px;
}

.logo-icon svg {
  width: 100%;
  height: 100%;
}

.login-header h2 {
  font-size: 22px;
  font-weight: 600;
  color: var(--his-text);
  margin: 0 0 6px;
  letter-spacing: 0.5px;
}

.login-header p {
  color: var(--his-text-muted);
  margin: 0;
  font-size: 14px;
}

.login-form {
  margin-top: 8px;
}

.login-form :deep(.el-form-item) {
  margin-bottom: 20px;
}

.login-form :deep(.el-input__wrapper) {
  border-radius: 8px;
  padding: 4px 12px;
  box-shadow: 0 0 0 1px var(--his-border) inset !important;
  transition: box-shadow 0.2s;
}

.login-form :deep(.el-input__wrapper:hover) {
  box-shadow: 0 0 0 1px var(--his-primary-hover) inset !important;
}

.login-form :deep(.el-input__wrapper.is-focus) {
  box-shadow: 0 0 0 1px var(--his-primary) inset !important;
}

.login-form :deep(.el-select .el-input__wrapper) {
  padding: 4px 12px;
}

.login-form :deep(.el-button--primary) {
  border-radius: 8px;
  height: 42px;
  font-size: 15px;
  font-weight: 500;
  letter-spacing: 1px;
}

.register-link {
  text-align: center;
  margin-top: 8px;
  font-size: 14px;
  color: var(--his-text-muted);
}

.register-link a {
  color: var(--his-primary);
  text-decoration: none;
  font-weight: 500;
  margin-left: 4px;
  transition: color 0.2s;
}

.register-link a:hover {
  color: var(--his-primary-dark);
  text-decoration: underline;
}
</style>
