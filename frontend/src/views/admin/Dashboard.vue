<template>
  <div class="dashboard">
    <div class="page-title fade-in">
      <h2>首页概览</h2>
      <p>系统数据总览</p>
    </div>

    <el-row :gutter="16" class="stats-grid fade-in">
      <el-col :xs="12" :sm="8" :md="6" v-for="item in statItems" :key="item.title">
        <div class="stat-card" :style="{ '--accent': item.color }">
          <div class="stat-icon">
            <el-icon :size="24"><component :is="item.icon" /></el-icon>
          </div>
          <div class="stat-info">
          <div class="stat-value">{{ item.value }}</div>
          <div class="stat-label">{{ item.title }}</div>
        </div>
        </div>
      </el-col>
    </el-row>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { getDoctors, getNurses, getPatients, getRegistrations, getConsultations, getExaminations, getHospitalizations, getMedicines } from '../../api/admin'

const raw = ref({
  doctors: 0, nurses: 0, patients: 0, registrations: 0,
  consultations: 0, examinations: 0, hospitalizations: 0, medicines: 0
})

const statItems = computed(() => [
  { title: '医生总数', value: raw.value.doctors, icon: 'User', color: '#1e88e5' },
  { title: '护士总数', value: raw.value.nurses, icon: 'UserFilled', color: '#00897b' },
  { title: '患者总数', value: raw.value.patients, icon: 'Avatar', color: '#f57c00' },
  { title: '挂号记录', value: raw.value.registrations, icon: 'Calendar', color: '#7b1fa2' },
  { title: '看诊记录', value: raw.value.consultations, icon: 'ChatDotRound', color: '#1e88e5' },
  { title: '检查记录', value: raw.value.examinations, icon: 'Monitor', color: '#00897b' },
  { title: '住院记录', value: raw.value.hospitalizations, icon: 'House', color: '#f57c00' },
  { title: '药品总数', value: raw.value.medicines, icon: 'FirstAidKit', color: '#c62828' },
])

onMounted(async () => {
  try {
    const [d, n, p, r, c, e, h, m] = await Promise.all([
      getDoctors(), getNurses(), getPatients(), getRegistrations(),
      getConsultations(), getExaminations(), getHospitalizations(), getMedicines()
    ])
    raw.value.doctors = d.data?.total ?? d.data?.list?.length ?? 0
    raw.value.nurses = n.data?.total ?? n.data?.list?.length ?? 0
    raw.value.patients = p.data?.total ?? p.data?.list?.length ?? 0
    raw.value.registrations = r.data?.total ?? r.data?.list?.length ?? 0
    raw.value.consultations = c.data?.total ?? c.data?.list?.length ?? 0
    raw.value.examinations = e.data?.total ?? e.data?.list?.length ?? 0
    raw.value.hospitalizations = h.data?.total ?? h.data?.list?.length ?? 0
    raw.value.medicines = m.data?.total ?? m.data?.list?.length ?? 0
  } catch (e) { console.error('Dashboard 数据加载失败:', e) }
})
</script>

<style scoped>
.dashboard {
  padding: 4px 0;
}

.page-title {
  margin-bottom: 24px;
}

.page-title h2 {
  font-size: 22px;
  font-weight: 600;
  color: var(--his-text);
  margin: 0;
}

.page-title h2::after {
  content: '';
  display: block;
  width: 40px;
  height: 3px;
  background: var(--his-primary);
  border-radius: 2px;
  margin-top: 8px;
}

.page-title p {
  color: var(--his-text-muted);
  margin: 6px 0 0;
  font-size: 14px;
}

.stats-grid {
  margin-bottom: 8px;
}

.stat-card {
  background: var(--his-surface);
  border: 1px solid var(--his-border);
  border-radius: 12px;
  padding: 20px;
  display: flex;
  align-items: center;
  gap: 16px;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  cursor: default;
}

.stat-card:hover {
  transform: translateY(-2px);
  box-shadow: var(--his-shadow-md);
  border-color: var(--accent);
}

.stat-icon {
  width: 48px;
  height: 48px;
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: color-mix(in srgb, var(--accent) 10%, transparent);
  color: var(--accent);
  flex-shrink: 0;
}

.stat-info {
  flex: 1;
  min-width: 0;
}

.stat-value {
  font-size: 26px;
  font-weight: 700;
  color: var(--his-text);
  line-height: 1.1;
}

.stat-label {
  font-size: 13px;
  color: var(--his-text-secondary);
  margin-top: 4px;
}
</style>
