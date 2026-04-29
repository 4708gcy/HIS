<template>
  <div class="fade-in">
    <el-row :gutter="20">
      <el-col :span="14">
        <el-card v-loading="loading">
          <template #header>
            <div style="display: flex; justify-content: space-between; align-items: center">
              <span>个人信息</span>
              <el-button type="primary" size="small" @click="toggleEdit">{{ editing ? '取消编辑' : '编辑' }}</el-button>
            </div>
          </template>
          <el-descriptions v-if="!editing && profile" :column="2" border>
            <el-descriptions-item label="患者ID">{{ profile.userID }}</el-descriptions-item>
            <el-descriptions-item label="姓名">{{ profile.username }}</el-descriptions-item>
            <el-descriptions-item label="性别">{{ profile.gender }}</el-descriptions-item>
            <el-descriptions-item label="年龄">{{ profile.age }}</el-descriptions-item>
            <el-descriptions-item label="电话">{{ profile.telephone }}</el-descriptions-item>
            <el-descriptions-item label="邮箱">{{ profile.email }}</el-descriptions-item>
            <el-descriptions-item label="地址" :span="2">{{ profile.address }}</el-descriptions-item>
            <el-descriptions-item label="紧急联系人">{{ profile.emergencyContactName }}</el-descriptions-item>
            <el-descriptions-item label="紧急电话">{{ profile.emergencyContactPhone }}</el-descriptions-item>
            <el-descriptions-item label="过敏史" :span="2">{{ profile.allergyHistory }}</el-descriptions-item>
            <el-descriptions-item label="既往病史" :span="2">{{ profile.pastMedicalHistory }}</el-descriptions-item>
          </el-descriptions>
          <el-form v-if="editing" :model="editForm" label-width="100px">
            <el-form-item label="姓名"><el-input v-model="editForm.username" /></el-form-item>
            <el-form-item label="性别">
              <el-radio-group v-model="editForm.gender"><el-radio value="男">男</el-radio><el-radio value="女">女</el-radio></el-radio-group>
            </el-form-item>
            <el-form-item label="年龄"><el-input-number v-model="editForm.age" :min="0" :max="150" /></el-form-item>
            <el-form-item label="电话"><el-input v-model="editForm.telephone" /></el-form-item>
            <el-form-item label="邮箱"><el-input v-model="editForm.email" /></el-form-item>
            <el-form-item label="地址"><el-input v-model="editForm.address" /></el-form-item>
            <el-form-item label="紧急联系人"><el-input v-model="editForm.emergencyContactName" /></el-form-item>
            <el-form-item label="紧急电话"><el-input v-model="editForm.emergencyContactPhone" /></el-form-item>
            <el-form-item>
              <el-button type="primary" :loading="saving" @click="handleSave">保存修改</el-button>
            </el-form-item>
          </el-form>
        </el-card>
      </el-col>
      <el-col :span="10">
        <el-card>
          <template #header><span>账户余额</span></template>
          <div style="text-align: center; padding: 20px 0">
            <el-statistic title="当前余额" :value="profile?.balance || 0" :precision="2" suffix="元" />
          </div>
          <el-divider />
          <el-form :inline="true">
            <el-form-item>
              <el-input-number v-model="rechargeAmount" :min="1" :max="100000" :step="100" />
            </el-form-item>
            <el-form-item>
              <el-button type="success" @click="handleRecharge">充值</el-button>
            </el-form-item>
          </el-form>
        </el-card>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getProfile, updateProfile, recharge } from '../../api/patient'

const loading = ref(false)
const saving = ref(false)
const profile = ref(null)
const editing = ref(false)
const editForm = reactive({})
const rechargeAmount = ref(100)

async function loadProfile() {
  loading.value = true
  try { const res = await getProfile(); if (res.code === 200) profile.value = res.data }
  catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
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
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { saving.value = false }
}

async function handleRecharge() {
  try {
    await ElMessageBox.confirm(`确认充值 ${rechargeAmount.value} 元？`, '充值确认')
    const res = await recharge(rechargeAmount.value)
    if (res.code === 200) { ElMessage.success(`充值成功，余额 ${res.data.balance?.toFixed(2)} 元`); loadProfile() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadProfile)
</script>
