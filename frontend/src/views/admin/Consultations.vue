<template>
  <div>
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item><el-button type="primary" @click="loadData">搜索</el-button></el-form-item>
      </el-form>
    </el-card>

    <el-card>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="consultationID" label="看诊ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="preliminaryDiagnosis" label="初步诊断" min-width="150" show-overflow-tooltip />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag :type="row.status === 3 ? 'success' : 'info'" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" width="180" />
        <el-table-column label="操作" width="120" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="viewDetail(row)">详情</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <el-dialog v-model="detailVisible" title="看诊详情" width="650px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="状态">{{ detail.statusStr }}</el-descriptions-item>
        <el-descriptions-item label="患者ID">{{ detail.patientID }}</el-descriptions-item>
        <el-descriptions-item label="医生ID">{{ detail.doctorID }}</el-descriptions-item>
        <el-descriptions-item label="主诉" :span="2">{{ detail.chiefComplaint }}</el-descriptions-item>
        <el-descriptions-item label="现病史" :span="2">{{ detail.historyOfPresentIllness }}</el-descriptions-item>
        <el-descriptions-item label="既往史" :span="2">{{ detail.pastMedicalHistory }}</el-descriptions-item>
        <el-descriptions-item label="初步诊断" :span="2">{{ detail.preliminaryDiagnosis }}</el-descriptions-item>
        <el-descriptions-item label="建议住院">{{ detail.isHospitalizationRecommended ? '是' : '否' }}</el-descriptions-item>
        <el-descriptions-item label="创建时间">{{ detail.createTime }}</el-descriptions-item>
      </el-descriptions>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getConsultations, deleteConsultation } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '' })
const detailVisible = ref(false)
const detail = reactive({})

async function loadData() {
  loading.value = true
  try {
    const res = await getConsultations(query)
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此看诊记录？', '确认', { type: 'warning' })
    const res = await deleteConsultation(row.consultationID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
