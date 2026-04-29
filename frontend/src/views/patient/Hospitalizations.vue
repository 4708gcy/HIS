<template>
  <div class="fade-in">
    <el-card>
      <template #header><span>我的住院记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="nurseID" label="责任护士" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="statusTagType(row.status)" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column prop="dischargeTime" label="出院时间" min-width="180" />
        <el-table-column label="操作" width="200">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="viewDetail(row)">查看详情</el-button>
            <el-button v-if="row.status === 1" type="warning" size="small" @click="openPayDialog(row)">缴纳押金</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="detailVisible" title="住院详情" width="750px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="住院ID">{{ detail.hospitalizationID }}</el-descriptions-item>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="医生ID">{{ detail.doctorID }}</el-descriptions-item>
        <el-descriptions-item label="护士ID">{{ detail.nurseID || '-' }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ detail.department }}</el-descriptions-item>
        <el-descriptions-item label="病房类型">{{ detail.wardType }}</el-descriptions-item>
        <el-descriptions-item label="床位号">{{ detail.bedNumber || '-' }}</el-descriptions-item>
        <el-descriptions-item label="状态">
          <el-tag :type="statusTagType(detail.status)" size="small">{{ detail.statusStr }}</el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="申请时间">{{ detail.applyTime }}</el-descriptions-item>
        <el-descriptions-item label="入院时间">{{ detail.admitTime || '-' }}</el-descriptions-item>
        <el-descriptions-item label="出院时间">{{ detail.dischargeTime || '-' }}</el-descriptions-item>
        <el-descriptions-item label="押金">{{ detail.deposit != null ? detail.deposit.toFixed(2) + ' 元' : '-' }}</el-descriptions-item>
        <el-descriptions-item label="总费用">{{ detail.totalCost != null ? detail.totalCost.toFixed(2) + ' 元' : '-' }}</el-descriptions-item>
      </el-descriptions>
    </el-dialog>

    <el-dialog v-model="payVisible" title="缴纳押金" width="400px">
      <el-form label-width="80px">
        <el-form-item label="住院ID">
          <span>{{ payRow.hospitalizationID }}</span>
        </el-form-item>
        <el-form-item label="押金金额">
          <el-input-number v-model="depositAmount" :min="1" :max="99999" :precision="2" :step="100" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="payVisible = false">取消</el-button>
        <el-button type="primary" :loading="payLoading" @click="handlePayDeposit">确认缴纳</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getMyHospitalizations, payHospitalization } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])
const detailVisible = ref(false)
const detail = reactive({})
const payVisible = ref(false)
const payRow = reactive({})
const depositAmount = ref(100)
const payLoading = ref(false)

function statusTagType(status) {
  switch (status) {
    case 1: return 'warning'
    case 2: return 'info'
    case 3: return 'success'
    case 4: return ''
    case 5: return 'danger'
    default: return 'info'
  }
}

async function loadData() {
  loading.value = true
  try { const res = await getMyHospitalizations(); if (res.code === 200) tableData.value = res.data.list }
  finally { loading.value = false }
}

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

function openPayDialog(row) {
  Object.assign(payRow, row)
  depositAmount.value = 100
  payVisible.value = true
}

async function handlePayDeposit() {
  if (!depositAmount.value || depositAmount.value <= 0) {
    ElMessage.warning('请输入有效的押金金额')
    return
  }
  payLoading.value = true
  try {
    const res = await payHospitalization(payRow.hospitalizationID, depositAmount.value)
    if (res.code === 200) {
      ElMessage.success('押金缴纳成功')
      payVisible.value = false
      loadData()
    } else {
      ElMessage.error(res.message || '缴纳失败')
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  } finally {
    payLoading.value = false
  }
}

onMounted(loadData)
</script>
