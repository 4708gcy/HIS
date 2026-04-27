<template>
  <div>
    <el-card>
      <template #header><span>我的检查记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="examinationID" label="检查ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="itemName" label="检查项目" width="120" />
        <el-table-column prop="fee" label="费用" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="reportSummary" label="报告摘要" min-width="150" show-overflow-tooltip />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button v-if="row.status === 0" type="success" size="small" @click="handlePay(row)">支付</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyExaminations, payExamination } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try { const res = await getMyExaminations(); if (res.code === 200) tableData.value = res.data.list }
  catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function handlePay(row) {
  try {
    await ElMessageBox.confirm(`确认支付检查费 ${row.fee?.toFixed(2)} 元？`, '支付确认')
    const res = await payExamination(row.examinationID)
    if (res.code === 200) { ElMessage.success('支付成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
