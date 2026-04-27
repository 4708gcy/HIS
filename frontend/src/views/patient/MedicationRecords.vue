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
        <el-table-column label="操作" width="160">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="viewDetail(row)">查看详情</el-button>
            <el-button v-if="row.status === 0 && row.reviewStatus === 2" type="success" size="small" @click="handlePay(row)">支付</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="detailVisible" title="用药记录详情" width="800px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="记录ID">{{ detail.medRecordID }}</el-descriptions-item>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="医生ID">{{ detail.doctorID }}</el-descriptions-item>
        <el-descriptions-item label="药师ID">{{ detail.pharmacistID || '-' }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ detail.department }}</el-descriptions-item>
        <el-descriptions-item label="总费用">{{ detail.totalCost?.toFixed(2) }} 元</el-descriptions-item>
        <el-descriptions-item label="缴费状态">
          <el-tag :type="detail.status === 1 ? 'success' : 'info'" size="small">{{ detail.statusStr }}</el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="审核状态">
          <el-tag size="small">{{ detail.reviewStatusStr }}</el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="创建时间">{{ detail.createTime }}</el-descriptions-item>
        <el-descriptions-item label="缴费时间">{{ detail.paymentTime || '-' }}</el-descriptions-item>
        <el-descriptions-item label="发药时间">{{ detail.dispenseTime || '-' }}</el-descriptions-item>
        <el-descriptions-item label="备注" :span="2">{{ detail.note || '-' }}</el-descriptions-item>
      </el-descriptions>

      <div v-if="detail.lines && detail.lines.length" style="margin-top: 16px">
        <h4 style="margin-bottom: 8px">药品明细</h4>
        <el-table :data="detail.lines" border size="small">
          <el-table-column prop="medicineName" label="药品名称" min-width="120" />
          <el-table-column prop="quantity" label="数量" width="80" />
          <el-table-column prop="unitPrice" label="单价" width="100">
            <template #default="{ row }">{{ row.unitPrice?.toFixed(2) }} 元</template>
          </el-table-column>
          <el-table-column prop="note" label="备注" min-width="120">
            <template #default="{ row }">{{ row.note || '-' }}</template>
          </el-table-column>
        </el-table>
      </div>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyMedicationRecords, payMedicationRecord } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])
const detailVisible = ref(false)
const detail = reactive({})

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

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

onMounted(loadData)
</script>
