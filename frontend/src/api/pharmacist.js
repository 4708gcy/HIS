import request from './index'

export function getMedicationRecords() {
  return request.get('/pharmacist/medication-records')
}
export function getMedicationRecord(id) {
  return request.get(`/pharmacist/medication-records/${id}`)
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
export function updateMedicineStock(id, delta) {
  return request.put(`/pharmacist/medicines/${id}/stock`, { delta })
}
export function getProfile() {
  return request.get('/pharmacist/profile')
}
