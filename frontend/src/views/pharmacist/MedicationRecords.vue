<template>
  <div>
    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center;">
          <span>用药记录审核</span>
          <el-button type="primary" size="small" @click="showCreateDialog">新建用药记录</el-button>
        </div>
      </template>
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
        <el-table-column label="操作" width="280" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="showDetail(row)">查看详情</el-button>
            <el-button type="success" size="small" @click="handleReview(row, 2)" :disabled="row.reviewStatus !== 1">通过</el-button>
            <el-button type="danger" size="small" @click="handleReview(row, 3)" :disabled="row.reviewStatus !== 1">驳回</el-button>
            <el-button type="primary" size="small" @click="handleDispense(row)" :disabled="row.reviewStatus !== 2 || row.status !== 1">发药</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- Create Dialog -->
    <el-dialog v-model="createDialogVisible" title="新建用药记录" width="450px" destroy-on-close>
      <el-form :model="createForm" label-width="100px">
        <el-form-item label="问诊ID" required>
          <el-input v-model="createForm.consultationID" placeholder="请输入问诊ID" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="createDialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="createLoading" @click="handleCreate">确定</el-button>
      </template>
    </el-dialog>

    <!-- Detail Dialog -->
    <el-dialog v-model="detailDialogVisible" title="用药记录详情" width="700px" destroy-on-close>
      <template v-if="detailData">
        <el-descriptions :column="2" border>
          <el-descriptions-item label="记录ID">{{ detailData.medRecordID }}</el-descriptions-item>
          <el-descriptions-item label="问诊ID">{{ detailData.consultationID }}</el-descriptions-item>
          <el-descriptions-item label="医生ID">{{ detailData.doctorID }}</el-descriptions-item>
          <el-descriptions-item label="患者ID">{{ detailData.patientID }}</el-descriptions-item>
          <el-descriptions-item label="科室">{{ detailData.department }}</el-descriptions-item>
          <el-descriptions-item label="创建时间">{{ detailData.createTime }}</el-descriptions-item>
          <el-descriptions-item label="总费用">{{ detailData.totalCost?.toFixed(2) }} 元</el-descriptions-item>
          <el-descriptions-item label="审核状态">{{ detailData.reviewStatusStr }}</el-descriptions-item>
          <el-descriptions-item label="缴费状态">{{ detailData.statusStr }}</el-descriptions-item>
          <el-descriptions-item label="药师ID">{{ detailData.pharmacistID || '-' }}</el-descriptions-item>
          <el-descriptions-item label="缴费时间">{{ detailData.paymentTime || '-' }}</el-descriptions-item>
          <el-descriptions-item label="发药时间">{{ detailData.dispenseTime || '-' }}</el-descriptions-item>
        </el-descriptions>
        <el-divider content-position="left">用药明细</el-divider>
        <el-table :data="detailData.lines || []" stripe border size="small">
          <el-table-column prop="medicineName" label="药品名称" />
          <el-table-column prop="quantity" label="数量" width="80" />
          <el-table-column prop="unitPrice" label="单价" width="100">
            <template #default="{ row }">{{ row.unitPrice?.toFixed(2) }} 元</template>
          </el-table-column>
          <el-table-column label="小计" width="100">
            <template #default="{ row }">{{ (row.quantity * row.unitPrice)?.toFixed(2) }} 元</template>
          </el-table-column>
        </el-table>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import {
  getMedicationRecords,
  getMedicationRecord,
  createMedicationRecord,
  deleteMedicationRecord,
  reviewMedicationRecord,
  dispenseMedication
} from '../../api/pharmacist'

const loading = ref(false)
const tableData = ref([])

// Create dialog
const createDialogVisible = ref(false)
const createLoading = ref(false)
const createForm = ref({ consultationID: '' })

// Detail dialog
const detailDialogVisible = ref(false)
const detailData = ref(null)

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicationRecords()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

function showCreateDialog() {
  createForm.value = { consultationID: '' }
  createDialogVisible.value = true
}

async function handleCreate() {
  if (!createForm.value.consultationID) {
    ElMessage.warning('请输入问诊ID')
    return
  }
  createLoading.value = true
  try {
    const res = await createMedicationRecord({ consultationID: createForm.value.consultationID })
    if (res.code === 200) {
      ElMessage.success('创建成功')
      createDialogVisible.value = false
      loadData()
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { createLoading.value = false }
}

async function showDetail(row) {
  try {
    const res = await getMedicationRecord(row.medRecordID)
    if (res.code === 200) {
      detailData.value = res.data
      detailDialogVisible.value = true
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
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

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定要删除该用药记录吗？', '确认删除', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    })
    const res = await deleteMedicationRecord(row.medRecordID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
