<template>
  <div class="fade-in">
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item><el-button type="primary" @click="loadData">搜索</el-button></el-form-item>
      </el-form>
    </el-card>

    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>床位列表</span>
          <el-button type="success" @click="openAdd">添加床位</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="bedID" label="床位ID" width="120" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="120" />
        <el-table-column prop="bedNumber" label="床位号" width="100" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="row.status === 1 ? 'success' : 'warning'" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="dailyRate" label="日费用" width="100">
          <template #default="{ row }">{{ row.dailyRate?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column label="操作" width="140" fixed="right">
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

    <el-dialog v-model="addVisible" title="添加床位" width="500px">
      <el-form ref="addFormRef" :model="addForm" :rules="addRules" label-width="80px">
        <el-form-item label="科室" prop="department">
          <el-select v-model="addForm.department" clearable placeholder="请选择科室" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="病房类型" prop="wardType">
          <el-select v-model="addForm.wardType" placeholder="请选择病房类型" style="width: 100%">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="隔离病房" value="隔离病房" />
            <el-option label="VIP病房" value="VIP病房" />
            <el-option label="ICU病房" value="ICU病房" />
          </el-select>
        </el-form-item>
        <el-form-item label="区号" prop="areaNumber"><el-input-number v-model="addForm.areaNumber" :min="1" :max="99" /></el-form-item>
        <el-form-item label="病房号" prop="wardNumber"><el-input-number v-model="addForm.wardNumber" :min="1" :max="999" /></el-form-item>
        <el-form-item label="床位号" prop="bedNumber"><el-input-number v-model="addForm.bedNumber" :min="1" :max="99" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="addVisible = false">取消</el-button>
        <el-button type="primary" :loading="addSaving" @click="handleAdd">确定</el-button>
      </template>
    </el-dialog>

    <el-dialog v-model="editVisible" title="编辑床位" width="500px">
      <el-form ref="editFormRef" :model="editForm" :rules="addRules" label-width="80px">
        <el-form-item label="科室" prop="department">
          <el-select v-model="editForm.department" clearable placeholder="请选择科室" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="病房类型" prop="wardType">
          <el-select v-model="editForm.wardType" placeholder="请选择病房类型" style="width: 100%">
            <el-option label="普通病房" value="普通病房" />
            <el-option label="隔离病房" value="隔离病房" />
            <el-option label="VIP病房" value="VIP病房" />
            <el-option label="ICU病房" value="ICU病房" />
          </el-select>
        </el-form-item>
        <el-form-item label="区号" prop="areaNumber"><el-input-number v-model="editForm.areaNumber" :min="1" :max="99" /></el-form-item>
        <el-form-item label="病房号" prop="wardNumber"><el-input-number v-model="editForm.wardNumber" :min="1" :max="999" /></el-form-item>
        <el-form-item label="床位号" prop="bedNumber"><el-input-number v-model="editForm.bedNumber" :min="1" :max="99" /></el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status" style="width: 100%">
            <el-option label="可用" :value="3" />
            <el-option label="已占用" :value="1" />
            <el-option label="清洁中" :value="2" />
            <el-option label="不可用" :value="4" />
          </el-select>
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="editVisible = false">取消</el-button>
        <el-button type="primary" :loading="editSaving" @click="handleEdit">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getBeds, createBed, updateBed, deleteBed } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const addSaving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '' })

// -- 添加床位 --
const addFormRef = ref(null)
const addVisible = ref(false)
const addForm = reactive({
  department: '',
  wardType: '',
  areaNumber: 1,
  wardNumber: 1,
  bedNumber: 1
})

// -- 编辑床位 --
const editFormRef = ref(null)
const editVisible = ref(false)
const editSaving = ref(false)
const editForm = reactive({
  bedID: '',
  department: '',
  wardType: '',
  areaNumber: 1,
  wardNumber: 1,
  bedNumber: 1,
  status: 3
})
const addRules = {
  department: [{ required: true, message: '请选择科室', trigger: 'change' }],
  wardType: [{ required: true, message: '请选择病房类型', trigger: 'change' }],
  areaNumber: [{ required: true, message: '请输入区号', trigger: 'blur' }],
  wardNumber: [{ required: true, message: '请输入病房号', trigger: 'blur' }],
  bedNumber: [{ required: true, message: '请输入床位号', trigger: 'blur' }]
}

function openAdd() {
  Object.assign(addForm, { department: '', wardType: '', areaNumber: 1, wardNumber: 1, bedNumber: 1 })
  addVisible.value = true
}

function openEdit(row) {
  Object.assign(editForm, {
    bedID: row.bedID,
    department: row.department || '',
    wardType: row.wardType || '',
    areaNumber: row.areaNumber || 1,
    wardNumber: row.wardNumber || 1,
    bedNumber: row.bedNumber || 1,
    status: row.status || 3
  })
  editVisible.value = true
}

async function handleEdit() {
  try {
    await editFormRef.value?.validate()
  } catch { return }
  editSaving.value = true
  try {
    const { bedID, ...data } = editForm
    const res = await updateBed(bedID, data)
    if (res.code === 200) { ElMessage.success('编辑成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '编辑失败') }
  finally { editSaving.value = false }
}

async function handleAdd() {
  try {
    await addFormRef.value?.validate()
  } catch { return }
  addSaving.value = true
  try {
    const res = await createBed(addForm)
    if (res.code === 200) { ElMessage.success('添加成功'); addVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '添加失败') }
  finally { addSaving.value = false }
}

// -- 床位列表 --
async function loadData() {
  loading.value = true
  try {
    const res = await getBeds(query)
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
    await ElMessageBox.confirm('确定删除此床位？', '确认', { type: 'warning' })
    const res = await deleteBed(row.bedID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
