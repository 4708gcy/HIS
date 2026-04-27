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
      { path: 'dashboard', name: 'Dashboard', component: () => import('../views/admin/Dashboard.vue') },
      // 管理员路由
      ...[
        { path: 'admin/doctors', name: 'AdminDoctors', component: () => import('../views/admin/Doctors.vue') },
        { path: 'admin/nurses', name: 'AdminNurses', component: () => import('../views/admin/Nurses.vue') },
        { path: 'admin/pharmacists', name: 'AdminPharmacists', component: () => import('../views/admin/Pharmacists.vue') },
        { path: 'admin/patients', name: 'AdminPatients', component: () => import('../views/admin/Patients.vue') },
        { path: 'admin/registrations', name: 'AdminRegistrations', component: () => import('../views/admin/Registrations.vue') },
        { path: 'admin/consultations', name: 'AdminConsultations', component: () => import('../views/admin/Consultations.vue') },
        { path: 'admin/examinations', name: 'AdminExaminations', component: () => import('../views/admin/Examinations.vue') },
        { path: 'admin/hospitalizations', name: 'AdminHospitalizations', component: () => import('../views/admin/Hospitalizations.vue') },
        { path: 'admin/medication-records', name: 'AdminMedicationRecords', component: () => import('../views/admin/MedicationRecords.vue') },
        { path: 'admin/medicines', name: 'AdminMedicines', component: () => import('../views/admin/Medicines.vue') },
        { path: 'admin/beds', name: 'AdminBeds', component: () => import('../views/admin/Beds.vue') },
        { path: 'admin/profile', name: 'AdminProfile', component: () => import('../views/admin/Profile.vue') }
      ],
      // 医生路由
      ...[
        { path: 'doctor/registrations', name: 'DoctorRegistrations', component: () => import('../views/doctor/Registrations.vue') },
        { path: 'doctor/consultations', name: 'DoctorConsultations', component: () => import('../views/doctor/Consultations.vue') },
        { path: 'doctor/examinations', name: 'DoctorExaminations', component: () => import('../views/doctor/Examinations.vue') },
        { path: 'doctor/profile', name: 'DoctorProfile', component: () => import('../views/doctor/Profile.vue') }
      ],
      // 护士路由
      ...[
        { path: 'nurse/hospitalizations', name: 'NurseHospitalizations', component: () => import('../views/nurse/Hospitalizations.vue') },
        { path: 'nurse/examinations', name: 'NurseExaminations', component: () => import('../views/nurse/Examinations.vue') },
        { path: 'nurse/beds', name: 'NurseBeds', component: () => import('../views/nurse/Beds.vue') },
        { path: 'nurse/profile', name: 'NurseProfile', component: () => import('../views/nurse/Profile.vue') }
      ],
      // 药剂师路由
      ...[
        { path: 'pharmacist/medication-records', name: 'PharmacistMedRecords', component: () => import('../views/pharmacist/MedicationRecords.vue') },
        { path: 'pharmacist/medicines', name: 'PharmacistMedicines', component: () => import('../views/pharmacist/Medicines.vue') },
        { path: 'pharmacist/profile', name: 'PharmacistProfile', component: () => import('../views/pharmacist/Profile.vue') }
      ],
      // 患者路由
      ...[
        { path: 'patient/registrations', name: 'PatientRegistrations', component: () => import('../views/patient/Registrations.vue') },
        { path: 'patient/consultations', name: 'PatientConsultations', component: () => import('../views/patient/Consultations.vue') },
        { path: 'patient/examinations', name: 'PatientExaminations', component: () => import('../views/patient/Examinations.vue') },
        { path: 'patient/medication-records', name: 'PatientMedRecords', component: () => import('../views/patient/MedicationRecords.vue') },
        { path: 'patient/hospitalizations', name: 'PatientHospitalizations', component: () => import('../views/patient/Hospitalizations.vue') },
        { path: 'patient/profile', name: 'PatientProfile', component: () => import('../views/patient/Profile.vue') }
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
  } else {
    next()
  }
})

export default router
