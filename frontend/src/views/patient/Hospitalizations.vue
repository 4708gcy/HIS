<template>
  <div>
    <el-card>
      <template #header><span>我的住院记录</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="hospitalizationID" label="住院ID" width="100" />
        <el-table-column prop="doctorID" label="主治医生" width="100" />
        <el-table-column prop="nurseID" label="责任护士" width="100" />
        <el-table-column prop="wardType" label="病房类型" width="100" />
        <el-table-column prop="bedNumber" label="床位号" width="80" />
        <el-table-column prop="statusStr" label="状态" width="100">
          <template #default="{ row }"><el-tag size="small">{{ row.statusStr }}</el-tag></template>
        </el-table-column>
        <el-table-column prop="admitTime" label="入院时间" width="180" />
        <el-table-column prop="dischargeTime" label="出院时间" min-width="180" />
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { getMyHospitalizations } from '../../api/patient'

const loading = ref(false)
const tableData = ref([])

async function loadData() {
  loading.value = true
  try { const res = await getMyHospitalizations(); if (res.code === 200) tableData.value = res.data.list }
  finally { loading.value = false }
}

onMounted(loadData)
</script>
