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
export function updateProfile(data) {
  return request.put('/nurse/profile', data)
}
export function createHospitalization(data) {
  return request.post('/nurse/hospitalizations', data)
}
export function deleteHospitalization(id) {
  return request.delete(`/nurse/hospitalizations/${id}`)
}
export function assignBed(hospitalizationId, data) {
  return request.post(`/nurse/hospitalizations/${hospitalizationId}/assign-bed`, data)
}
export function dischargePatient(id) {
  return request.post(`/nurse/hospitalizations/${id}/discharge`)
}
export function createBed(data) {
  return request.post('/nurse/beds', data)
}
export function updateBed(id, data) {
  return request.put(`/nurse/beds/${id}`, data)
}
export function deleteBed(id) {
  return request.delete(`/nurse/beds/${id}`)
}
