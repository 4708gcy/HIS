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

    <el-dialog v-model="detailVisible" title="看诊详情" width="800px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="挂号ID">{{ detail.registrationID }}</el-descriptions-item>
        <el-descriptions-item label="患者ID">{{ detail.patientID }}</el-descriptions-item>
        <el-descriptions-item label="医生ID">{{ detail.doctorID }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ detail.department }}</el-descriptions-item>
        <el-descriptions-item label="看诊时间">{{ detail.consultationTime }}</el-descriptions-item>
        <el-descriptions-item label="状态">
          <el-tag size="small">{{ detail.statusStr }}</el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="处方已审核">
          <el-tag :type="detail.isPrescriptionReviewed ? 'success' : 'info'" size="small">
            {{ detail.isPrescriptionReviewed ? '是' : '否' }}
          </el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="建议住院">
          <el-tag :type="detail.isHospitalizationRecommended ? 'warning' : 'info'" size="small">
            {{ detail.isHospitalizationRecommended ? '是' : '否' }}
          </el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="主诉" :span="2">{{ detail.chiefComplaint || '-' }}</el-descriptions-item>
        <el-descriptions-item label="现病史" :span="2">{{ detail.historyOfPresentIllness || '-' }}</el-descriptions-item>
        <el-descriptions-item label="既往史" :span="2">{{ detail.pastMedicalHistory || '-' }}</el-descriptions-item>
        <el-descriptions-item label="家族史" :span="2">{{ detail.familyHistory || '-' }}</el-descriptions-item>
        <el-descriptions-item label="初步诊断" :span="2">{{ detail.preliminaryDiagnosis || '-' }}</el-descriptions-item>
        <el-descriptions-item label="备注" :span="2">{{ detail.note || '-' }}</el-descriptions-item>
      </el-descriptions>

      <div v-if="detail.examinationList && detail.examinationList.length" style="margin-top: 16px">
        <h4 style="margin-bottom: 8px">检查项目</h4>
        <div>
          <el-tag
            v-for="(item, idx) in detail.examinationList"
            :key="idx"
            style="margin: 2px 4px"
          >{{ typeof item === 'string' ? item : item.itemName || item }}</el-tag>
        </div>
      </div>

      <div v-if="detail.prescriptions && detail.prescriptions.length" style="margin-top: 16px">
        <h4 style="margin-bottom: 8px">处方</h4>
        <el-table :data="detail.prescriptions" border size="small">
          <el-table-column prop="name" label="药品名称" min-width="120" />
          <el-table-column prop="quantity" label="数量" width="80" />
          <el-table-column prop="dosage" label="用法用量" min-width="120" />
          <el-table-column prop="frequency" label="频次" width="100" />
          <el-table-column prop="duration" label="疗程" width="100" />
        </el-table>
      </div>
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
