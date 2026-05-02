import request from './index'

export function getDepartments() {
  return request.get('/departments')
}

export function getDoctors(params) {
  return request.get('/doctors', { params })
}

export function getPatients(params) {
  return request.get('/admin/patients', { params })
}

export function getFeeStandards() {
  return request.get('/fee-standards')
}

export function getExaminationItems() {
  return request.get('/examination-items')
}

export function getSchedules(params) {
  return request.get('/schedules', { params })
}
