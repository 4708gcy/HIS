import request from './index'

export function getMedicationRecords() {
  return request.get('/pharmacist/medication-records')
}
export function getMedicationRecord(id) {
  return request.get(`/pharmacist/medication-records/${id}`)
}
export function createMedicationRecord(data) {
  return request.post('/pharmacist/medication-records', data)
}
export function deleteMedicationRecord(id) {
  return request.delete(`/pharmacist/medication-records/${id}`)
}
export function reviewMedicationRecord(id, reviewStatus) {
  return request.put(`/pharmacist/medication-records/${id}/review`, { reviewStatus })
}
export function dispenseMedication(id) {
  return request.put(`/pharmacist/medication-records/${id}/dispense`)
}
export function getMedicines() {
  return request.get('/pharmacist/medicines')
}
export function createMedicine(data) {
  return request.post('/pharmacist/medicines', data)
}
export function updateMedicine(id, data) {
  return request.put(`/pharmacist/medicines/${id}`, data)
}
export function deleteMedicine(id) {
  return request.delete(`/pharmacist/medicines/${id}`)
}
export function updateMedicineStock(id, delta) {
  return request.put(`/pharmacist/medicines/${id}/stock`, { delta })
}
export function getProfile() {
  return request.get('/pharmacist/profile')
}
export function updateProfile(data) {
  return request.put('/pharmacist/profile', data)
}
