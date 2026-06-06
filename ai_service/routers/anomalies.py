"""
异常检测路由
"""
import time
from fastapi import APIRouter, Depends
from core.security import verify_api_key
from core.response import ResponseBuilder
from schemas import AnomalyRequest
from services.anomaly_detector import AnomalyDetector

router = APIRouter(prefix="/api/v2/anomalies", tags=["异常检测"])
detector = AnomalyDetector()


@router.post("", dependencies=[Depends(verify_api_key)])
def detect_anomalies(req: AnomalyRequest):
    """异常检测 + LLM 解读"""
    start = time.time()
    result = detector.detect(
        months=req.months,
        department=req.department,
        threshold=req.threshold,
        strategy=req.strategy.value
    )
    if "error" in result:
        return ResponseBuilder.error(result["error"], code=400, start_time=start)
    return ResponseBuilder.success(result, "异常检测完成", start_time=start)
