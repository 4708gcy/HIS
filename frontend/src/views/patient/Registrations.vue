<template>
  <div class="fade-in">
    <el-card style="margin-bottom: 16px">
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>预约挂号</span>
          <el-button type="primary" @click="regDialogVisible = true">新建挂号</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="registrationID" label="挂号ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="fee" label="挂号费" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag :type="['info','success','warning','danger'][row.status]" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="registerTime" label="挂号时间" min-width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button v-if="row.status === 0" type="success" size="small" @click="handlePay(row)">支付</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 新建挂号弹窗 -->
    <el-dialog v-model="regDialogVisible" title="预约挂号" width="450px">
      <el-form :model="regForm" label-width="80px">
        <el-form-item label="科室">
          <el-select v-model="regForm.department" placeholder="选择科室" style="width: 100%" @change="loadDoctors">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="医生">
          <el-select v-model="regForm.doctorID" placeholder="选择医生" style="width: 100%">
            <el-option v-for="doc in doctorList" :key="doc.userID" :label="`${doc.username} (${doc.titleStr})`" :value="doc.userID" />
          </el-select>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="regDialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="submitting" @click="handleRegister">确认挂号</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyRegistrations, createRegistration, payRegistration } from '../../api/patient'
import { getDepartments, getDoctors } from '../../api/common'
import { useUserStore } from '../../store/user'

const store = useUserStore()
const loading = ref(false)
const submitting = ref(false)
const tableData = ref([])
const regDialogVisible = ref(false)
const departments = ref([])
const doctorList = ref([])
const regForm = reactive({ department: '', doctorID: '' })

getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') })

async function loadData() {
  loading.value = true
  try {
    const res = await getMyRegistrations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function loadDoctors(department) {
  try {
    regForm.doctorID = ''
    const res = await getDoctors({ department })
    if (res.code === 200) doctorList.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleRegister() {
  if (!regForm.doctorID) { ElMessage.warning('请选择医生'); return }
  submitting.value = true
  try {
    const res = await createRegistration(regForm)
    if (res.code === 200) { ElMessage.success(`挂号成功，费用 ${res.data.fee?.toFixed(2)} 元`); regDialogVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { submitting.value = false }
}

async function handlePay(row) {
  try {
    await ElMessageBox.confirm(`确认支付挂号费 ${row.fee?.toFixed(2)} 元？`, '支付确认')
    const res = await payRegistration(row.registrationID)
    if (res.code === 200) { ElMessage.success('支付成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
