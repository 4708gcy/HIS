<template>
  <div>
    <el-card>
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center;">
          <span>药品库存管理</span>
          <el-button type="primary" size="small" @click="showCreateDialog">添加药品</el-button>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="medicineID" label="药品ID" width="100" />
        <el-table-column prop="name" label="药品名称" width="150" />
        <el-table-column prop="specification" label="规格" width="120" />
        <el-table-column prop="manufacturer" label="生产厂家" width="150" />
        <el-table-column prop="stock" label="库存" width="80" />
        <el-table-column prop="safetyStock" label="安全库存" width="90">
          <template #default="{ row }">
            <span :style="{ color: row.stock <= row.safetyStock ? 'red' : '' }">{{ row.safetyStock }}</span>
          </template>
        </el-table-column>
        <el-table-column prop="purchasePrice" label="进价" width="80">
          <template #default="{ row }">{{ row.purchasePrice?.toFixed(2) }}</template>
        </el-table-column>
        <el-table-column prop="salePrice" label="售价" width="80">
          <template #default="{ row }">{{ row.salePrice?.toFixed(2) }}</template>
        </el-table-column>
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="statusStr" label="状态" width="80">
          <template #default="{ row }">
            <el-tag :type="row.status === 1 ? 'success' : 'danger'" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="260" fixed="right">
          <template #default="{ row }">
            <el-button type="primary" size="small" @click="showEditDialog(row)">编辑</el-button>
            <el-button type="success" size="small" @click="adjustStock(row, 1)">入库</el-button>
            <el-button type="warning" size="small" @click="adjustStock(row, -1)">出库</el-button>
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- Create/Edit Dialog -->
    <el-dialog v-model="dialogVisible" :title="isEdit ? '编辑药品' : '添加药品'" width="550px" destroy-on-close>
      <el-form :model="formData" label-width="100px">
        <el-form-item label="药品名称" required>
          <el-input v-model="formData.name" placeholder="请输入药品名称" />
        </el-form-item>
        <el-form-item label="规格">
          <el-input v-model="formData.specification" placeholder="请输入规格" />
        </el-form-item>
        <el-form-item label="生产厂家">
          <el-input v-model="formData.manufacturer" placeholder="请输入生产厂家" />
        </el-form-item>
        <el-form-item label="进价">
          <el-input-number v-model="formData.purchasePrice" :precision="2" :min="0" :step="0.1" />
        </el-form-item>
        <el-form-item label="售价">
          <el-input-number v-model="formData.salePrice" :precision="2" :min="0" :step="0.1" />
        </el-form-item>
        <el-form-item label="库存" v-if="!isEdit">
          <el-input-number v-model="formData.stock" :min="0" :step="1" />
        </el-form-item>
        <el-form-item label="安全库存">
          <el-input-number v-model="formData.safetyStock" :min="0" :step="1" />
        </el-form-item>
        <el-form-item label="生产日期">
          <el-date-picker v-model="formData.productionDate" type="date" placeholder="选择日期" value-format="YYYY-MM-DD" style="width: 100%;" />
        </el-form-item>
        <el-form-item label="有效期至">
          <el-date-picker v-model="formData.expiryDate" type="date" placeholder="选择日期" value-format="YYYY-MM-DD" style="width: 100%;" />
        </el-form-item>
        <el-form-item label="科室">
          <el-select v-model="formData.department" placeholder="请选择科室" clearable style="width: 100%;">
            <el-option v-for="dept in departments" :key="dept" :label="dept" :value="dept" />
          </el-select>
        </el-form-item>
        <el-form-item label="备注">
          <el-input v-model="formData.note" type="textarea" :rows="3" placeholder="请输入备注" />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" :loading="submitLoading" @click="handleSubmit">确定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMedicines, createMedicine, updateMedicine, deleteMedicine, updateMedicineStock } from '../../api/pharmacist'
import { getDepartments } from '../../api/common'

const loading = ref(false)
const tableData = ref([])
const departments = ref([])

// Dialog state
const dialogVisible = ref(false)
const isEdit = ref(false)
const submitLoading = ref(false)
const editingId = ref(null)
const formData = ref(getEmptyForm())

function getEmptyForm() {
  return {
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
  }
}

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicines()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
}

async function loadDepartments() {
  try {
    const res = await getDepartments()
    if (res.code === 200) departments.value = res.data?.list ?? res.data ?? []
  } catch (e) { console.error('科室列表加载失败:', e) }
}

function showCreateDialog() {
  isEdit.value = false
  editingId.value = null
  formData.value = getEmptyForm()
  dialogVisible.value = true
}

function showEditDialog(row) {
  isEdit.value = true
  editingId.value = row.medicineID
  formData.value = {
    name: row.name || '',
    specification: row.specification || '',
    manufacturer: row.manufacturer || '',
    purchasePrice: row.purchasePrice ?? 0,
    salePrice: row.salePrice ?? 0,
    stock: row.stock ?? 0,
    safetyStock: row.safetyStock ?? 0,
    productionDate: row.productionDate || '',
    expiryDate: row.expiryDate || '',
    department: row.department || '',
    note: row.note || ''
  }
  dialogVisible.value = true
}

async function handleSubmit() {
  if (!formData.value.name) {
    ElMessage.warning('请输入药品名称')
    return
  }
  submitLoading.value = true
  try {
    let res
    if (isEdit.value) {
      res = await updateMedicine(editingId.value, formData.value)
    } else {
      res = await createMedicine(formData.value)
    }
    if (res.code === 200) {
      ElMessage.success(isEdit.value ? '编辑成功' : '添加成功')
      dialogVisible.value = false
      loadData()
    } else {
      ElMessage.error(res.message)
    }
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { submitLoading.value = false }
}

async function adjustStock(row, direction) {
  try {
    const label = direction > 0 ? '入库数量' : '出库数量'
    const { value } = await ElMessageBox.prompt(`请输入${label}`, '库存调整', {
      inputPattern: /^\d+$/, inputErrorMessage: '请输入正整数'
    })
    const delta = parseInt(value) * direction
    const res = await updateMedicineStock(row.medicineID, delta)
    if (res.code === 200) { ElMessage.success('调整成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

async function handleDelete(row) {
  try {
    await ElMessageBox.confirm('确定要删除该药品吗？', '确认删除', {
      confirmButtonText: '确定',
      cancelButtonText: '取消',
      type: 'warning'
    })
    const res = await deleteMedicine(row.medicineID)
    if (res.code === 200) { ElMessage.success('删除成功'); loadData() }
    else ElMessage.error(res.message)
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
}

onMounted(() => {
  loadData()
  loadDepartments()
})
</script>
