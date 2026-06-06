"""API Key 认证"""
import hmac
from fastapi import Header, HTTPException, status
from core.config import settings


async def verify_api_key(authorization: str = Header(None, alias="X-API-Key")):
    # X-API-Key 请求头认证
    expected = settings.security.get("api_key", "")
    if not expected or not authorization or not hmac.compare_digest(authorization.encode(), expected.encode()):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail={
                "error_code": 1001,
                "error_msg": "Unauthorized — 缺少有效的 API Key"
            }
        )
    return True
