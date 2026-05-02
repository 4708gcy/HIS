<template>
  <div class="fade-in">
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
      <div class="no-print" style="margin-bottom: 12px; text-align: right">
        <el-button type="primary" @click="printPrescription">
          <el-icon><Printer /></el-icon> 打印处方
        </el-button>
      </div>
      <div ref="printArea">
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
      </div><!-- /printArea -->
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { Printer } from '@element-plus/icons-vue'
import { getMyConsultations } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])
const detailVisible = ref(false)
const detail = reactive({})
const printArea = ref(null)

async function loadData() {
  loading.value = true
  try { const res = await getMyConsultations(); if (res.code === 200) tableData.value = res.data.list }
  finally { loading.value = false }
}

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

function printPrescription() {
  const content = printArea.value
  if (!content) return
  const printWindow = window.open('', '_blank')
  printWindow.document.write(`
    <html><head><title>处方 - ${detail.consultationID}</title>
    <style>
      body { font-family: "SimSun", serif; padding: 40px; color: #000; }
      h2 { text-align: center; margin-bottom: 4px; }
      h4 { margin: 16px 0 8px; border-bottom: 1px solid #000; padding-bottom: 4px; }
      table { width: 100%; border-collapse: collapse; margin: 8px 0; }
      th, td { border: 1px solid #000; padding: 6px 8px; text-align: left; font-size: 14px; }
      th { background: #f0f0f0; }
      .info-row { display: flex; gap: 24px; margin: 4px 0; font-size: 14px; }
      .info-row span { min-width: 200px; }
      .footer { margin-top: 40px; text-align: right; font-size: 14px; }
    </style></head><body>
    <h2>门诊处方笺</h2>
    <div class="info-row"><span>看诊编号：${detail.consultationID}</span><span>挂号编号：${detail.registrationID}</span></div>
    <div class="info-row"><span>患者编号：${detail.patientID}</span><span>医生编号：${detail.doctorID}</span></div>
    <div class="info-row"><span>科室：${detail.department}</span><span>看诊时间：${detail.consultationTime}</span></div>
    <h4>诊断</h4>
    <p>${detail.preliminaryDiagnosis || '-'}</p>
    <h4>主诉</h4>
    <p>${detail.chiefComplaint || '-'}</p>
    <h4>处方</h4>
    ${(detail.prescriptions && detail.prescriptions.length) ? `
    <table><tr><th>药品名称</th><th>数量</th><th>用法用量</th><th>频次</th><th>疗程</th><th>备注</th></tr>
    ${detail.prescriptions.map(p => `<tr><td>${p.name||'-'}</td><td>${p.quantity||'-'}</td><td>${p.dosage||'-'}</td><td>${p.frequency||'-'}</td><td>${p.duration||'-'}</td><td>${p.note||'-'}</td></tr>`).join('')}
    </table>` : '<p>无处方</p>'}
    ${(detail.examinationList && detail.examinationList.length) ? `<h4>检查项目</h4><p>${detail.examinationList.join('、')}</p>` : ''}
    ${detail.note && detail.note !== '#' ? `<h4>医嘱备注</h4><p>${detail.note}</p>` : ''}
    <div class="footer">打印时间：${new Date().toLocaleString('zh-CN')}</div>
    </body></html>`)
  printWindow.document.close()
  printWindow.print()
}

onMounted(loadData)
</script>
