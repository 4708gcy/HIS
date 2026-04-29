<template>
  <div class="fade-in">
    <el-card>
      <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 12px;">
        <span style="font-size: 14px; color: var(--his-text-secondary);">共 {{ total }} 条记录</span>
        <el-button type="primary" size="small" @click="showCreateDialog">新建住院</el-button>
      </div>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="nurseID" label="责任护士" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag :type="row.status === 1 ? 'success' : row.status === 3 ? 'warning' : 'info'" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column label="操作" width="80" fixed="right">
          <template #default="{ row }">
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: flex-end">
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <!-- 新建弹窗 -->
    <el-dialog v-model="createVisible" title="新建住院" width="500px" destroy-on-close>
      <el-form :model="createForm" label-width="80px">
        <el-form-item label="看诊ID" required>
          <el-select v-model="createForm.consultationID" filterable placeholder="请选择看诊记录" style="width: 100%">
            <el-option v-for="c in consultations" :key="c.consultationID" :label="`${c.consultationID} - 患者${c.patientID}`" :value="c.consultationID" />
          </el-select>
        </el-form-item>
        <el-form-item label="病房类型">
          <el-select v-model="createForm.wardType" style="width: 100%">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="单人病房" value="单人病房" />
            <el-option label="VIP病房" value="VIP病房" />
          </el-select>
        </el-form-item>
        <el-form-item label="押金"><el-input-number v-model="createForm.deposit" :min="0" :max="999999" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="createVisible = false">取消</el-button>
        <el-button type="primary" :loading="createLoading" @click="handleCreate">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getHospitalizations, createHospitalization, deleteHospitalization, getConsultations } from '../../api/admin'

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15

async function loadData() {
  loading.value = true
  try {
    const res = await getHospitalizations()
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此住院记录？', '确认', { type: 'warning' })
    const res = await deleteHospitalization(row.hospitalizationID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

const consultations = ref([])
getConsultations().then(res => { consultations.value = res.data.list })

const createVisible = ref(false)
const createLoading = ref(false)
const createForm = reactive({ consultationID: '', wardType: '普通病房', deposit: 0 })

function showCreateDialog() {
  Object.assign(createForm, { consultationID: '', wardType: '普通病房', deposit: 0 })
  createVisible.value = true
}

async function handleCreate() {
  if (!createForm.consultationID) { ElMessage.warning('请选择看诊记录'); return }
  createLoading.value = true
  try {
    const res = await createHospitalization(createForm)
    if (res.code === 200) { ElMessage.success('添加成功'); createVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '添加失败') }
  finally { createLoading.value = false }
}

onMounted(loadData)
</script>
