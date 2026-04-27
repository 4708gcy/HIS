<template>
  <div>
    <!-- 搜索栏 -->
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="姓名">
          <el-input v-model="query.name" clearable placeholder="搜索姓名" />
        </el-form-item>
        <el-form-item label="职称">
          <el-select v-model="query.title" clearable placeholder="全部" style="width: 140px">
            <el-option label="实习医生" value="1" /><el-option label="住院医师" value="2" />
            <el-option label="主治医师" value="3" /><el-option label="副主任医师" value="4" />
            <el-option label="主任医师" value="5" />
          </el-select>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="loadData">搜索</el-button>
          <el-button @click="resetQuery">重置</el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <!-- 表格 -->
    <el-card>
      <el-table :data="tableData" stripe style="width: 100%" v-loading="loading">
        <el-table-column prop="userID" label="医生ID" width="100" />
        <el-table-column prop="username" label="姓名" width="100" />
        <el-table-column prop="gender" label="性别" width="60" />
        <el-table-column prop="age" label="年龄" width="60" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="titleStr" label="职称" width="100" />
        <el-table-column prop="telephone" label="电话" width="130" />
        <el-table-column prop="isOnDuty" label="在岗" width="60">
          <template #default="{ row }">
            <el-tag :type="row.isOnDuty ? 'success' : 'info'" size="small">{{ row.isOnDuty ? '是' : '否' }}</el-tag>
          </template>
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
        <el-pagination
          v-model:current-page="page"
          :page-size="pageSize"
          :total="total"
          layout="prev, pager, next"
          @current-change="loadData"
        />
      </div>
    </el-card>

    <!-- 编辑弹窗 -->
    <el-dialog v-model="editVisible" title="编辑医生" width="500px">
      <el-form :model="editForm" label-width="80px">
        <el-form-item label="姓名"><el-input v-model="editForm.username" /></el-form-item>
        <el-form-item label="性别">
          <el-radio-group v-model="editForm.gender"><el-radio value="男">男</el-radio><el-radio value="女">女</el-radio></el-radio-group>
        </el-form-item>
        <el-form-item label="年龄"><el-input-number v-model="editForm.age" :min="0" :max="150" /></el-form-item>
        <el-form-item label="电话"><el-input v-model="editForm.telephone" /></el-form-item>
        <el-form-item label="科室">
          <el-select v-model="editForm.department" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="职称">
          <el-select v-model="editForm.title" style="width: 100%">
            <el-option label="实习医生" :value="1" /><el-option label="住院医师" :value="2" />
            <el-option label="主治医师" :value="3" /><el-option label="副主任医师" :value="4" />
            <el-option label="主任医师" :value="5" />
          </el-select>
        </el-form-item>
        <el-form-item label="专长"><el-input v-model="editForm.specialty" type="textarea" :rows="2" /></el-form-item>
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
import { getDoctors, updateDoctor, deleteDoctor, updateAccountStatus } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '', name: '', title: '' })
const editVisible = ref(false)
const editForm = reactive({})

function resetQuery() {
  query.department = ''; query.name = ''; query.title = ''
  page.value = 1
  loadData()
}

async function loadData() {
  loading.value = true
  try {
    const res = await getDoctors(query)
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function openEdit(row) {
  Object.assign(editForm, { ...row })
  editVisible.value = true
}

async function handleSave() {
  saving.value = true
  try {
    const res = await updateDoctor(editForm.userID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败') }
  finally { saving.value = false }
}

async function toggleStatus(row) {
  try {
    const action = row.isAccountActive ? '封锁' : '激活'
    await ElMessageBox.confirm(`确定${action}医生 ${row.username}？`, '确认')
    const res = await updateAccountStatus(row.userID, 2, !row.isAccountActive)
    if (res.code === 200) { ElMessage.success(`${action}成功`); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm(`确定删除医生 ${row.username}？此为逻辑删除`, '确认', { type: 'warning' })
    const res = await deleteDoctor(row.userID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
