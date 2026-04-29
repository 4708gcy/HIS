<template>
  <div class="fade-in">
    <el-card>
      <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 12px;">
        <span style="font-size: 14px; color: var(--his-text-secondary);">共 {{ total }} 条记录</span>
        <el-button type="primary" size="small" @click="showCreateDialog">新建药剂师</el-button>
      </div>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="userID" label="药剂师ID" width="110" />
        <el-table-column prop="username" label="姓名" width="100" />
        <el-table-column prop="gender" label="性别" width="60" />
        <el-table-column prop="age" label="年龄" width="60" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="levelStr" label="级别" width="100">
          <template #default="{ row }">{{ ['', '初级药剂师', '中级药剂师', '高级药剂师'][row.level] || row.level }}</template>
        </el-table-column>
        <el-table-column prop="telephone" label="电话" width="130" />
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
      <div style="margin-top: 16px; display: flex; justify-content: flex-end">
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <!-- 新建弹窗 -->
    <el-dialog v-model="createVisible" title="新建药剂师" width="500px">
      <el-form :model="createForm" label-width="80px">
        <el-form-item label="用户名" required><el-input v-model="createForm.username" /></el-form-item>
        <el-form-item label="密码" required><el-input v-model="createForm.password" type="password" show-password /></el-form-item>
        <el-form-item label="性别">
          <el-radio-group v-model="createForm.gender"><el-radio value="男">男</el-radio><el-radio value="女">女</el-radio></el-radio-group>
        </el-form-item>
        <el-form-item label="年龄"><el-input-number v-model="createForm.age" :min="0" :max="150" /></el-form-item>
        <el-form-item label="电话"><el-input v-model="createForm.telephone" /></el-form-item>
        <el-form-item label="科室">
          <el-select v-model="createForm.department" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="等级">
          <el-select v-model="createForm.level" style="width: 100%">
            <el-option label="实习药剂师" :value="1" /><el-option label="初级药剂师" :value="2" />
            <el-option label="中级药剂师" :value="3" /><el-option label="高级药剂师" :value="4" />
            <el-option label="主任药剂师" :value="5" />
          </el-select>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="createVisible = false">取消</el-button>
        <el-button type="primary" :loading="createLoading" @click="handleCreate">确定</el-button>
      </template>
    </el-dialog>

    <el-dialog v-model="editVisible" title="编辑药剂师" width="500px">
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
        <el-form-item label="级别">
          <el-select v-model="editForm.level" style="width: 100%">
            <el-option label="初级药剂师" :value="1" /><el-option label="中级药剂师" :value="2" /><el-option label="高级药剂师" :value="3" />
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
import { ElMessage, ElMessageBox } from 'element-plus'
import { getPharmacists, createPharmacist, updatePharmacist, deletePharmacist, updateAccountStatus } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const editVisible = ref(false)
const editForm = reactive({})
const createVisible = ref(false)
const createLoading = ref(false)
const createForm = reactive({ username: '', password: '', gender: '男', age: 25, telephone: '', department: '急诊科', level: 1 })

async function loadData() {
  loading.value = true
  try {
    const res = await getPharmacists()
    if (res.code === 200) {
      const list = res.data.list
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function showCreateDialog() {
  Object.assign(createForm, { username: '', password: '', gender: '男', age: 25, telephone: '', department: '急诊科', level: 1 })
  createVisible.value = true
}

async function handleCreate() {
  if (!createForm.username || !createForm.password) { ElMessage.warning('用户名和密码不能为空'); return }
  createLoading.value = true
  try {
    const res = await createPharmacist(createForm)
    if (res.code === 200) { ElMessage.success('添加成功'); createVisible.value = false; loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '添加失败') }
  finally { createLoading.value = false }
}

function openEdit(row) { Object.assign(editForm, { ...row }); editVisible.value = true }

async function handleSave() {
  saving.value = true
  try {
    const res = await updatePharmacist(editForm.userID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败') }
  finally { saving.value = false }
}

async function toggleStatus(row) {
  try {
    const action = row.isAccountActive ? '封锁' : '激活'
    await ElMessageBox.confirm(`确定${action}药剂师 ${row.username}？`, '确认')
    const res = await updateAccountStatus(row.userID, 4, !row.isAccountActive)
    if (res.code === 200) { ElMessage.success(`${action}成功`); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm(`确定删除药剂师 ${row.username}？`, '确认', { type: 'warning' })
    const res = await deletePharmacist(row.userID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
