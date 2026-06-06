"""
预测路由
"""
import time
from fastapi import APIRouter, Depends, Query
from core.security import verify_api_key
from core.response import ResponseBuilder
from schemas import PredictRequest, Strategy
from services.predictor import Predictor

router = APIRouter(prefix="/api/v2/predictions", tags=["预测分析"])
predictor = Predictor()


@router.post("", dependencies=[Depends(verify_api_key)])
def predict_demand(req: PredictRequest):
    start = time.time()
    result = predictor.predict(
        months=req.months,
        department=req.department,
        strategy=req.strategy.value
    )
    if "error" in result:
        return ResponseBuilder.error(result["error"], code=400, start_time=start)
    return ResponseBuilder.success(result, "预测完成", start_time=start)
