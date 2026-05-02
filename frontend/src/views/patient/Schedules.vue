<template>
  <div class="fade-in">
    <!-- 搜索栏 -->
    <el-card style="margin-bottom: 16px">
      <el-form :inline="true" :model="query">
        <el-form-item label="科室">
          <el-select v-model="query.department" clearable placeholder="全部" style="width: 140px">
            <el-option v-for="d in departments" :key="d" :label="d" :value="d" />
          </el-select>
        </el-form-item>
        <el-form-item label="日期">
          <el-date-picker v-model="query.date" type="date" value-format="YYYY-MM-DD" placeholder="选择日期" clearable style="width: 160px" />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="loadData">搜索</el-button>
          <el-button @click="resetQuery">重置</el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <!-- 排班表 -->
    <el-card>
      <template #header><span>医生排班表</span></template>
      <el-table :data="tableData" stripe v-loading="loading">
        <el-table-column prop="doctorName" label="医生姓名" width="120" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="date" label="日期" width="120" />
        <el-table-column prop="timeSlot" label="时段" width="100">
          <template #default="{ row }">
            <el-tag :type="row.timeSlot === '上午' ? 'success' : row.timeSlot === '下午' ? 'warning' : 'primary'" size="small">{{ row.timeSlot }}</el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="note" label="备注" min-width="150" />
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
import { getSchedules, getDepartments } from '../../api/common'

const departments = ref([])
getDepartments().then(res => { departments.value = res.data.list }).catch(e => { if (e !== 'cancel' && e?.message !== 'cancel') console.error('获取科室失败', e) })

const loading = ref(false)
const tableData = ref([])
const total = ref(0)
const page = ref(1)
const pageSize = 15
const query = reactive({ department: '', date: '' })

function resetQuery() {
  query.department = ''; query.date = ''
  page.value = 1
  loadData()
}

async function loadData() {
  loading.value = true
  try {
    const res = await getSchedules(query)
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
