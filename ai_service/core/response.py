"""统一响应封装"""
import json
import uuid
import time
from decimal import Decimal
from typing import Optional
from fastapi.responses import JSONResponse


class _DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super().default(obj)


class ResponseBuilder:

    @staticmethod
    def success(data: Optional[dict] = None, msg: str = "ok",
                start_time: Optional[float] = None) -> JSONResponse:
        elapsed = round(time.time() - start_time, 3) if start_time else 0.0
        content = {
            "request_id": str(uuid.uuid4()),
            "data": data or {},
            "response_code": 200,
            "response_msg": msg,
            "process_status": "completed",
            "processing_time": elapsed
        }
        return JSONResponse(content=json.loads(json.dumps(content, cls=_DecimalEncoder)))

    @staticmethod
    def error(msg: str, code: int = 500, data: Optional[dict] = None,
              start_time: Optional[float] = None) -> JSONResponse:
        elapsed = round(time.time() - start_time, 3) if start_time else 0.0
        content = {
            "request_id": str(uuid.uuid4()),
            "data": data or {},
            "response_code": code,
            "response_msg": msg,
            "process_status": "failed",
            "processing_time": elapsed
        }
        return JSONResponse(
            status_code=code,
            content=json.loads(json.dumps(content, cls=_DecimalEncoder))
        )

    @staticmethod
    def processing(request_id: str) -> dict:
        return {
            "request_id": request_id,
            "data": None,
            "response_code": 202,
            "response_msg": "processing",
            "process_status": "processing",
            "processing_time": 0.0
        }
