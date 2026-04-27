<template>
  <div>
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
        <el-table-column label="操作" width="80" fixed="right">
          <template #default="{ row }">
            <el-button type="danger" size="small" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>
      <div style="margin-top: 16px; display: flex; justify-content: space-between; align-items: center">
        <span>共 {{ total }} 条</span>
        <el-pagination v-model:current-page="page" :page-size="pageSize" :total="total" layout="prev, pager, next" @current-change="loadData" />
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { ElMessage, ElMessageBox } from 'element-plus'
import { getBeds, deleteBed } from '../../api/admin'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '' })

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
