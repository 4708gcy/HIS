<template>
  <div>
    <el-card>
      <template #header><span>住院记录管理</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column label="操作" width="100">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <el-dialog v-model="editVisible" title="编辑住院记录" width="500px">
      <el-form :model="editForm" label-width="100px">
        <el-form-item label="床位号"><el-input v-model="editForm.bedNumber" /></el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status" style="width: 100%">
            <el-option label="住院中" :value="1" /><el-option label="出院审批中" :value="2" /><el-option label="已出院" :value="3" />
          </el-select>
        </el-form-item>
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
import { getHospitalizations, updateHospitalization } from '../../api/nurse'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const editVisible = ref(false)
const editForm = reactive({})

async function loadData() {
  loading.value = true
  try {
    const res = await getHospitalizations()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

function openEdit(row) { Object.assign(editForm, { ...row }); editVisible.value = true }

async function handleSave() {
  saving.value = true
  try {
    const res = await updateHospitalization(editForm.hospitalizationID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { saving.value = false }
}

onMounted(loadData)
</script>
