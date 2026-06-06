"""
统一响应封装
参考来源：04-government-rag 的标准化 Response 设计
"""
import uuid
import time
from typing import Dict, Any, Optional
from fastapi.responses import JSONResponse


class ResponseBuilder:
    """构建统一格式响应"""

    @staticmethod
    def success(data: Optional[Dict[str, Any]] = None, msg: str = "ok",
                start_time: Optional[float] = None) -> JSONResponse:
        processing_time = round(time.time() - start_time, 3) if start_time else 0.0
        return JSONResponse(content={
            "request_id": str(uuid.uuid4()),
            "data": data or {},
            "response_code": 200,
            "response_msg": msg,
            "process_status": "completed",
            "processing_time": processing_time
        })

    @staticmethod
    def error(msg: str, code: int = 500, data: Optional[Dict[str, Any]] = None,
              start_time: Optional[float] = None) -> JSONResponse:
        processing_time = round(time.time() - start_time, 3) if start_time else 0.0
        return JSONResponse(
            status_code=code,
            content={
                "request_id": str(uuid.uuid4()),
                "data": data or {},
                "response_code": code,
                "response_msg": msg,
                "process_status": "failed",
                "processing_time": processing_time
            }
        )

    @staticmethod
    def processing(request_id: str) -> Dict[str, Any]:
        return {
            "request_id": request_id,
            "data": None,
            "response_code": 202,
            "response_msg": "processing",
            "process_status": "processing",
            "processing_time": 0.0
        }
