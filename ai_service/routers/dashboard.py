"""
综合仪表盘路由
"""
import time
from fastapi import APIRouter, Depends
from core.security import verify_api_key
from core.response import ResponseBuilder
from schemas import DashboardRequest
from services.report_generator import ReportGenerator

router = APIRouter(prefix="/api/v2/dashboard", tags=["综合仪表盘"])
report_gen = ReportGenerator()


@router.post("", dependencies=[Depends(verify_api_key)])
def get_dashboard(req: DashboardRequest):
    start = time.time()
    result = report_gen.generate(strategy=req.strategy.value)
    return ResponseBuilder.success(result, "仪表盘数据加载完成", start_time=start)
