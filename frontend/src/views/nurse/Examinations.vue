<template>
  <div>
    <el-card>
      <template #header><span>体征录入</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="examinationID" label="检查ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="itemName" label="检查项目" width="120" />
        <el-table-column prop="statusStr" label="状态" width="100" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openVitals(row)">录入体征</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="vitalsVisible" title="体征录入" width="500px">
      <el-form :model="vitalsForm" label-width="100px">
        <el-form-item label="体温(℃)"><el-input-number v-model="vitalsForm.temperatureC" :precision="1" :step="0.1" /></el-form-item>
        <el-form-item label="收缩压"><el-input-number v-model="vitalsForm.systolicBP" :min="0" /></el-form-item>
        <el-form-item label="舒张压"><el-input-number v-model="vitalsForm.diastolicBP" :min="0" /></el-form-item>
        <el-form-item label="心率"><el-input-number v-model="vitalsForm.heartRate" :min="0" /></el-form-item>
        <el-form-item label="呼吸频率"><el-input-number v-model="vitalsForm.respiratoryRate" :min="0" /></el-form-item>
        <el-form-item label="血氧(%)"><el-input-number v-model="vitalsForm.spo2" :min="0" :max="100" /></el-form-item>
        <el-form-item label="身高(cm)"><el-input-number v-model="vitalsForm.height" :min="0" /></el-form-item>
        <el-form-item label="体重(kg)"><el-input-number v-model="vitalsForm.weight" :precision="1" :step="0.1" /></el-form-item>
        <el-form-item label="血糖"><el-input-number v-model="vitalsForm.bloodSugar" :precision="1" :step="0.1" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="vitalsVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="handleSave">保存</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getExaminations, updateVitalSigns } from '../../api/nurse'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const vitalsVisible = ref(false)
const currentExamId = ref('')
const vitalsForm = reactive({
  temperatureC: 36.5, systolicBP: 120, diastolicBP: 80, heartRate: 72,
  respiratoryRate: 18, spo2: 98, height: 170, weight: 65, bmi: 0, bloodSugar: 5.0
})

async function loadData() {
  loading.value = true
  try {
    const res = await getExaminations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

function openVitals(row) {
  currentExamId.value = row.examinationID
  if (row.vitalSigns) Object.assign(vitalsForm, row.vitalSigns)
  vitalsVisible.value = true
}

async function handleSave() {
  saving.value = true
  try {
    vitalsForm.bmi = vitalsForm.weight > 0 ? +(vitalsForm.weight / ((vitalsForm.height / 100) ** 2)).toFixed(1) : 0
    const res = await updateVitalSigns(currentExamId.value, { ...vitalsForm })
    if (res.code === 200) { ElMessage.success('保存成功'); vitalsVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { saving.value = false }
}

onMounted(loadData)
</script>
