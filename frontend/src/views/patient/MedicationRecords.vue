<template>
  <div>
    <el-card>
      <template #header><span>我的用药记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="medRecordID" label="记录ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="totalCost" label="总费用" width="100">
          <template #default="{ row }">{{ row.totalCost?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="缴费状态" width="100">
          <template #default="{ row }"><el-tag :type="row.status === 1 ? 'success' : 'info'" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="reviewStatusStr" label="审核状态" width="120">
          <template #default="{ row }"><el-tag size="small">{{ row.reviewStatusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" min-width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button v-if="row.status === 0 && row.reviewStatus === 2" type="success" size="small" @click="handlePay(row)">支付</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyMedicationRecords, payMedicationRecord } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try { const res = await getMyMedicationRecords(); if (res.code === 200) tableData.value = res.data.list }
  catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function handlePay(row) {
  try {
    await ElMessageBox.confirm(`确认支付药费 ${row.totalCost?.toFixed(2)} 元？`, '支付确认')
    const res = await payMedicationRecord(row.medRecordID)
    if (res.code === 200) { ElMessage.success('支付成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
