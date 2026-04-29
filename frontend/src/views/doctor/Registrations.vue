<template>
  <div class="fade-in">
    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>我的挂号列表</span>
          <el-select v-model="statusFilter" placeholder="状态筛选" style="width: 120px" clearable @change="handleFilter">
            <el-option label="全部" :value="-1" />
            <el-option v-for="(label, key) in statusMap" :key="key" :label="label" :value="Number(key)" />
          </el-select>
        </div>
      </template>
      <el-table :data="filteredData" stripe v-loading="loading">
        <el-table-column prop="registrationID" label="挂号ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="fee" label="挂号费" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="statusType[row.status]" size="small">{{ statusMap[row.status] || row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="registerTime" label="挂号时间" min-width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button v-if="row.status === 1" type="primary" size="small" @click="openDialog(row)">开始看诊</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 看诊弹窗 -->
    <el-dialog v-model="dialogVisible" title="开始看诊" width="700px" @closed="resetForm">
      <el-form :model="consultForm" label-width="90px">
        <el-form-item label="挂号ID">
          <el-input :model-value="consultForm.registrationID" disabled />
        </el-form-item>
        <el-form-item label="患者ID">
          <el-input :model-value="consultForm.patientID" disabled />
        </el-form-item>
        <el-form-item label="主诉">
          <el-input v-model="consultForm.chiefComplaint" type="textarea" :rows="2" placeholder="请输入主诉" />
        </el-form-item>
        <el-form-item label="现病史">
          <el-input v-model="consultForm.historyOfPresentIllness" type="textarea" :rows="2" placeholder="请输入现病史" />
        </el-form-item>
        <el-form-item label="既往史">
          <el-input v-model="consultForm.pastMedicalHistory" type="textarea" :rows="2" placeholder="请输入既往史" />
        </el-form-item>
        <el-form-item label="家族史">
          <el-input v-model="consultForm.familyHistory" placeholder="请输入家族史" />
        </el-form-item>
        <el-form-item label="初步诊断">
          <el-input v-model="consultForm.preliminaryDiagnosis" placeholder="请输入初步诊断" />
        </el-form-item>
        <el-form-item label="检查项目">
          <el-checkbox-group v-model="consultForm.examinationList">
            <el-row :gutter="8">
              <el-col :span="6" v-for="item in examinationItems" :key="item.name">
                <el-checkbox :label="item.name" :value="item.name" style="margin-bottom: 8px" />
              </el-col>
            </el-row>
          </el-checkbox-group>
        </el-form-item>
        <el-form-item label="建议住院">
          <el-switch v-model="consultForm.isHospitalizationRecommended" />
        </el-form-item>
        <el-form-item label="备注">
          <el-input v-model="consultForm.note" type="textarea" :rows="2" placeholder="请输入备注" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="submitting" @click="handleSubmit">提交看诊</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, computed, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getMyRegistrations, createConsultation } from '../../api/doctor'
import { getExaminationItems } from '../../api/common'

const statusMap = { 0: '已预约', 1: '已支付', 2: '已取消', 3: '已完成' }
const statusType = ['info', 'success', 'warning', 'danger']

const loading = ref(false)
const submitting = ref(false)
const tableData = ref([])
const statusFilter = ref(-1)
const dialogVisible = ref(false)
const examinationItems = ref([])

const consultForm = reactive({
  registrationID: '',
  patientID: '',
  chiefComplaint: '',
  historyOfPresentIllness: '',
  pastMedicalHistory: '',
  familyHistory: '',
  preliminaryDiagnosis: '',
  examinationList: [],
  isHospitalizationRecommended: false,
  note: ''
})

const filteredData = computed(() => {
  if (statusFilter.value === -1) return tableData.value
  return tableData.value.filter(row => row.status === statusFilter.value)
})

function handleFilter() {
  // filteredData is computed, no action needed
}

function resetForm() {
  consultForm.registrationID = ''
  consultForm.patientID = ''
  consultForm.chiefComplaint = ''
  consultForm.historyOfPresentIllness = ''
  consultForm.pastMedicalHistory = ''
  consultForm.familyHistory = ''
  consultForm.preliminaryDiagnosis = ''
  consultForm.examinationList = []
  consultForm.isHospitalizationRecommended = false
  consultForm.note = ''
}

function openDialog(row) {
  consultForm.registrationID = row.registrationID
  consultForm.patientID = row.patientID
  dialogVisible.value = true
}

async function loadExaminationItems() {
  try {
    const res = await getExaminationItems()
    if (res.code === 200) examinationItems.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function loadData() {
  loading.value = true
  try {
    const res = await getMyRegistrations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function handleSubmit() {
  if (!consultForm.preliminaryDiagnosis) { ElMessage.warning('请填写初步诊断'); return }
  submitting.value = true
  try {
    const res = await createConsultation(consultForm)
    if (res.code === 200) { ElMessage.success('看诊记录创建成功'); dialogVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { submitting.value = false }
}

onMounted(() => {
  loadData()
  loadExaminationItems()
})
</script>
