<template>
  <div>
    <el-card>
      <template #header><span>我的看诊记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="consultationID" label="看诊ID" width="100" />
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="preliminaryDiagnosis" label="初步诊断" min-width="150" show-overflow-tooltip />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" width="180" />
        <el-table-column label="操作" width="80">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="viewDetail(row)">详情</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="detailVisible" title="看诊详情" width="600px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="状态">{{ detail.statusStr }}</el-descriptions-item>
        <el-descriptions-item label="主诉" :span="2">{{ detail.chiefComplaint }}</el-descriptions-item>
        <el-descriptions-item label="现病史" :span="2">{{ detail.historyOfPresentIllness }}</el-descriptions-item>
        <el-descriptions-item label="初步诊断" :span="2">{{ detail.preliminaryDiagnosis }}</el-descriptions-item>
        <el-descriptions-item label="建议住院">{{ detail.isHospitalizationRecommended ? '是' : '否' }}</el-descriptions-item>
        <el-descriptions-item label="创建时间">{{ detail.createTime }}</el-descriptions-item>
      </el-descriptions>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { getMyConsultations } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])
const detailVisible = ref(false)
const detail = reactive({})

async function loadData() {
  loading.value = true
  try { const res = await getMyConsultations(); if (res.code === 200) tableData.value = res.data.list }
  finally { loading.value = false }
}

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

onMounted(loadData)
</script>
