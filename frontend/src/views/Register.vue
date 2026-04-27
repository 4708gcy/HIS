<template>
  <div class="login-container">
    <el-card class="login-card" shadow="always">
      <template #header>
        <div class="card-header">
          <h2>HIS 医院信息系统</h2>
          <p>注册新账号</p>
        </div>
      </template>
      <el-form ref="formRef" :model="form" :rules="rules" label-width="90px">
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
        <el-form-item>
          <router-link to="/login">
            <el-link type="primary">已有账号？返回登录</el-link>
          </router-link>
        </el-form-item>
      </el-form>
    </el-card>
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
      router.push('/dashboard')
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
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
}
.login-card {
  width: 480px;
  border-radius: 12px;
  max-height: 90vh;
  overflow-y: auto;
}
.card-header {
  text-align: center;
}
.card-header h2 { margin: 0 0 8px; color: #303133; }
.card-header p { color: #909399; margin: 0; }
</style>
