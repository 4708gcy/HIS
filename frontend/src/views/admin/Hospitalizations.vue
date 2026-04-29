<template>
  <div class="fade-in">
    <el-card>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="nurseID" label="责任护士" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag :type="row.status === 1 ? 'success' : row.status === 3 ? 'warning' : 'info'" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column label="操作" width="80" fixed="right">
          <template #default="{ row }">
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getHospitalizations, deleteHospitalization } from '../../api/admin'

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15

async function loadData() {
  loading.value = true
  try {
    const res = await getHospitalizations()
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此住院记录？', '确认', { type: 'warning' })
    const res = await deleteHospitalization(row.hospitalizationID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
