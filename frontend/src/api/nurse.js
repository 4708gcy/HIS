import request from './index'

export function getHospitalizations() {
  return request.get('/nurse/hospitalizations')
}
export function getHospitalization(id) {
  return request.get(`/nurse/hospitalizations/${id}`)
}
export function updateHospitalization(id, data) {
  return request.put(`/nurse/hospitalizations/${id}`, data)
}
export function getExaminations() {
  return request.get('/nurse/examinations')
}
export function updateVitalSigns(id, vitalSigns) {
  return request.put(`/nurse/examinations/${id}/vitals`, { vitalSigns })
}
export function getBeds(params) {
  return request.get('/nurse/beds', { params })
}
export function getProfile() {
  return request.get('/nurse/profile')
}
