import request from './index'

export function getMyRegistrations() {
  return request.get('/patient/registrations')
}
export function createRegistration(data) {
  return request.post('/patient/registrations', data)
}
export function payRegistration(id) {
  return request.put(`/patient/registrations/${id}/pay`)
}
export function getMyConsultations() {
  return request.get('/patient/consultations')
}
export function getMyExaminations() {
  return request.get('/patient/examinations')
}
export function payExamination(id) {
  return request.put(`/patient/examinations/${id}/pay`)
}
export function getMyMedicationRecords() {
  return request.get('/patient/medication-records')
}
export function payMedicationRecord(id) {
  return request.put(`/patient/medication-records/${id}/pay`)
}
export function getMyHospitalizations() {
  return request.get('/patient/hospitalizations')
}
export function payHospitalization(id, deposit) {
  return request.put(`/patient/hospitalizations/${id}/pay`, { deposit })
}
export function recharge(amount) {
  return request.post('/patient/recharge', { amount })
}
export function getProfile() {
  return request.get('/patient/profile')
}
export function updateProfile(data) {
  return request.put('/patient/profile', data)
}
export function deleteRegistration(id) {
  return request.delete(`/patient/registrations/${id}`)
}
