<template>
  <div class="fade-in">
    <!-- 概览卡片 -->
    <el-row :gutter="16" style="margin-bottom: 16px">
      <el-col :xs="12" :sm="8" :md="4" v-for="item in overviewCards" :key="item.title">
        <el-card shadow="hover" class="overview-card">
          <div class="overview-value">{{ item.value }}</div>
          <div class="overview-label">{{ item.title }}</div>
        </el-card>
      </el-col>
    </el-row>

    <!-- 科室统计 -->
    <el-card style="margin-bottom: 16px" v-loading="loading.dept">
      <template #header><span>科室统计</span></template>
      <el-table :data="deptData" stripe>
        <el-table-column prop="department" label="科室" width="120" />
        <el-table-column prop="doctorCount" label="医生数" width="100" />
        <el-table-column prop="patientCount" label="患者数" width="100" />
        <el-table-column prop="registrationCount" label="挂号数" width="100" />
        <el-table-column prop="consultationCount" label="看诊数" width="100" />
        <el-table-column prop="examinationCount" label="检查数" width="100" />
        <el-table-column prop="revenue" label="收入(元)" min-width="120">
          <template #default="{ row }">{{ row.revenue?.toFixed(2) ?? '-' }}</template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 医生工作量 -->
    <el-card style="margin-bottom: 16px" v-loading="loading.doctor">
      <template #header><span>医生工作量统计</span></template>
      <el-table :data="doctorData" stripe>
        <el-table-column prop="doctorID" label="医生ID" width="100" />
        <el-table-column prop="doctorName" label="姓名" width="100" />
        <el-table-column prop="department" label="科室" width="100" />
        <el-table-column prop="registrationCount" label="接诊数" width="100" />
        <el-table-column prop="consultationCount" label="看诊数" width="100" />
        <el-table-column prop="examinationCount" label="检查数" width="100" />
        <el-table-column prop="avgConsultationTime" label="平均看诊时长" min-width="130">
          <template #default="{ row }">{{ row.avgConsultationTime ?? '-' }} 分钟</template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 床位利用率 -->
    <el-card style="margin-bottom: 16px" v-loading="loading.bed">
      <template #header><span>床位利用率</span></template>
      <el-table :data="bedData" stripe>
        <el-table-column prop="wardType" label="病房类型" width="120" />
        <el-table-column prop="totalBeds" label="总床位" width="100" />
        <el-table-column prop="occupiedBeds" label="已占用" width="100" />
        <el-table-column prop="availableBeds" label="空闲" width="100" />
        <el-table-column prop="utilizationRate" label="利用率" min-width="120">
          <template #default="{ row }">
            <el-progress :percentage="Math.round((row.utilizationRate || 0) * 100)" :stroke-width="14" :text-inside="true" />
          </template>
        </el-table-column>
      </el-table>
    </el-card>

    <!-- 药品库存 -->
    <el-card v-loading="loading.medicine">
      <template #header><span>药品库存统计</span></template>
      <el-table :data="medicineData" stripe>
        <el-table-column prop="medicineID" label="药品ID" width="100" />
        <el-table-column prop="name" label="药品名称" width="150" />
        <el-table-column prop="stock" label="当前库存" width="100" />
        <el-table-column prop="safetyStock" label="安全库存" width="100" />
        <el-table-column prop="status" label="状态" width="100">
          <template #default="{ row }">
            <el-tag :type="row.stock > row.safetyStock ? 'success' : row.stock > 0 ? 'warning' : 'danger'" size="small">
              {{ row.stock > row.safetyStock ? '充足' : row.stock > 0 ? '偏低' : '缺货' }}
            </el-tag>
          </template>
        </el-table-column>
        <el-table-column prop="purchasePrice" label="进价" width="90">
          <template #default="{ row }">{{ row.purchasePrice?.toFixed(2) ?? '-' }}</template>
        </el-table-column>
        <el-table-column prop="salePrice" label="售价" width="90">
          <template #default="{ row }">{{ row.salePrice?.toFixed(2) ?? '-' }}</template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, reactive, computed, onMounted } from 'vue'
import {
  getOverviewReport, getDepartmentReport, getDoctorWorkloadReport,
  getBedUtilizationReport, getMedicineInventoryReport
} from '../../api/admin'

const loading = reactive({ dept: false, doctor: false, bed: false, medicine: false })
const overviewData = ref({})
const deptData = ref([])
const doctorData = ref([])
const bedData = ref([])
const medicineData = ref([])

const overviewCards = computed(() => [
  { title: '总医生数', value: overviewData.value.totalDoctors ?? '-' },
  { title: '总患者数', value: overviewData.value.totalPatients ?? '-' },
  { title: '总挂号数', value: overviewData.value.totalRegistrations ?? '-' },
  { title: '总看诊数', value: overviewData.value.totalConsultations ?? '-' },
  { title: '总住院数', value: overviewData.value.totalHospitalizations ?? '-' },
  { title: '总药品数', value: overviewData.value.totalMedicines ?? '-' }
])

onMounted(async () => {
  // Overview
  try {
    const res = await getOverviewReport()
    if (res.code === 200) overviewData.value = res.data || {}
  } catch (e) { console.error('概览数据加载失败', e) }

  // Department
  loading.dept = true
  try {
    const res = await getDepartmentReport()
    if (res.code === 200) deptData.value = res.data.list || res.data || []
  } catch (e) { console.error('科室统计加载失败', e) }
  finally { loading.dept = false }

  // Doctor workload
  loading.doctor = true
  try {
    const res = await getDoctorWorkloadReport()
    if (res.code === 200) doctorData.value = res.data.list || res.data || []
  } catch (e) { console.error('医生工作量加载失败', e) }
  finally { loading.doctor = false }

  // Bed utilization
  loading.bed = true
  try {
    const res = await getBedUtilizationReport()
    if (res.code === 200) bedData.value = res.data.list || res.data || []
  } catch (e) { console.error('床位利用率加载失败', e) }
  finally { loading.bed = false }

  // Medicine inventory
  loading.medicine = true
  try {
    const res = await getMedicineInventoryReport()
    if (res.code === 200) medicineData.value = res.data.list || res.data || []
  } catch (e) { console.error('药品库存加载失败', e) }
  finally { loading.medicine = false }
})
</script>

<style scoped>
.overview-card {
  text-align: center;
  margin-bottom: 8px;
}

.overview-value {
  font-size: 28px;
  font-weight: 700;
  color: var(--his-primary);
  line-height: 1.2;
}

.overview-label {
  font-size: 13px;
  color: var(--his-text-secondary);
  margin-top: 6px;
}
</style>
