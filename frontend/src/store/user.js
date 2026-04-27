import { defineStore } from 'pinia'
import { ref, computed } from 'vue'

export const useUserStore = defineStore('user', () => {
  const token = ref(localStorage.getItem('token') || '')
  const userID = ref(localStorage.getItem('userID') || '')
  const username = ref(localStorage.getItem('username') || '')
  const role = ref(parseInt(localStorage.getItem('role') || '0'))
  const roleStr = ref(localStorage.getItem('roleStr') || '')

  const isLoggedIn = computed(() => !!token.value)

  const roleNames = { 1: '管理员', 2: '医生', 3: '护士', 4: '药剂师', 5: '患者' }
  const roleName = computed(() => roleNames[role.value] || '')

  function setLogin(data) {
    token.value = data.token
    userID.value = data.userID
    username.value = data.username
    role.value = data.role
    roleStr.value = data.roleStr
    localStorage.setItem('token', data.token)
    localStorage.setItem('userID', data.userID)
    localStorage.setItem('username', data.username)
    localStorage.setItem('role', String(data.role))
    localStorage.setItem('roleStr', data.roleStr)
  }

  function logout() {
    token.value = ''
    userID.value = ''
    username.value = ''
    role.value = 0
    roleStr.value = ''
    localStorage.removeItem('token')
    localStorage.removeItem('userID')
    localStorage.removeItem('username')
    localStorage.removeItem('role')
    localStorage.removeItem('roleStr')
  }

  return { token, userID, username, role, roleStr, isLoggedIn, roleName, setLogin, logout }
})
