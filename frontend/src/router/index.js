import { createRouter, createWebHistory } from 'vue-router'
import { useUserStore } from '../store/user'

const routes = [
  {
    path: '/login',
    name: 'Login',
    component: () => import('../views/Login.vue')
  },
  {
    path: '/register',
    name: 'Register',
    component: () => import('../views/Register.vue')
  },
  {
    path: '/',
    component: () => import('../views/Layout.vue'),
    redirect: '/dashboard',
    meta: { requiresAuth: true },
    children: [
      { path: 'dashboard', name: 'Dashboard', component: () => import('../views/admin/Dashboard.vue'), meta: { roles: [1] } },
      // 管理员路由
      ...[
        { path: 'admin/doctors', name: 'AdminDoctors', component: () => import('../views/admin/Doctors.vue'), meta: { roles: [1] } },
        { path: 'admin/nurses', name: 'AdminNurses', component: () => import('../views/admin/Nurses.vue'), meta: { roles: [1] } },
        { path: 'admin/pharmacists', name: 'AdminPharmacists', component: () => import('../views/admin/Pharmacists.vue'), meta: { roles: [1] } },
        { path: 'admin/patients', name: 'AdminPatients', component: () => import('../views/admin/Patients.vue'), meta: { roles: [1] } },
        { path: 'admin/registrations', name: 'AdminRegistrations', component: () => import('../views/admin/Registrations.vue'), meta: { roles: [1] } },
        { path: 'admin/consultations', name: 'AdminConsultations', component: () => import('../views/admin/Consultations.vue'), meta: { roles: [1] } },
        { path: 'admin/examinations', name: 'AdminExaminations', component: () => import('../views/admin/Examinations.vue'), meta: { roles: [1] } },
        { path: 'admin/hospitalizations', name: 'AdminHospitalizations', component: () => import('../views/admin/Hospitalizations.vue'), meta: { roles: [1] } },
        { path: 'admin/medication-records', name: 'AdminMedicationRecords', component: () => import('../views/admin/MedicationRecords.vue'), meta: { roles: [1] } },
        { path: 'admin/medicines', name: 'AdminMedicines', component: () => import('../views/admin/Medicines.vue'), meta: { roles: [1] } },
        { path: 'admin/beds', name: 'AdminBeds', component: () => import('../views/admin/Beds.vue'), meta: { roles: [1] } },
        { path: 'admin/profile', name: 'AdminProfile', component: () => import('../views/admin/Profile.vue'), meta: { roles: [1] } }
      ],
      // 医生路由
      ...[
        { path: 'doctor/registrations', name: 'DoctorRegistrations', component: () => import('../views/doctor/Registrations.vue'), meta: { roles: [2] } },
        { path: 'doctor/consultations', name: 'DoctorConsultations', component: () => import('../views/doctor/Consultations.vue'), meta: { roles: [2] } },
        { path: 'doctor/examinations', name: 'DoctorExaminations', component: () => import('../views/doctor/Examinations.vue'), meta: { roles: [2] } },
        { path: 'doctor/profile', name: 'DoctorProfile', component: () => import('../views/doctor/Profile.vue'), meta: { roles: [2] } }
      ],
      // 护士路由
      ...[
        { path: 'nurse/hospitalizations', name: 'NurseHospitalizations', component: () => import('../views/nurse/Hospitalizations.vue'), meta: { roles: [3] } },
        { path: 'nurse/examinations', name: 'NurseExaminations', component: () => import('../views/nurse/Examinations.vue'), meta: { roles: [3] } },
        { path: 'nurse/beds', name: 'NurseBeds', component: () => import('../views/nurse/Beds.vue'), meta: { roles: [3] } },
        { path: 'nurse/profile', name: 'NurseProfile', component: () => import('../views/nurse/Profile.vue'), meta: { roles: [3] } }
      ],
      // 药剂师路由
      ...[
        { path: 'pharmacist/medication-records', name: 'PharmacistMedRecords', component: () => import('../views/pharmacist/MedicationRecords.vue'), meta: { roles: [4] } },
        { path: 'pharmacist/medicines', name: 'PharmacistMedicines', component: () => import('../views/pharmacist/Medicines.vue'), meta: { roles: [4] } },
        { path: 'pharmacist/profile', name: 'PharmacistProfile', component: () => import('../views/pharmacist/Profile.vue'), meta: { roles: [4] } }
      ],
      // 患者路由
      ...[
        { path: 'patient/registrations', name: 'PatientRegistrations', component: () => import('../views/patient/Registrations.vue'), meta: { roles: [5] } },
        { path: 'patient/consultations', name: 'PatientConsultations', component: () => import('../views/patient/Consultations.vue'), meta: { roles: [5] } },
        { path: 'patient/examinations', name: 'PatientExaminations', component: () => import('../views/patient/Examinations.vue'), meta: { roles: [5] } },
        { path: 'patient/medication-records', name: 'PatientMedRecords', component: () => import('../views/patient/MedicationRecords.vue'), meta: { roles: [5] } },
        { path: 'patient/hospitalizations', name: 'PatientHospitalizations', component: () => import('../views/patient/Hospitalizations.vue'), meta: { roles: [5] } },
        { path: 'patient/profile', name: 'PatientProfile', component: () => import('../views/patient/Profile.vue'), meta: { roles: [5] } }
      ]
    ]
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

router.beforeEach((to, from, next) => {
  const store = useUserStore()
  if (to.meta.requiresAuth && !store.isLoggedIn) {
    next('/login')
  } else if ((to.path === '/login' || to.path === '/register') && store.isLoggedIn) {
    next('/dashboard')
  } else if (to.meta.roles && !to.meta.roles.includes(Number(store.role))) {
    // 角色不匹配，重定向到对应角色的默认页面
    const roleRedirects = { 1: '/dashboard', 2: '/doctor/registrations', 3: '/nurse/hospitalizations', 4: '/pharmacist/medication-records', 5: '/patient/registrations' }
    next(roleRedirects[Number(store.role)] || '/dashboard')
  } else {
    next()
  }
})

export default router
