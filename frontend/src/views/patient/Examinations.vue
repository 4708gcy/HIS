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
        <el-table-column label="操作" width="160">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="viewDetail(row)">查看详情</el-button>
            <el-button v-if="row.status === 0" type="success" size="small" @click="handlePay(row)">支付</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="detailVisible" title="检查详情" width="750px">
      <el-descriptions :column="2" border>
        <el-descriptions-item label="检查ID">{{ detail.examinationID }}</el-descriptions-item>
        <el-descriptions-item label="看诊ID">{{ detail.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="医生ID">{{ detail.doctorID }}</el-descriptions-item>
        <el-descriptions-item label="科室">{{ detail.department }}</el-descriptions-item>
        <el-descriptions-item label="检查项目">{{ detail.itemName }}</el-descriptions-item>
        <el-descriptions-item label="费用">{{ detail.fee?.toFixed(2) }} 元</el-descriptions-item>
        <el-descriptions-item label="状态">
          <el-tag size="small">{{ detail.statusStr }}</el-tag>
        </el-descriptions-item>
        <el-descriptions-item label="下单时间">{{ detail.orderTime }}</el-descriptions-item>
        <el-descriptions-item label="报告时间">{{ detail.reportTime }}</el-descriptions-item>
        <el-descriptions-item label="报告摘要" :span="2">{{ detail.reportSummary || '-' }}</el-descriptions-item>
        <el-descriptions-item label="备注" :span="2">{{ detail.note || '-' }}</el-descriptions-item>
      </el-descriptions>

      <div v-if="detail.vitalSigns" style="margin-top: 16px">
        <h4 style="margin-bottom: 8px">生命体征</h4>
        <el-descriptions :column="3" border size="small">
          <el-descriptions-item label="体温">{{ detail.vitalSigns.temperatureC != null ? detail.vitalSigns.temperatureC + ' °C' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="收缩压">{{ detail.vitalSigns.systolicBP != null ? detail.vitalSigns.systolicBP + ' mmHg' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="舒张压">{{ detail.vitalSigns.diastolicBP != null ? detail.vitalSigns.diastolicBP + ' mmHg' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="心率">{{ detail.vitalSigns.heartRate != null ? detail.vitalSigns.heartRate + ' bpm' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="呼吸频率">{{ detail.vitalSigns.respiratoryRate != null ? detail.vitalSigns.respiratoryRate + ' 次/分' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="血氧">{{ detail.vitalSigns.spo2 != null ? detail.vitalSigns.spo2 + '%' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="身高">{{ detail.vitalSigns.height != null ? detail.vitalSigns.height + ' cm' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="体重">{{ detail.vitalSigns.weight != null ? detail.vitalSigns.weight + ' kg' : '-' }}</el-descriptions-item>
          <el-descriptions-item label="BMI">{{ detail.vitalSigns.bmi != null ? detail.vitalSigns.bmi : '-' }}</el-descriptions-item>
          <el-descriptions-item label="血糖">{{ detail.vitalSigns.bloodSugar != null ? detail.vitalSigns.bloodSugar + ' mmol/L' : '-' }}</el-descriptions-item>
        </el-descriptions>
      </div>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyExaminations, payExamination } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])
const detailVisible = ref(false)
const detail = reactive({})

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

function viewDetail(row) { Object.assign(detail, row); detailVisible.value = true }

onMounted(loadData)
</script>
