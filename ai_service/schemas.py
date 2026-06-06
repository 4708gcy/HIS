"""Pydantic 数据模型"""
from typing import Optional, List, Dict, Any
from pydantic import BaseModel, Field
from enum import Enum


class Strategy(str, Enum):
    """分析策略枚举"""
    TRADITIONAL = "traditional"
    LLM = "llm"
    AUTO = "auto"


class HISResponse(BaseModel):
    """
    统一响应格式
    """
    request_id: str = Field(..., description="唯一请求 ID")
    data: Optional[Dict[str, Any]] = Field(None, description="业务数据")
    response_code: int = Field(..., description="HTTP 风格状态码")
    response_msg: str = Field(..., description="状态描述")
    process_status: str = Field(..., description="处理状态: processing / completed / failed")
    processing_time: float = Field(..., description="处理耗时（秒）")


class PredictRequest(BaseModel):
    """需求预测请求"""
    department: Optional[str] = Field(None, description="科室名称，为空则全量")
    months: int = Field(6, ge=3, le=24, description="参考历史月数")
    strategy: Strategy = Field(Strategy.AUTO, description="分析策略")


class PredictResponseData(BaseModel):
    """预测结果数据"""
    department: str
    predicted_next_month: int
    growth_rate: float
    confidence: str                          # high / medium / low
    interpretation: str                      # LLM 自然语言解读
    historical_context: List[Dict[str, Any]] # 相似历史场景


class AnomalyRequest(BaseModel):
    """异常检测请求"""
    department: Optional[str] = Field(None, description="科室名称")
    threshold: float = Field(2.0, ge=1.0, le=4.0, description="Z-score 阈值")
    strategy: Strategy = Field(Strategy.AUTO, description="分析策略")


class AnomalyItem(BaseModel):
    """单个异常项"""
    department: str
    month: str
    value: int
    z_score: float
    direction: str                           # high / low
    interpretation: Optional[str] = None     # LLM 解读


class BedOptimizeRequest(BaseModel):
    """床位优化请求"""
    department: Optional[str] = Field(None, description="科室名称")
    strategy: Strategy = Field(Strategy.AUTO, description="分析策略")


class BedOptimizeData(BaseModel):
    """床位优化结果"""
    department: str
    total_beds: int
    occupied: int
    available: int
    utilization_rate: float
    suggestions: List[str]                   # 结构化建议
    report: Optional[str] = None             # LLM 自然语言报告


class DashboardRequest(BaseModel):
    """综合仪表盘请求"""
    strategy: Strategy = Field(Strategy.LLM, description="分析策略")


class RAGChatRequest(BaseModel):
    """RAG 知识库问答请求"""
    query: str = Field(..., min_length=1, description="用户问题")
    top_k: int = Field(5, ge=1, le=10, description="检索片段数")


class RAGChatResponseData(BaseModel):
    """RAG 问答结果"""
    answer: str
    sources: List[Dict[str, Any]]            # 引用来源


class DocumentUploadResponse(BaseModel):
    """文档上传响应"""
    document_id: str
    filename: str
    status: str                              # processing / completed / failed


class ChartRequest(BaseModel):
    """图表生成请求"""
    chart_type: str = Field(..., pattern="^(prediction|bed|anomaly|medicine)$")
    department: Optional[str] = None
