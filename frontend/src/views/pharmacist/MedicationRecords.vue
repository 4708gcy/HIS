<template>
  <div>
    <el-card>
      <template #header><span>用药记录审核</span></template>
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
        <el-table-column prop="reviewStatusStr" label="审核状态" width="120">
          <template #default="{ row }">
            <el-tag :type="row.reviewStatus === 2 ? 'success' : row.reviewStatus === 3 ? 'danger' : 'warning'" size="small">{{ row.reviewStatusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" width="180" />
        <el-table-column label="操作" width="180" fixed="right">
          <template #default="{ row }">
            <el-button type="success" size="small" @click="handleReview(row, 2)" :disabled="row.reviewStatus !== 1">通过</el-button>
            <el-button type="danger" size="small" @click="handleReview(row, 3)" :disabled="row.reviewStatus !== 1">驳回</el-button>
            <el-button type="primary" size="small" @click="handleDispense(row)" :disabled="row.reviewStatus !== 2 || row.status !== 1">发药</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getMedicationRecords, reviewMedicationRecord, dispenseMedication } from '../../api/pharmacist'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicationRecords()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function handleReview(row, status) {
  try {
    const res = await reviewMedicationRecord(row.medRecordID, status)
    if (res.code === 200) { ElMessage.success(status === 2 ? '审核通过' : '已驳回'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDispense(row) {
  try {
    const res = await dispenseMedication(row.medRecordID)
    if (res.code === 200) { ElMessage.success('发药成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
