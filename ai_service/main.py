"""
HIS AI Service v2.0 — FastAPI 入口

参考来源：
- 11_文本分类_fastapi.py（多策略路由）
- fastapi-student-crud-demo（Pydantic + HTTPException）
- es_kb_api.py（Depends 认证 + 复杂路由）
- 04-government-rag/main.py（统一响应 + 后台任务）
"""
import os
import sys

# 将当前目录加入路径，方便导入
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from core.config import settings
from routers import predictions, anomalies, beds, dashboard, charts, rag

app = FastAPI(
    title=settings.section("app").get("name", "HIS-AI-Service"),
    description="HIS 医院信息系统 AI 分析微服务 v2.0 — LLM 增强版",
    version=settings.section("app").get("version", "2.0.0")
)

# CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 注册路由
app.include_router(predictions.router)
app.include_router(anomalies.router)
app.include_router(beds.router)
app.include_router(dashboard.router)
app.include_router(charts.router)
app.include_router(rag.router)


@app.get("/health")
def health_check():
    """健康检查"""
    return {"status": "ok", "version": "2.0.0"}


@app.get("/")
def root():
    """根路径"""
    return {
        "service": "HIS AI Service",
        "version": "2.0.0",
        "docs": "/docs",
        "health": "/health"
    }


if __name__ == "__main__":
    import uvicorn
    host = settings.section("app").get("host", "127.0.0.1")
    port = settings.section("app").get("port", 5001)
    uvicorn.run(app, host=host, port=port)
