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
        <el-table-column prop="dailyRate" label="日费用">
          <template #default="{ row }">{{ row.dailyRate?.toFixed(2) }} 元</template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue'
import { getBeds } from '../../api/nurse'
import { getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list })

const loading = ref(false)
const tableData = ref([])
const query = reactive({ department: '' })

async function loadData() {
  loading.value = true
  try {
    const res = await getBeds(query)
    if (res.code === 200) tableData.value = res.data.list
  } finally { loading.value = false }
}

onMounted(loadData)
</script>
