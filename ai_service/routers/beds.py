"""
床位优化路由
"""
import time
from fastapi import APIRouter, Depends
from core.security import verify_api_key
from core.response import ResponseBuilder
from schemas import BedOptimizeRequest
from services.bed_optimizer import BedOptimizer

router = APIRouter(prefix="/api/v2/beds", tags=["床位优化"])
optimizer = BedOptimizer()


@router.post("", dependencies=[Depends(verify_api_key)])
def optimize_beds(req: BedOptimizeRequest):
    start = time.time()
    result = optimizer.optimize(
        department=req.department,
        strategy=req.strategy.value
    )
    if "error" in result:
        return ResponseBuilder.error(result["error"], code=400, start_time=start)
    return ResponseBuilder.success(result, "床位优化完成", start_time=start)
