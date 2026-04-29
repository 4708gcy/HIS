<template>
  <div class="fade-in">
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="病房类型">
          <el-select v-model="query.wardType" clearable placeholder="全部" style="width: 140px">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="隔离病房" value="隔离病房" />
            <el-option label="VIP病房" value="VIP病房" />
            <el-option label="ICU病房" value="ICU病房" />
          </el-select>
        </el-form-item>
        <el-form-item><el-button type="primary" @click="loadData">搜索</el-button></el-form-item>
      </el-form>
    </el-card>

    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>床位管理</span>
          <el-button type="primary" @click="openCreate">添加床位</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="bedID" label="床位ID" width="120" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="120" />
        <el-table-column prop="areaNumber" label="区号" width="80" />
        <el-table-column prop="wardNumber" label="病房号" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="100" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="bedStatusTagType(row.status)" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="dailyRate" label="日费用">
          <template #default="{ row }">{{ row.dailyRate?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="note" label="备注" min-width="120" show-overflow-tooltip />
        <el-table-column label="操作" width="160" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="openEdit(row)">编辑</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 添加床位对话框 -->
    <el-dialog v-model="createVisible" title="添加床位" width="500px" @close="resetCreateForm">
      <el-form :model="createForm" label-width="100px" :rules="createRules" ref="createFormRef">
        <el-form-item label="科室" prop="department">
          <el-select v-model="createForm.department" placeholder="请选择科室" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="病房类型" prop="wardType">
          <el-select v-model="createForm.wardType" placeholder="请选择病房类型" style="width: 100%">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="隔离病房" value="隔离病房" />
            <el-option label="VIP病房" value="VIP病房" />
            <el-option label="ICU病房" value="ICU病房" />
          </el-select>
        </el-form-item>
        <el-form-item label="区号" prop="areaNumber">
          <el-input-number v-model="createForm.areaNumber" :min="1" :max="99" style="width: 100%" />
        </el-form-item>
        <el-form-item label="病房号" prop="wardNumber">
          <el-input-number v-model="createForm.wardNumber" :min="1" :max="999" style="width: 100%" />
        </el-form-item>
        <el-form-item label="床位号" prop="bedNumber">
          <el-input-number v-model="createForm.bedNumber" :min="1" :max="99" style="width: 100%" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="createVisible = false">取消</el-button>
        <el-button type="primary" :loading="saving" @click="handleCreate">确认添加</el-button>
      </template>
    </el-dialog>

    <!-- 编辑床位对话框 -->
    <el-dialog v-model="editVisible" title="编辑床位" width="500px">
      <el-form :model="editForm" label-width="100px">
        <el-form-item label="床位ID">
          <el-input :model-value="editForm.bedID" disabled />
        </el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status" style="width: 100%">
            <el-option label="已占用" :value="1" />
            <el-option label="清洁中" :value="2" />
            <el-option label="可分配" :value="3" />
            <el-option label="不可用" :value="4" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注">
          <el-input v-model="editForm.note" type="textarea" :rows="2" placeholder="请输入备注" />
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
import { getBeds, createBed, updateBed, deleteBed } from '../../api/nurse'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list })

const loading = ref(false)
const saving = ref(false)
const tableData = ref([])
const query = reactive({ department: '', wardType: '' })

// 添加床位
const createVisible = ref(false)
const createFormRef = ref(null)
const createForm = reactive({
  department: '',
  wardType: '',
  areaNumber: 1,
  wardNumber: 1,
  bedNumber: 1
})
const createRules = {
  department: [{ required: true, message: '请选择科室', trigger: 'change' }],
  wardType: [{ required: true, message: '请选择病房类型', trigger: 'change' }],
  areaNumber: [{ required: true, message: '请输入区号', trigger: 'blur' }],
  wardNumber: [{ required: true, message: '请输入病房号', trigger: 'blur' }],
  bedNumber: [{ required: true, message: '请输入床位号', trigger: 'blur' }]
}

// 编辑床位
const editVisible = ref(false)
const editForm = reactive({
  bedID: '',
  status: 3,
  note: ''
})

function bedStatusTagType(status) {
  switch (status) {
    case 1: return 'danger'
    case 2: return 'warning'
    case 3: return 'success'
    case 4: return 'info'
    default: return 'info'
  }
}

async function loadData() {
  loading.value = true
  try {
    const params = {}
    if (query.department) params.department = query.department
    if (query.wardType) params.wardType = query.wardType
    const res = await getBeds(params)
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '加载失败')
  } finally { loading.value = false }
}

function openCreate() {
  Object.assign(createForm, { department: '', wardType: '', areaNumber: 1, wardNumber: 1, bedNumber: 1 })
  createVisible.value = true
}

function resetCreateForm() {
  createFormRef.value?.resetFields()
}

async function handleCreate() {
  try {
    await createFormRef.value.validate()
  } catch { return }
  saving.value = true
  try {
    const res = await createBed({ ...createForm })
    if (res.code === 200) {
      ElMessage.success('添加成功')
      createVisible.value = false
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '添加失败')
  } finally { saving.value = false }
}

function openEdit(row) {
  Object.assign(editForm, {
    bedID: row.bedID,
    status: row.status,
    note: row.note || ''
  })
  editVisible.value = true
}

async function handleSave() {
  saving.value = true
  try {
    const res = await updateBed(editForm.bedID, {
      status: editForm.status,
      note: editForm.note
    })
    if (res.code === 200) {
      ElMessage.success('保存成功')
      editVisible.value = false
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败')
  } finally { saving.value = false }
}

async function handleDelete(row) {
  if (row.status === 1) {
    ElMessage.warning('该床位已占用，无法删除')
    return
  }
  try {
    await ElMessageBox.confirm(
      `确认删除床位 ${row.bedID}？此操作不可恢复。`,
      '删除确认',
      { confirmButtonText: '删除', cancelButtonText: '取消', type: 'danger' }
    )
  } catch { return }
  try {
    const res = await deleteBed(row.bedID)
    if (res.code === 200) {
      ElMessage.success('删除成功')
      loadData()
    }
  } catch (e) {
    if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '删除失败')
  }
}

onMounted(loadData)
</script>
