<template>
  <div>
    <el-card>
      <template #header><span>我的看诊记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="consultationID" label="看诊ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="preliminaryDiagnosis" label="初步诊断" min-width="150" show-overflow-tooltip />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="createTime" label="创建时间" width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="editVisible" title="编辑看诊记录" width="600px">
      <el-form :model="editForm" label-width="100px">
        <el-form-item label="主诉"><el-input v-model="editForm.chiefComplaint" type="textarea" :rows="2" /></el-form-item>
        <el-form-item label="现病史"><el-input v-model="editForm.historyOfPresentIllness" type="textarea" :rows="2" /></el-form-item>
        <el-form-item label="既往史"><el-input v-model="editForm.pastMedicalHistory" type="textarea" :rows="2" /></el-form-item>
        <el-form-item label="家族史"><el-input v-model="editForm.familyHistory" /></el-form-item>
        <el-form-item label="初步诊断"><el-input v-model="editForm.preliminaryDiagnosis" /></el-form-item>
        <el-form-item label="建议住院">
          <el-switch v-model="editForm.isHospitalizationRecommended" />
        </el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status">
            <el-option label="进行中" :value="1" /><el-option label="已完成" :value="3" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注"><el-input v-model="editForm.note" type="textarea" :rows="2" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="editVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="handleSave">保存</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getMyConsultations, updateConsultation } from '../../api/doctor'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const editVisible = ref(false)
const editForm = reactive({})

async function loadData() {
  loading.value = true
  try {
    const res = await getMyConsultations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

function openEdit(row) { Object.assign(editForm, { ...row }); editVisible.value = true }

async function handleSave() {
  saving.value = true
  try {
    const res = await updateConsultation(editForm.consultationID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { saving.value = false }
}

onMounted(loadData)
</script>
