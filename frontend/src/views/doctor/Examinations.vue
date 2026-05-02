<template>
  <div class="fade-in">
    <el-card>
      <template #header><span>我的检查记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="examinationID" label="检查ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="itemName" label="检查项目" width="120" />
        <el-table-column prop="fee" label="费用" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="reportSummary" label="报告摘要" min-width="150" show-overflow-tooltip />
        <el-table-column label="操作" width="150">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="editVisible" title="编辑检查记录" width="500px">
      <el-form :model="editForm" label-width="80px">
        <el-form-item label="报告摘要"><el-input v-model="editForm.reportSummary" type="textarea" :rows="3" /></el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status">
            <el-option label="待检查" :value="1" /><el-option label="已完成" :value="3" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注"><el-input v-model="editForm.note" type="textarea" /></el-form-item>
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
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMyExaminations, updateExamination, deleteExamination } from '../../api/doctor'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const editVisible = ref(false)
const editForm = reactive({})

async function loadData() {
  loading.value = true
  try {
    const res = await getMyExaminations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

function openEdit(row) { Object.assign(editForm, { ...row }); editVisible.value = true }

async function handleSave() {
  saving.value = true
  try {
    const res = await updateExamination(editForm.examinationID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { saving.value = false }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此检查记录？', '确认', { type: 'warning' })
    const res = await deleteExamination(row.examinationID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
