<template>
  <div>
    <el-row :gutter="20" style="margin-bottom: 20px">
      <el-col :span="6">
        <el-statistic title="医生总数" :value="stats.doctors" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="护士总数" :value="stats.nurses" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="患者总数" :value="stats.patients" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="挂号记录" :value="stats.registrations" />
      </el-col>
    </el-row>
    <el-row :gutter="20">
      <el-col :span="6">
        <el-statistic title="看诊记录" :value="stats.consultations" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="检查记录" :value="stats.examinations" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="住院记录" :value="stats.hospitalizations" />
      </el-col>
      <el-col :span="6">
        <el-statistic title="药品总数" :value="stats.medicines" />
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { reactive, onMounted } from 'vue'
import { getDoctors, getNurses, getPatients, getRegistrations, getConsultations, getExaminations, getHospitalizations, getMedicines } from '../../api/admin'

const stats = reactive({
  doctors: 0, nurses: 0, patients: 0, registrations: 0,
  consultations: 0, examinations: 0, hospitalizations: 0, medicines: 0
})

onMounted(async () => {
  try {
    const [d, n, p, r, c, e, h, m] = await Promise.all([
      getDoctors(), getNurses(), getPatients(), getRegistrations(),
      getConsultations(), getExaminations(), getHospitalizations(), getMedicines()
    ])
    stats.doctors = d.data.total
    stats.nurses = n.data.total
    stats.patients = p.data.total
    stats.registrations = r.data.total
    stats.consultations = c.data.total
    stats.examinations = e.data.total
    stats.hospitalizations = h.data.total
    stats.medicines = m.data.total
  } catch (e) { /* ignore */ }
})
</script>
