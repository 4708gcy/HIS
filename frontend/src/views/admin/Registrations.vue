<template>
  <div>
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 120px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="患者ID"><el-input v-model="query.patientID" clearable style="width: 120px" /></el-form-item>
        <el-form-item label="医生ID"><el-input v-model="query.doctorID" clearable style="width: 120px" /></el-form-item>
        <el-form-item label="状态">
          <el-select v-model="query.status" clearable placeholder="全部" style="width: 120px">
            <el-option label="已预约" value="0" /><el-option label="已缴费" value="1" />
            <el-option label="已看诊" value="2" /><el-option label="已取消" value="3" />
          </el-select>
        </el-form-item>
        <el-form-item><el-button type="primary" @click="loadData">搜索</el-button><el-button @click="resetQuery">重置</el-button></el-form-item>
      </el-form>
    </el-card>

    <el-card>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="registrationID" label="挂号ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="fee" label="挂号费" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="80">
          <template #default="{ row }">
            <el-tag :type="statusType(row.status)" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="registerTime" label="挂号时间" width="180" />
        <el-table-column label="操作" width="180" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openStatusDialog(row)">改状态</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <el-dialog v-model="statusVisible" title="修改挂号状态" width="400px">
      <el-form label-width="80px">
        <el-form-item label="挂号ID"><span>{{ currentReg.registrationID }}</span></el-form-item>
        <el-form-item label="新状态">
          <el-select v-model="newStatus" style="width: 100%">
            <el-option label="已预约" :value="0" /><el-option label="已缴费" :value="1" />
            <el-option label="已看诊" :value="2" /><el-option label="已取消" :value="3" />
          </el-select>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="statusVisible = false">取消</el-button>
        <el-button type="primary" @click="updateStatus">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getRegistrations, updateRegistrationStatus, deleteRegistration } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '', patientID: '', doctorID: '', status: '' })
const statusVisible = ref(false)
const currentReg = reactive({})
const newStatus = ref(0)

const statusType = (s) => ['', 'success', 'warning', 'info'][s] || ''

function resetQuery() { Object.assign(query, { department: '', patientID: '', doctorID: '', status: '' }); page.value = 1; loadData() }

async function loadData() {
  loading.value = true
  try {
    const res = await getRegistrations(query)
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function openStatusDialog(row) { Object.assign(currentReg, row); newStatus.value = row.status; statusVisible.value = true }

async function updateStatus() {
  try {
    const res = await updateRegistrationStatus(currentReg.registrationID, newStatus.value)
    if (res.code === 200) { ElMessage.success('修改成功'); statusVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此挂号记录？', '确认', { type: 'warning' })
    const res = await deleteRegistration(row.registrationID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
