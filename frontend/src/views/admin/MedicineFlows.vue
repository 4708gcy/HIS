<template>
  <div class="fade-in">
    <!-- 搜索栏 -->
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="药品">
          <el-select v-model="query.medicineID" filterable clearable placeholder="全部" style="width: 200px">
            <el-option v-for="m in medicineList" :key="m.medicineID" :label="m.name" :value="m.medicineID" />
          </el-select>
        </el-form-item>
        <el-form-item label="类型">
          <el-select v-model="query.type" clearable placeholder="全部" style="width: 120px">
            <el-option label="入库" value="入库" />
            <el-option label="出库" value="出库" />
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
      <template #header>
        <div style="display: flex; justify-content: space-between; align-items: center">
          <span>药品流水记录</span>
        </div>
      </template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="flowID" label="流水ID" width="120" />
        <el-table-column prop="medicineName" label="药品名称" width="150" />
        <el-table-column prop="type" label="类型" width="80">
          <template #default="{ row }">
            <el-tag :type="row.type === '入库' ? 'success' : 'warning'" size="small">{{ row.type }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="quantity" label="数量" width="80" />
        <el-table-column prop="operatorName" label="操作人" width="100" />
        <el-table-column prop="reason" label="原因" min-width="150" />
        <el-table-column prop="time" label="操作时间" width="180" />
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
import { getMedicineFlows, getMedicines } from '../../api/admin'

const medicineList = ref([])
getMedicines().then(res => { medicineList.value = res.data.list || [] }).catch(() => {})

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 20
const query = reactive({ medicineID: '', type: '' })

function resetQuery() {
  query.medicineID = ''; query.type = ''
  page.value = 1
  loadData()
}

async function loadData() {
  loading.value = true
  try {
    const res = await getMedicineFlows(query)
    if (res.code === 200) {
      const list = res.data.list || []
      total.value = list.length
      const start = (page.value - 1) * pageSize
      tableData.value = list.slice(start, start + pageSize)
    }
  } finally { loading.value = false }
}

onMounted(loadData)
</script>
