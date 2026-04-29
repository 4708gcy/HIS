<template>
  <div class="fade-in">
    <el-card>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="medRecordID" label="记录ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="totalCost" label="总费用" width="100">
          <template #default="{ row }">{{ row.totalCost?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="缴费状态" width="100">
          <template #default="{ row }"><el-tag :type="row.status === 1 ? 'success' : 'info'" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="reviewStatusStr" label="审核状态" width="100">
          <template #default="{ row }"><el-tag :type="row.reviewStatus === 2 ? 'success' : 'warning'" size="small">{{ row.reviewStatusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" width="180" />
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
import { getMedicationRecords, deleteMedicationRecord } from '../../api/admin'

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicationRecords()
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
    await ElMessageBox.confirm('确定删除此用药记录？', '确认', { type: 'warning' })
    const res = await deleteMedicationRecord(row.medRecordID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
