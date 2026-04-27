<template>
  <div>
    <el-card>
      <template #header><span>药品库存管理</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="medicineID" label="药品ID" width="100" />
        <el-table-column prop="name" label="药品名称" width="150" />
        <el-table-column prop="specification" label="规格" width="120" />
        <el-table-column prop="stock" label="库存" width="80" />
        <el-table-column prop="safetyStock" label="安全库存" width="90">
          <template #default="{ row }">
            <span :style="{ color: row.stock <= row.safetyStock ? 'red' : '' }">{{ row.safetyStock }}</span>
          </template>
        </el-table-column>
        <el-table-column prop="salePrice" label="售价" width="80">
          <template #default="{ row }">{{ row.salePrice?.toFixed(2) }}</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="80">
          <template #default="{ row }">
            <el-tag :type="row.status === 1 ? 'success' : 'danger'" size="small">{{ row.statusStr }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column label="操作" width="150" fixed="right">
          <template #default="{ row }">
            <el-button type="success" size="small" @click="adjustStock(row, 1)">入库</el-button>
            <el-button type="warning" size="small" @click="adjustStock(row, -1)">出库</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getMedicines, updateMedicineStock } from '../../api/pharmacist'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicines()
    if (res.code === 200) tableData.value = res.data.list
  } catch (e) { if (e !== 'cancel' && e?.message !== 'cancel') ElMessage.error(e.message || '操作失败') }
  finally { loading.value = false }
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

onMounted(loadData)
</script>
