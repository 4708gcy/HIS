<template>
  <div class="fade-in">
    <el-row :gutter="20">
      <el-col :span="14">
        <el-card v-loading="loading">
          <template #header>
            <div style="display: flex; justify-content: space-between; align-items: center">
              <span>管理员信息</span>
              <el-button type="primary" size="small" @click="toggleEdit">{{ editing ? '取消编辑' : '编辑' }}</el-button>
            </div>
          </template>
          <el-descriptions v-if="!editing && profile" :column="2" border>
            <el-descriptions-item label="管理员ID">{{ profile.userID }}</el-descriptions-item>
            <el-descriptions-item label="用户名">{{ profile.username }}</el-descriptions-item>
            <el-descriptions-item label="性别">{{ profile.gender || '未设置' }}</el-descriptions-item>
            <el-descriptions-item label="年龄">{{ profile.age || '未设置' }}</el-descriptions-item>
            <el-descriptions-item label="电话">{{ profile.telephone || '未设置' }}</el-descriptions-item>
            <el-descriptions-item label="邮箱">{{ profile.email || '未设置' }}</el-descriptions-item>
            <el-descriptions-item label="创建时间">{{ profile.createTime }}</el-descriptions-item>
            <el-descriptions-item label="账号状态">
              <el-tag :type="profile.isAccountActive ? 'success' : 'danger'" size="small">{{ profile.isAccountActive ? '正常' : '已锁定' }}</el-tag>
            </el-descriptions-item>
          </el-descriptions>
          <el-form v-if="editing" :model="editForm" label-width="100px">
            <el-form-item label="姓名"><el-input v-model="editForm.username" /></el-form-item>
            <el-form-item label="性别">
              <el-radio-group v-model="editForm.gender"><el-radio value="男">男</el-radio><el-radio value="女">女</el-radio></el-radio-group>
            </el-form-item>
            <el-form-item label="年龄"><el-input-number v-model="editForm.age" :min="0" :max="150" /></el-form-item>
            <el-form-item label="电话"><el-input v-model="editForm.telephone" /></el-form-item>
            <el-form-item label="邮箱"><el-input v-model="editForm.email" /></el-form-item>
            <el-form-item>
              <el-button type="primary" :loading="saving" @click="handleSave">保存修改</el-button>
            </el-form-item>
          </el-form>
        </el-card>
      </el-col>
      <el-col :span="10">
        <el-card>
          <template #header><span>修改密码</span></template>
          <el-form ref="pwdFormRef" :model="pwdForm" :rules="pwdRules" label-width="100px">
            <el-form-item label="当前密码" prop="oldPassword">
              <el-input v-model="pwdForm.oldPassword" type="password" show-password />
            </el-form-item>
            <el-form-item label="新密码" prop="newPassword">
              <el-input v-model="pwdForm.newPassword" type="password" show-password />
            </el-form-item>
            <el-form-item label="确认新密码" prop="confirmPassword">
              <el-input v-model="pwdForm.confirmPassword" type="password" show-password />
            </el-form-item>
            <el-form-item>
              <el-button type="primary" :loading="changingPwd" @click="handleChangePassword">修改密码</el-button>
            </el-form-item>
          </el-form>
        </el-card>
        <el-card style="margin-top: 16px">
          <template #header><span>账号管理</span></template>
          <el-descriptions :column="1" border v-if="profile">
            <el-descriptions-item label="用户ID">{{ profile.userID }}</el-descriptions-item>
            <el-descriptions-item label="角色">管理员</el-descriptions-item>
            <el-descriptions-item label="注册时间">{{ profile.createTime }}</el-descriptions-item>
          </el-descriptions>
          <div style="margin-top: 16px">
            <el-button type="danger" @click="handleLogout">退出登录</el-button>
          </div>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { useRouter } from 'vue-router'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getAdmins, updateAdminProfile } from '../../api/admin'
import { useUserStore } from '../../store/user'

const router = useRouter()
const store = useUserStore()
const loading = ref(false)
const saving = ref(false)
const changingPwd = ref(false)
const profile = ref(null)
const editing = ref(false)
const editForm = reactive({})
const pwdFormRef = ref(null)

const pwdForm = reactive({ oldPassword: '', newPassword: '', confirmPassword: '' })

const validateConfirm = (rule, value, callback) => {
  if (value !== pwdForm.newPassword) callback(new Error('两次输入密码不一致'))
  else callback()
}

const pwdRules = {
  oldPassword: [{ required: true, message: '请输入当前密码', trigger: 'blur' }],
  newPassword: [{ required: true, message: '请输入新密码', trigger: 'blur' }, { min: 6, message: '密码至少6位', trigger: 'blur' }],
  confirmPassword: [{ required: true, message: '请确认新密码', trigger: 'blur' }, { validator: validateConfirm, trigger: 'blur' }]
}

async function loadProfile() {
  loading.value = true
  try {
    const res = await getAdmins()
    if (res.code === 200) {
      profile.value = res.data.list.find(a => a.userID === store.userID)
    }
  } catch (e) {
    ElMessage.error('加载信息失败')
  } finally {
    loading.value = false
  }
}

function toggleEdit() {
  if (editing.value) { editing.value = false; return }
  Object.assign(editForm, { ...profile.value })
  editing.value = true
}

async function handleSave() {
  saving.value = true
  try {
    const res = await updateAdminProfile(editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editing.value = false; loadProfile() }
    else ElMessage.error(res.message)
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  } finally { saving.value = false }
}

async function handleChangePassword() {
  try {
    await pwdFormRef.value.validate()
  } catch { return }
  await ElMessageBox.confirm('确定修改密码？修改后需要重新登录', '确认')
  changingPwd.value = true
  try {
    const { changePassword } = await import('../../api/auth')
    const res = await changePassword(pwdForm.oldPassword, pwdForm.newPassword)
    if (res.code === 200) {
      ElMessage.success('密码修改成功，请重新登录')
      store.logout()
      router.push('/login')
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  } finally {
    changingPwd.value = false
  }
}

function handleLogout() {
  store.logout()
  router.push('/login')
}

onMounted(loadProfile)
</script>
