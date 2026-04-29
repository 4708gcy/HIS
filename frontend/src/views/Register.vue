<template>
  <div class="login-container">
    <!-- Decorative background (same as Login) -->
    <svg class="bg-pattern" viewBox="0 0 1440 900" fill="none" xmlns="http://www.w3.org/2000/svg" aria-hidden="true">
      <defs>
        <linearGradient id="bg" x1="0" y1="0" x2="1" y2="1">
          <stop offset="0%" stop-color="#e3f2fd"/>
          <stop offset="50%" stop-color="#f6f8fc"/>
          <stop offset="100%" stop-color="#e8f5e9" stop-opacity="0.3"/>
        </linearGradient>
      </defs>
      <rect width="1440" height="900" fill="url(#bg)"/>
      <circle cx="300" cy="200" r="280" fill="#1e88e5" opacity="0.04"/>
      <circle cx="1200" cy="700" r="350" fill="#1e88e5" opacity="0.03"/>
      <circle cx="700" cy="100" r="180" fill="#1e88e5" opacity="0.025"/>
      <g opacity="0.06" fill="#1e88e5">
        <rect x="1300" y="150" width="12" height="36" rx="3"/>
        <rect x="1294" y="162" width="24" height="12" rx="3"/>
        <rect x="150" y="500" width="16" height="48" rx="3"/>
        <rect x="142" y="516" width="32" height="16" rx="3"/>
      </g>
      <path d="M0 500 Q 500 350, 900 480 T 1440 300" stroke="#1e88e5" stroke-width="1.5" fill="none" opacity="0.08"/>
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
        <p>注册新账号</p>
      </div>

      <el-form ref="formRef" :model="form" :rules="rules" label-width="90px" class="login-form">
        <el-form-item label="角色" prop="role">
          <el-select v-model="form.role" placeholder="请选择角色" style="width: 100%">
            <el-option label="医生" :value="2" />
            <el-option label="护士" :value="3" />
            <el-option label="药剂师" :value="4" />
            <el-option label="患者" :value="5" />
          </el-select>
        </el-form-item>
        <el-form-item label="用户名" prop="username">
          <el-input v-model="form.username" placeholder="请输入用户名" />
        </el-form-item>
        <el-form-item label="密码" prop="password">
          <el-input v-model="form.password" type="password" show-password placeholder="请输入密码" />
        </el-form-item>
        <el-form-item label="确认密码" prop="confirmPassword">
          <el-input v-model="form.confirmPassword" type="password" show-password placeholder="请再次输入密码" />
        </el-form-item>
        <el-form-item label="性别">
          <el-radio-group v-model="form.gender">
            <el-radio value="男">男</el-radio>
            <el-radio value="女">女</el-radio>
          </el-radio-group>
        </el-form-item>
        <el-form-item label="年龄">
          <el-input-number v-model="form.age" :min="0" :max="150" />
        </el-form-item>
        <el-form-item label="联系电话">
          <el-input v-model="form.telephone" placeholder="请输入手机号" />
        </el-form-item>
        <el-form-item label="科室" v-if="form.role >= 2 && form.role <= 4">
          <el-select v-model="form.department" placeholder="请选择科室" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item v-if="form.role === 2" label="职称">
          <el-select v-model="form.title" style="width: 100%">
            <el-option label="实习医生" :value="1" />
            <el-option label="住院医师" :value="2" />
            <el-option label="主治医师" :value="3" />
            <el-option label="副主任医师" :value="4" />
            <el-option label="主任医师" :value="5" />
          </el-select>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" :loading="loading" style="width: 100%" @click="handleRegister">注 册</el-button>
        </el-form-item>
        <div class="login-link">
          <span>已有账号？</span>
          <router-link to="/login">返回登录</router-link>
        </div>
      </el-form>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage } from 'element-plus'
import { register } from '../api/auth'
import { getDepartments } from '../api/common'
import { useUserStore } from '../store/user'

const router = useRouter()
const store = useUserStore()
const formRef = ref(null)
const loading = ref(false)
const departments = ref([])

getDepartments().then(res => { departments.value = res.data.list })

const form = reactive({
  role: null, username: '', password: '', confirmPassword: '',
  gender: '#', age: 0, telephone: '#', department: '急诊科', title: 1
})

const validateConfirm = (rule, value, callback) => {
  if (value !== form.password) callback(new Error('两次输入密码不一致'))
  else callback()
}

const rules = {
  role: [{ required: true, message: '请选择角色', trigger: 'change' }],
  username: [{ required: true, message: '请输入用户名', trigger: 'blur' }],
  password: [{ required: true, message: '请输入密码', trigger: 'blur' }, { min: 6, message: '密码至少6位', trigger: 'blur' }],
  confirmPassword: [{ required: true, message: '请确认密码', trigger: 'blur' }, { validator: validateConfirm, trigger: 'blur' }]
}

async function handleRegister() {
  await formRef.value.validate()
  loading.value = true
  try {
    const { confirmPassword, ...data } = form
    const res = await register(data)
    if (res.code === 200) {
      store.setLogin(res.data)
      ElMessage.success(`注册成功，您的ID为 ${res.data.userID}`)
      const roleRedirects = { 1: '/dashboard', 2: '/doctor/registrations', 3: '/nurse/hospitalizations', 4: '/pharmacist/medication-records', 5: '/patient/registrations' }
      router.push(roleRedirects[res.data.role] || '/dashboard')
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) {
    ElMessage.error(e.message || '注册失败')
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
  width: 460px;
  max-height: 90vh;
  overflow-y: auto;
  padding: 36px 36px 28px;
  background: rgba(255, 255, 255, 0.85);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.6);
  box-shadow: 0 8px 32px rgba(30, 136, 229, 0.1), 0 1px 2px rgba(30, 136, 229, 0.05);
}

.login-card::-webkit-scrollbar {
  width: 4px;
}
.login-card::-webkit-scrollbar-thumb {
  background: var(--his-border);
  border-radius: 4px;
}

.login-header {
  text-align: center;
  margin-bottom: 28px;
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
  margin-bottom: 18px;
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

.login-form :deep(.el-button--primary) {
  border-radius: 8px;
  height: 42px;
  font-size: 15px;
  font-weight: 500;
  letter-spacing: 1px;
}

.login-link {
  text-align: center;
  margin-top: 8px;
  font-size: 14px;
  color: var(--his-text-muted);
}

.login-link a {
  color: var(--his-primary);
  text-decoration: none;
  font-weight: 500;
  margin-left: 4px;
  transition: color 0.2s;
}

.login-link a:hover {
  color: var(--his-primary-dark);
  text-decoration: underline;
}
</style>
