<template>
  <div>
    <el-card>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="userID" label="患者ID" width="100" />
        <el-table-column prop="username" label="姓名" width="100" />
        <el-table-column prop="gender" label="性别" width="60" />
        <el-table-column prop="age" label="年龄" width="60" />
        <el-table-column prop="telephone" label="电话" width="130" />
        <el-table-column prop="balance" label="余额" width="100">
          <template #default="{ row }">{{ row.balance?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="isAccountActive" label="状态" width="80">
          <template #default="{ row }">
            <el-tag :type="row.isAccountActive ? 'success' : 'danger'" size="small">{{ row.isAccountActive ? '正常' : '锁定' }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="200" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
            <el-button :type="row.isAccountActive ? 'warning' : 'success'" size="small" @click="toggleStatus(row)">
              {{ row.isAccountActive ? '封锁' : '激活' }}
            </el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <el-dialog v-model="editVisible" title="编辑患者" width="500px">
      <el-form :model="editForm" label-width="100px">
        <el-form-item label="姓名"><el-input v-model="editForm.username" /></el-form-item>
        <el-form-item label="性别">
          <el-radio-group v-model="editForm.gender"><el-radio value="男">男</el-radio><el-radio value="女">女</el-radio></el-radio-group>
        </el-form-item>
        <el-form-item label="年龄"><el-input-number v-model="editForm.age" :min="0" :max="150" /></el-form-item>
        <el-form-item label="电话"><el-input v-model="editForm.telephone" /></el-form-item>
        <el-form-item label="紧急联系人"><el-input v-model="editForm.emergencyContactName" /></el-form-item>
        <el-form-item label="紧急电话"><el-input v-model="editForm.emergencyContactPhone" /></el-form-item>
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
import { getPatients, updatePatient, deletePatient, updateAccountStatus } from '../../api/admin'

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const editVisible = ref(false)
const editForm = reactive({})

async function loadData() {
  loading.value = true
  try {
    const res = await getPatients()
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function openEdit(row) { Object.assign(editForm, { ...row }); editVisible.value = true }

async function handleSave() {
  saving.value = true
  try {
    const res = await updatePatient(editForm.userID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败') }
  finally { saving.value = false }
}

async function toggleStatus(row) {
  try {
    const action = row.isAccountActive ? '封锁' : '激活'
    await ElMessageBox.confirm(`确定${action}患者 ${row.username}？`, '确认')
    const res = await updateAccountStatus(row.userID, 5, !row.isAccountActive)
    if (res.code === 200) { ElMessage.success(`${action}成功`); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm(`确定删除患者 ${row.username}？`, '确认', { type: 'warning' })
    const res = await deletePatient(row.userID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
