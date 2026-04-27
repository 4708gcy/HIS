import request from './index'

export function getDepartments() {
  return request.get('/departments')
}

export function getFeeStandards() {
  return request.get('/fee-standards')
}
