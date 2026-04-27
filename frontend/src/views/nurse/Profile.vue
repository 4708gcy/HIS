<template>
  <div>
    <el-card v-loading="loading">
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>个人信息</span>
          <el-button type="primary" size="small" @click="toggleEdit">{{ editing ? '取消编辑' : '编辑' }}</el-button>
        </div>
      </template>
      <el-descriptions v-if="!editing && profile" :column="2" border>
        <el-descriptions-item label="护士ID">{{ profile.userID }}</el-descriptions-item>
        <el-descriptions-item label="姓名">{{ profile.username }}</el-descriptions-item>
        <el-descriptions-item label="性别">{{ profile.gender }}</el-descriptions-item>
        <el-descriptions-item label="年龄">{{ profile.age }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ profile.department }}</el-descriptions-item>
        <el-descriptions-item label="级别">{{ profile.levelStr }}</el-descriptions-item>
        <el-descriptions-item label="电话">{{ profile.telephone }}</el-descriptions-item>
        <el-descriptions-item label="排班">{{ profile.scheduleInfo }}</el-descriptions-item>
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
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getProfile, updateProfile } from '../../api/nurse'

const loading = ref(false)
const saving = ref(false)
const profile = ref(null)
const editing = ref(false)
const editForm = reactive({})

async function loadProfile() {
  loading.value = true
  try {
    const res = await getProfile()
    if (res.code === 200) profile.value = res.data
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  } finally { loading.value = false }
}

function toggleEdit() {
  if (editing.value) { editing.value = false; return }
  Object.assign(editForm, { ...profile.value })
  editing.value = true
}

async function handleSave() {
  saving.value = true
  try {
    const res = await updateProfile(editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editing.value = false; loadProfile() }
    else ElMessage.error(res.message)
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  } finally { saving.value = false }
}

onMounted(loadProfile)
</script>
