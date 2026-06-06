"""
API Key 认证
参考来源：es_kb_api.py 的 Depends 注入 + HIS 原项目的 API Key 设计
"""
from fastapi import Header, HTTPException, status
from core.config import settings


async def verify_api_key(authorization: str = Header(None, alias="X-API-Key")):
    """
    验证 API Key（依赖注入方式）
    请求头中必须包含 X-API-Key
    """
    expected = settings.security.get("api_key") or "his-default-key"
    if not authorization or authorization != expected:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail={
                "error_code": 1001,
                "error_msg": "Unauthorized — 缺少有效的 API Key"
            }
        )
    return True
