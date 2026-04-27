import request from './index'

export function getDepartments() {
  return request.get('/departments')
}

export function getFeeStandards() {
  return request.get('/fee-standards')
}

export function getExaminationItems() {
  return request.get('/examination-items')
}
