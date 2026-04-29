import request from './index'

export function login(data) {
  return request.post('/auth/login', data)
}

export function register(data) {
  return request.post('/auth/register', data)
}

export function changePassword(oldPassword, newPassword) {
  return request.put('/auth/change-password', { oldPassword, newPassword })
}
