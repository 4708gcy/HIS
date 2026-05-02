<template>
  <div class="fade-in">
    <!-- 搜索栏 -->
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="日期">
          <el-date-picker v-model="query.date" type="date" value-format="YYYY-MM-DD" placeholder="选择日期" clearable style="width: 160px" />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="loadData">搜索</el-button>
          <el-button @click="resetQuery">重置</el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <!-- 表格 -->
    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>排班列表</span>
          <el-button type="success" @click="openAdd">新建排班</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="scheduleID" label="排班ID" width="120" />
        <el-table-column prop="doctorName" label="医生姓名" width="120" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="date" label="日期" width="120" />
        <el-table-column prop="timeSlot" label="时段" width="100" />
        <el-table-column prop="note" label="备注" min-width="150" />
        <el-table-column label="操作" width="160" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>

    <!-- 新建排班 -->
    <el-dialog v-model="addVisible" title="新建排班" width="500px">
      <el-form ref="addFormRef" :model="addForm" :rules="addRules" label-width="80px">
        <el-form-item label="医生" prop="doctorID">
          <el-select v-model="addForm.doctorID" filterable placeholder="请选择医生" style="width: 100%">
            <el-option v-for="d in doctorList" :key="d.userID" :label="`${d.username} (${d.department})`" :value="d.userID" />
          </el-select>
        </el-form-item>
        <el-form-item label="日期" prop="date">
          <el-date-picker v-model="addForm.date" type="date" value-format="YYYY-MM-DD" placeholder="选择日期" style="width: 100%" />
        </el-form-item>
        <el-form-item label="时段" prop="timeSlot">
          <el-select v-model="addForm.timeSlot" placeholder="请选择时段" style="width: 100%">
            <el-option label="上午" value="上午" />
            <el-option label="下午" value="下午" />
            <el-option label="全天" value="全天" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注"><el-input v-model="addForm.note" type="textarea" :rows="2" placeholder="请输入备注" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="addVisible = false">取消</el-button>
        <el-button type="primary" :loading="addSaving" @click="handleAdd">确定</el-button>
      </template>
    </el-dialog>

    <!-- 编辑排班 -->
    <el-dialog v-model="editVisible" title="编辑排班" width="500px">
      <el-form ref="editFormRef" :model="editForm" :rules="editRules" label-width="80px">
        <el-form-item label="医生">
          <el-input :model-value="editForm.doctorName || editForm.doctorID" disabled />
        </el-form-item>
        <el-form-item label="日期" prop="date">
          <el-date-picker v-model="editForm.date" type="date" value-format="YYYY-MM-DD" placeholder="选择日期" style="width: 100%" />
        </el-form-item>
        <el-form-item label="时段" prop="timeSlot">
          <el-select v-model="editForm.timeSlot" placeholder="请选择时段" style="width: 100%">
            <el-option label="上午" value="上午" />
            <el-option label="下午" value="下午" />
            <el-option label="全天" value="全天" />
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
import { ElMessage, ElMessageBox } from 'element-plus'
import { getSchedules, createSchedule, updateSchedule, deleteSchedule } from '../../api/admin'
import { getDoctors } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const doctorList = ref([])
getDoctors().then(res => { doctorList.value = res.data.list || [] }).catch(() => {})

const loading = ref(false)
const saving = ref(false)
const addSaving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '', date: '' })

// 添加
const addVisible = ref(false)
const addFormRef = ref(null)
const addForm = reactive({ doctorID: '', date: '', timeSlot: '', note: '' })
const addRules = {
  doctorID: [{ required: true, message: '请选择医生', trigger: 'change' }],
  date: [{ required: true, message: '请选择日期', trigger: 'change' }],
  timeSlot: [{ required: true, message: '请选择时段', trigger: 'change' }]
}

// 编辑
const editVisible = ref(false)
const editFormRef = ref(null)
const editForm = reactive({})
const editRules = {
  date: [{ required: true, message: '请选择日期', trigger: 'change' }],
  timeSlot: [{ required: true, message: '请选择时段', trigger: 'change' }]
}

function resetQuery() {
  query.department = ''; query.date = ''
  page.value = 1
  loadData()
}

async function loadData() {
  loading.value = true
  try {
    const res = await getSchedules(query)
    if (res.code === 200) {
      const list = res.data.list || []
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

function openAdd() {
  Object.assign(addForm, { doctorID: '', date: '', timeSlot: '', note: '' })
  addVisible.value = true
}

async function handleAdd() {
  try { await addFormRef.value?.validate() } catch { return }
  addSaving.value = true
  try {
    const res = await createSchedule(addForm)
    if (res.code === 200) { ElMessage.success('创建成功'); addVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '创建失败') }
  finally { addSaving.value = false }
}

function openEdit(row) {
  Object.assign(editForm, { ...row })
  editVisible.value = true
}

async function handleSave() {
  try { await editFormRef.value?.validate() } catch { return }
  saving.value = true
  try {
    const res = await updateSchedule(editForm.scheduleID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败') }
  finally { saving.value = false }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此排班记录？', '确认', { type: 'warning' })
    const res = await deleteSchedule(row.scheduleID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
