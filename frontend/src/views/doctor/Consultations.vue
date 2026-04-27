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
        <el-table-column label="操作" width="160">
          <template #default="{ row }">
            <el-button type="warning" size="small" @click="openExamDialog(row)">开具检查</el-button>
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

    <el-dialog v-model="examDialogVisible" title="开具检查" width="600px" @open="loadExamItems">
      <el-descriptions :column="3" border class="exam-summary">
        <el-descriptions-item label="看诊ID">{{ examRow.consultationID }}</el-descriptions-item>
        <el-descriptions-item label="患者ID">{{ examRow.patientID }}</el-descriptions-item>
        <el-descriptions-item label="初步诊断">{{ examRow.preliminaryDiagnosis }}</el-descriptions-item>
      </el-descriptions>

      <div style="margin-top: 20px">
        <div style="margin-bottom: 10px; font-weight: bold">选择检查项目：</div>
        <el-checkbox-group v-model="examSelectedItems" v-loading="examItemsLoading">
          <el-checkbox
            v-for="item in examItemOptions"
            :key="item.name"
            :label="item.name"
            :value="item.name"
            style="margin-bottom: 8px"
          >
            {{ item.name }}(¥{{ item.fee }})
          </el-checkbox>
        </el-checkbox-group>
        <el-empty v-if="!examItemsLoading && examItemOptions.length === 0" description="暂无可选检查项目" />
      </div>

      <template #footer>
        <el-button @click="examDialogVisible = false">取消</el-button>
        <el-button
          type="primary"
          :loading="examSubmitting"
          :disabled="examSelectedItems.length === 0"
          @click="handleCreateExam"
        >
          确认开具 ({{ examSelectedItems.length }}项)
        </el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage } from 'element-plus'
import { getMyConsultations, updateConsultation, createExaminations } from '../../api/doctor'
import { getExaminationItems } from '../../api/common'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const editVisible = ref(false)
const editForm = reactive({})

// Examination dialog state
const examDialogVisible = ref(false)
const examRow = reactive({})
const examItemOptions = ref([])
const examSelectedItems = ref([])
const examItemsLoading = ref(false)
const examSubmitting = ref(false)

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

function openExamDialog(row) {
  Object.assign(examRow, { ...row })
  examSelectedItems.value = []
  examDialogVisible.value = true
}

async function loadExamItems() {
  examItemsLoading.value = true
  try {
    const res = await getExaminationItems()
    if (res.code === 200) examItemOptions.value = res.data.list || res.data || []
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '加载检查项目失败') }
  finally { examItemsLoading.value = false }
}

async function handleCreateExam() {
  examSubmitting.value = true
  try {
    const res = await createExaminations({
      consultationID: examRow.consultationID,
      items: examSelectedItems.value
    })
    if (res.code === 200) {
      ElMessage.success('检查开具成功')
      examDialogVisible.value = false
      loadData()
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { examSubmitting.value = false }
}

onMounted(loadData)
</script>
