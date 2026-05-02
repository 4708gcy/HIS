import request from './index'

// 医生管理
export function getDoctors(params) {
  return request.get('/admin/doctors', { params })
}
export function getDoctor(id) {
  return request.get(`/admin/doctors/${id}`)
}
export function createDoctor(data) {
  return request.post('/admin/doctors', data)
}
export function updateDoctor(id, data) {
  return request.put(`/admin/doctors/${id}`, data)
}
export function deleteDoctor(id) {
  return request.delete(`/admin/doctors/${id}`)
}

// 护士管理
export function getNurses(params) {
  return request.get('/admin/nurses', { params })
}
export function getNurse(id) {
  return request.get(`/admin/nurses/${id}`)
}
export function createNurse(data) {
  return request.post('/admin/nurses', data)
}
export function updateNurse(id, data) {
  return request.put(`/admin/nurses/${id}`, data)
}
export function deleteNurse(id) {
  return request.delete(`/admin/nurses/${id}`)
}

// 药剂师管理
export function getPharmacists(params) {
  return request.get('/admin/pharmacists', { params })
}
export function getPharmacist(id) {
  return request.get(`/admin/pharmacists/${id}`)
}
export function createPharmacist(data) {
  return request.post('/admin/pharmacists', data)
}
export function updatePharmacist(id, data) {
  return request.put(`/admin/pharmacists/${id}`, data)
}
export function deletePharmacist(id) {
  return request.delete(`/admin/pharmacists/${id}`)
}

// 患者管理
export function getPatients(params) {
  return request.get('/admin/patients', { params })
}
export function getPatient(id) {
  return request.get(`/admin/patients/${id}`)
}
export function createPatient(data) {
  return request.post('/admin/patients', data)
}
export function updatePatient(id, data) {
  return request.put(`/admin/patients/${id}`, data)
}
export function deletePatient(id) {
  return request.delete(`/admin/patients/${id}`)
}

// 管理员列表
export function getAdmins(params) {
  return request.get('/admin/admins', { params })
}

// 挂号记录
export function getRegistrations(params) {
  return request.get('/admin/registrations', { params })
}
export function getRegistration(id) {
  return request.get(`/admin/registrations/${id}`)
}
export function createRegistration(data) {
  return request.post('/admin/registrations', data)
}
export function updateRegistrationStatus(id, status) {
  return request.put(`/admin/registrations/${id}/status`, { status })
}
export function deleteRegistration(id) {
  return request.delete(`/admin/registrations/${id}`)
}

// 看诊记录
export function getConsultations(params) {
  return request.get('/admin/consultations', { params })
}
export function getConsultation(id) {
  return request.get(`/admin/consultations/${id}`)
}
export function createConsultation(data) {
  return request.post('/admin/consultations', data)
}
export function updateConsultationStatus(id, status) {
  return request.put(`/admin/consultations/${id}/status`, { status })
}
export function deleteConsultation(id) {
  return request.delete(`/admin/consultations/${id}`)
}

// 检查记录
export function getExaminations(params) {
  return request.get('/admin/examinations', { params })
}
export function getExamination(id) {
  return request.get(`/admin/examinations/${id}`)
}
export function createExamination(data) {
  return request.post('/admin/examinations', data)
}
export function deleteExamination(id) {
  return request.delete(`/admin/examinations/${id}`)
}

// 住院记录
export function getHospitalizations(params) {
  return request.get('/admin/hospitalizations', { params })
}
export function getHospitalization(id) {
  return request.get(`/admin/hospitalizations/${id}`)
}
export function createHospitalization(data) {
  return request.post('/admin/hospitalizations', data)
}
export function deleteHospitalization(id) {
  return request.delete(`/admin/hospitalizations/${id}`)
}

// 用药记录
export function getMedicationRecords(params) {
  return request.get('/admin/medication-records', { params })
}
export function createMedicationRecord(data) {
  return request.post('/admin/medication-records', data)
}
export function deleteMedicationRecord(id) {
  return request.delete(`/admin/medication-records/${id}`)
}

// 药品管理
export function getMedicines(params) {
  return request.get('/admin/medicines', { params })
}
export function getMedicine(id) {
  return request.get(`/admin/medicines/${id}`)
}
export function createMedicine(data) {
  return request.post('/admin/medicines', data)
}
export function updateMedicine(id, data) {
  return request.put(`/admin/medicines/${id}`, data)
}
export function deleteMedicine(id) {
  return request.delete(`/admin/medicines/${id}`)
}

// 床位管理
export function getBeds(params) {
  return request.get('/admin/beds', { params })
}
export function createBed(data) {
  return request.post('/admin/beds', data)
}
export function deleteBed(id) {
  return request.delete(`/admin/beds/${id}`)
}

// 账号管理
export function updateAccountStatus(id, role, active) {
  return request.put(`/admin/account/${id}/status`, { role, active })
}

// 管理员个人信息
export function updateAdminProfile(data) {
  return request.put('/admin/profile', data)
}

// 床位编辑
export function updateBed(id, data) {
  return request.put(`/admin/beds/${id}`, data)
}

// 挂号记录完整编辑
export function updateRegistration(id, data) {
  return request.put(`/admin/registrations/${id}`, data)
}

// 看诊记录完整编辑
export function updateConsultation(id, data) {
  return request.put(`/admin/consultations/${id}`, data)
}

// 检查记录编辑
export function updateExamination(id, data) {
  return request.put(`/admin/examinations/${id}`, data)
}

// 排班管理
export function getSchedules(params) {
  return request.get('/admin/schedules', { params })
}
export function createSchedule(data) {
  return request.post('/admin/schedules', data)
}
export function updateSchedule(id, data) {
  return request.put(`/admin/schedules/${id}`, data)
}
export function deleteSchedule(id) {
  return request.delete(`/admin/schedules/${id}`)
}

// 统计报表
export function getDepartmentReport() {
  return request.get('/admin/reports/department')
}
export function getDoctorWorkloadReport() {
  return request.get('/admin/reports/doctor-workload')
}
export function getPatientReport() {
  return request.get('/admin/reports/patient')
}
export function getBedUtilizationReport() {
  return request.get('/admin/reports/bed-utilization')
}
export function getMedicineInventoryReport() {
  return request.get('/admin/reports/medicine-inventory')
}
export function getOverviewReport() {
  return request.get('/admin/reports/overview')
}

// 药品流水
export function getMedicineFlows(params) {
  return request.get('/admin/medicine-flows', { params })
}
