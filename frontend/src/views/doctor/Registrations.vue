<template>
  <div>
    <el-card>
      <template #header><span>我的挂号列表</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="registrationID" label="挂号ID" width="100" />
        <el-table-column prop="patientID" label="患者ID" width="100" />
        <el-table-column prop="department" label="科室" width="80" />
        <el-table-column prop="fee" label="挂号费" width="80">
          <template #default="{ row }">{{ row.fee?.toFixed(2) }} 元</template>
        </el-table-column>
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag :type="['info','success','warning','danger'][row.status]" size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="registerTime" label="挂号时间" min-width="180" />
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { getMyRegistrations } from '../../api/doctor'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try {
    const res = await getMyRegistrations()
    if (res.code === 200) tableData.value = res.data.list
  } finally { loading.value = false }
}

onMounted(loadData)
</script>
