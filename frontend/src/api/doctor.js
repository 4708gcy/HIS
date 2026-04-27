import request from './index'

export function getMyRegistrations() {
  return request.get('/doctor/registrations')
}
export function getMyConsultations() {
  return request.get('/doctor/consultations')
}
export function getConsultation(id) {
  return request.get(`/doctor/consultations/${id}`)
}
export function updateConsultation(id, data) {
  return request.put(`/doctor/consultations/${id}`, data)
}
export function getMyExaminations() {
  return request.get('/doctor/examinations')
}
export function getExamination(id) {
  return request.get(`/doctor/examinations/${id}`)
}
export function updateExamination(id, data) {
  return request.put(`/doctor/examinations/${id}`, data)
}
export function getProfile() {
  return request.get('/doctor/profile')
}
export function updateProfile(data) {
  return request.put('/doctor/profile', data)
}
export function createConsultation(data) {
  return request.post('/doctor/consultations', data)
}
export function createExaminations(data) {
  return request.post('/doctor/examinations', data)
}
