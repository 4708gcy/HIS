<template>
  <div>
    <el-card v-loading="loading">
      <template #header><span>个人信息</span></template>
      <el-descriptions :column="2" border v-if="profile">
        <el-descriptions-item label="医生ID">{{ profile.userID }}</el-descriptions-item>
        <el-descriptions-item label="姓名">{{ profile.username }}</el-descriptions-item>
        <el-descriptions-item label="性别">{{ profile.gender }}</el-descriptions-item>
        <el-descriptions-item label="年龄">{{ profile.age }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ profile.department }}</el-descriptions-item>
        <el-descriptions-item label="职称">{{ profile.titleStr }}</el-descriptions-item>
        <el-descriptions-item label="电话">{{ profile.telephone }}</el-descriptions-item>
        <el-descriptions-item label="邮箱">{{ profile.email }}</el-descriptions-item>
        <el-descriptions-item label="专长">{{ profile.specialty }}</el-descriptions-item>
        <el-descriptions-item label="排班">{{ profile.scheduleInfo }}</el-descriptions-item>
      </el-descriptions>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { getProfile } from '../../api/doctor'

const loading = ref(false)
const profile = ref(null)

async function loadProfile() {
  loading.value = true
  try {
    const res = await getProfile()
    if (res.code === 200) profile.value = res.data
  } finally { loading.value = false }
}

onMounted(loadProfile)
</script>
