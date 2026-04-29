<template>
  <div class="fade-in">
    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>住院记录管理</span>
          <el-button type="primary" @click="openCreate">新建住院</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="consultationID" label="问诊ID" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="120" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="deposit" label="押金(元)" width="100">
          <template #default="{ row }">{{ row.deposit != null ? row.deposit.toFixed(2) : '-' }}</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="statusTagType(row.status)" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column prop="dischargeTime" label="出院时间" width="180" />
        <el-table-column label="操作" width="240" fixed="right">
          <template #default="{ row }">
            <el-button
              v-if="row.status === 1 || row.status === 2"
              type="warning"
              size="small"
              @click="openAssignBed(row)"
            >分配床位</el-button>
            <el-button
              v-if="row.status === 3"
              type="success"
              size="small"
              @click="handleDischarge(row)"
            >办理出院</el-button>
            <el-button
              type="danger"
              size="small"
              @click="handleDelete(row)"
            >删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 新建住院对话框 -->
    <el-dialog v-model="createVisible" title="新建住院" width="500px" @close="resetCreateForm">
      <el-form :model="createForm" label-width="100px" :rules="createRules" ref="createFormRef">
        <el-form-item label="问诊ID" prop="consultationID">
          <el-input v-model="createForm.consultationID" placeholder="请输入问诊ID" />
        </el-form-item>
        <el-form-item label="病房类型" prop="wardType">
          <el-select v-model="createForm.wardType" placeholder="请选择病房类型" style="width: 100%">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="隔离病房" value="隔离病房" />
            <el-option label="VIP病房" value="VIP病房" />
            <el-option label="ICU病房" value="ICU病房" />
          </el-select>
        </el-form-item>
        <el-form-item label="押金(元)" prop="deposit">
          <el-input-number v-model="createForm.deposit" :min="0" :precision="2" style="width: 100%" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="createVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="handleCreate">确认创建</el-button>
      </template>
    </el-dialog>

    <!-- 分配床位对话框 -->
    <el-dialog v-model="assignVisible" title="分配床位" width="600px" @close="availableBeds = []">
      <div v-if="assigningRow" style="margin-bottom: 12px">
        <el-descriptions :column="2" border size="small">
          <el-descriptions-item label="住院ID">{{ assigningRow.hospitalizationID }}</el-descriptions-item>
          <el-descriptions-item label="患者ID">{{ assigningRow.patientID }}</el-descriptions-item>
          <el-descriptions-item label="病房类型">{{ assigningRow.wardType }}</el-descriptions-item>
        </el-descriptions>
      </div>
      <el-table
        :data="availableBeds"
        stripe
        v-loading="bedsLoading"
        max-height="300"
        highlight-current-row
        @current-change="handleBedSelect"
      >
        <el-table-column prop="bedID" label="床位ID" width="100" />
        <el-table-column prop="department" label="科室" width="120" />
        <el-table-column prop="wardType" label="病房类型" width="120" />
        <el-table-column prop="bedNumber" label="床位号" width="100" />
        <el-table-column prop="dailyRate" label="日费用(元)">
          <template #default="{ row }">{{ row.dailyRate?.toFixed(2) }}</template>
        </el-table-column>
      </el-table>
      <div v-if="availableBeds.length === 0 && !bedsLoading" style="text-align: center; color: #999; padding: 20px">
        该病房类型暂无可用床位
      </div>
      <template #footer>
        <el-button @click="assignVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" :disabled="!selectedBed" @click="handleAssignBed">确认分配</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import {
  getHospitalizations,
  createHospitalization,
  deleteHospitalization,
  assignBed,
  dischargePatient,
  getBeds
} from '../../api/nurse'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])

// 新建住院
const createVisible = ref(false)
const createFormRef = ref(null)
const createForm = reactive({
  consultationID: '',
  wardType: '',
  deposit: 0
})
const createRules = {
  consultationID: [{ required: true, message: '请输入问诊ID', trigger: 'blur' }],
  wardType: [{ required: true, message: '请选择病房类型', trigger: 'change' }],
  deposit: [{ required: true, message: '请输入押金', trigger: 'blur' }]
}

// 分配床位
const assignVisible = ref(false)
const assigningRow = ref(null)
const availableBeds = ref([])
const bedsLoading = ref(false)
const selectedBed = ref(null)

function statusTagType(status) {
  switch (status) {
    case 1: return 'info'
    case 2: return 'warning'
    case 3: return 'success'
    case 4: return ''
    case 5: return 'danger'
    default: return 'info'
  }
}

async function loadData() {
  loading.value = true
  try {
    const res = await getHospitalizations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '加载失败')
  } finally { loading.value = false }
}

function openCreate() {
  Object.assign(createForm, { consultationID: '', wardType: '', deposit: 0 })
  createVisible.value = true
}

function resetCreateForm() {
  createFormRef.value?.resetFields()
}

async function handleCreate() {
  try {
    await createFormRef.value.validate()
  } catch { return }
  saving.value = true
  try {
    const res = await createHospitalization({ ...createForm })
    if (res.code === 200) {
      ElMessage.success('创建成功')
      createVisible.value = false
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '创建失败')
  } finally { saving.value = false }
}

async function openAssignBed(row) {
  assigningRow.value = row
  selectedBed.value = null
  availableBeds.value = []
  assignVisible.value = true
  bedsLoading.value = true
  try {
    const res = await getBeds({ wardType: row.wardType, status: 3 })
    if (res.code === 200) availableBeds.value = res.data.list
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '加载床位失败')
  } finally { bedsLoading.value = false }
}

function handleBedSelect(bed) {
  selectedBed.value = bed
}

async function handleAssignBed() {
  if (!selectedBed.value) return
  saving.value = true
  try {
    const res = await assignBed(assigningRow.value.hospitalizationID, {
      bedID: selectedBed.value.bedID
    })
    if (res.code === 200) {
      ElMessage.success('床位分配成功')
      assignVisible.value = false
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '分配失败')
  } finally { saving.value = false }
}

async function handleDischarge(row) {
  try {
    await ElMessageBox.confirm(
      `确认办理住院记录 ${row.hospitalizationID} 的出院手续？`,
      '办理出院',
      { confirmButtonText: '确认', cancelButtonText: '取消', type: 'warning' }
    )
  } catch { return }
  try {
    const res = await dischargePatient(row.hospitalizationID)
    if (res.code === 200) {
      ElMessage.success('出院办理成功')
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败')
  }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm(
      `确认删除住院记录 ${row.hospitalizationID}？此操作不可恢复。`,
      '删除确认',
      { confirmButtonText: '删除', cancelButtonText: '取消', type: 'danger' }
    )
  } catch { return }
  try {
    const res = await deleteHospitalization(row.hospitalizationID)
    if (res.code === 200) {
      ElMessage.success('删除成功')
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '删除失败')
  }
}

onMounted(loadData)
</script>
