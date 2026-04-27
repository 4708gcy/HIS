<template>
  <div>
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item><el-button type="primary" @click="loadData">搜索</el-button><el-button @click="query.department = ''; loadData()">重置</el-button></el-form-item>
      </el-form>
    </el-card>

    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>药品列表</span>
          <el-button type="success" @click="openAdd">添加药品</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="medicineID" label="药品ID" width="100" />
        <el-table-column prop="name" label="药品名称" width="150" />
        <el-table-column prop="specification" label="规格" width="120" />
        <el-table-column prop="manufacturer" label="生产商" width="150" />
        <el-table-column prop="purchasePrice" label="进价" width="80">
          <template #default="{ row }">{{ row.purchasePrice?.toFixed(2) }}</template>
        </el-table-column>
        <el-table-column prop="salePrice" label="售价" width="80">
          <template #default="{ row }">{{ row.salePrice?.toFixed(2) }}</template>
        </el-table-column>
        <el-table-column prop="stock" label="库存" width="80" />
        <el-table-column prop="safetyStock" label="安全库存" width="90" />
        <el-table-column prop="statusStr" label="状态" width="80">
          <template #default="{ row }">
            <el-tag :type="row.status === 1 ? 'success' : 'danger'" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="120" fixed="right">
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

    <el-dialog v-model="addVisible" title="添加药品" width="500px">
      <el-form :model="addForm" label-width="80px" :rules="addRules" ref="addFormRef">
        <el-form-item label="名称" prop="name"><el-input v-model="addForm.name" placeholder="请输入药品名称" /></el-form-item>
        <el-form-item label="规格"><el-input v-model="addForm.specification" placeholder="请输入规格" /></el-form-item>
        <el-form-item label="生产商"><el-input v-model="addForm.manufacturer" placeholder="请输入生产商" /></el-form-item>
        <el-form-item label="进价"><el-input-number v-model="addForm.purchasePrice" :precision="2" :min="0" /></el-form-item>
        <el-form-item label="售价"><el-input-number v-model="addForm.salePrice" :precision="2" :min="0" /></el-form-item>
        <el-form-item label="库存"><el-input-number v-model="addForm.stock" :min="0" /></el-form-item>
        <el-form-item label="安全库存"><el-input-number v-model="addForm.safetyStock" :min="0" /></el-form-item>
        <el-form-item label="生产日期"><el-date-picker v-model="addForm.productionDate" type="date" value-format="YYYY-MM-DD" placeholder="选择生产日期" style="width: 100%" /></el-form-item>
        <el-form-item label="有效期至"><el-date-picker v-model="addForm.expiryDate" type="date" value-format="YYYY-MM-DD" placeholder="选择有效期" style="width: 100%" /></el-form-item>
        <el-form-item label="科室">
          <el-select v-model="addForm.department" clearable placeholder="请选择科室" style="width: 100%">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注"><el-input v-model="addForm.note" type="textarea" :rows="3" placeholder="请输入备注" /></el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="addVisible = false">取消</el-button>
        <el-button type="primary" :loading="addSaving" @click="handleAdd">确定</el-button>
      </template>
    </el-dialog>

    <el-dialog v-model="editVisible" title="编辑药品" width="500px">
      <el-form :model="editForm" label-width="80px">
        <el-form-item label="名称"><el-input v-model="editForm.name" /></el-form-item>
        <el-form-item label="规格"><el-input v-model="editForm.specification" /></el-form-item>
        <el-form-item label="生产商"><el-input v-model="editForm.manufacturer" /></el-form-item>
        <el-form-item label="进价"><el-input-number v-model="editForm.purchasePrice" :precision="2" :min="0" /></el-form-item>
        <el-form-item label="售价"><el-input-number v-model="editForm.salePrice" :precision="2" :min="0" /></el-form-item>
        <el-form-item label="库存"><el-input-number v-model="editForm.stock" :min="0" /></el-form-item>
        <el-form-item label="安全库存"><el-input-number v-model="editForm.safetyStock" :min="0" /></el-form-item>
        <el-form-item label="状态">
          <el-select v-model="editForm.status"><el-option label="正常" :value="1" /><el-option label="停用" :value="2" /></el-select>
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
import { getMedicines, createMedicine, updateMedicine, deleteMedicine } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const saving = ref(false)
const addSaving = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '' })
const editVisible = ref(false)
const editForm = reactive({})

// -- 添加药品 --
const addVisible = ref(false)
const addFormRef = ref(null)
const addForm = reactive({
  name: '',
  specification: '',
  manufacturer: '',
  purchasePrice: 0,
  salePrice: 0,
  stock: 0,
  safetyStock: 0,
  productionDate: '',
  expiryDate: '',
  department: '',
  note: ''
})
const addRules = {
  name: [{ required: true, message: '请输入药品名称', trigger: 'blur' }]
}

function openAdd() {
  Object.assign(addForm, { name: '', specification: '', manufacturer: '', purchasePrice: 0, salePrice: 0, stock: 0, safetyStock: 0, productionDate: '', expiryDate: '', department: '', note: '' })
  addVisible.value = true
}

async function handleAdd() {
  if (addFormRef.value) {
    try { await addFormRef.value.validate() } catch { return }
  }
  addSaving.value = true
  try {
    const res = await createMedicine(addForm)
    if (res.code === 200) { ElMessage.success('添加成功'); addVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '添加失败') }
  finally { addSaving.value = false }
}

// -- 编辑药品 --
async function loadData() {
  loading.value = true
  try {
    const res = await getMedicines(query)
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
    const res = await updateMedicine(editForm.medicineID, editForm)
    if (res.code === 200) { ElMessage.success('保存成功'); editVisible.value = false; loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '保存失败') }
  finally { saving.value = false }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定删除此药品？', '确认', { type: 'warning' })
    const res = await deleteMedicine(row.medicineID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(loadData)
</script>
