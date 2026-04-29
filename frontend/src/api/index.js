import axios from 'axios'
import { ElMessage } from 'element-plus'
import { useUserStore } from '../store/user'
import router from '../router'

const request = axios.create({
  baseURL: '/api',
  timeout: 10000
})

request.interceptors.request.use(config => {
  const store = useUserStore()
  if (store.token) {
    config.headers.Authorization = `Bearer ${store.token}`
  }
  return config
})

request.interceptors.response.use(
  response => {
    const data = response.data
    // 业务错误（后端返回 code !== 200）
    if (data?.code !== 200) {
      ElMessage.error(data?.message || '请求失败')
      return Promise.reject(new Error(data?.message || '请求失败'))
    }
    return data
  },
  error => {
    if (error.response?.status === 401) {
      const store = useUserStore()
      store.logout()
      router.push('/login')
      return Promise.reject(new Error('登录已过期，请重新登录'))
    } else if (error.response?.status === 404) {
      return Promise.reject(new Error('请求的资源不存在'))
    } else if (error.response?.status === 403) {
      return Promise.reject(new Error('没有权限访问该资源'))
    } else if (error.code === 'ECONNABORTED') {
      return Promise.reject(new Error('请求超时，请稍后重试'))
    }
    return Promise.reject(new Error(error.response?.data?.message || error.message || '网络错误'))
  }
)

export default request
