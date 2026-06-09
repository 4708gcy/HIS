"""
HIS AI Service v2.0 — 核心服务单元测试（LangChain 版）
"""
import pytest
from services.analyzer import TraditionalAnalyzer
from services.prompt_builder import PromptBuilder


class TestTraditionalAnalyzer:
    """传统统计算法测试"""

    def test_holt_winters_basic(self):
        values = [100, 110, 105, 115, 120, 130]
        result = TraditionalAnalyzer.holt_winters_forecast(values)
        assert result is not None
        assert result > 130

    def test_holt_winters_short_series(self):
        assert TraditionalAnalyzer.holt_winters_forecast([10]) is None
        assert TraditionalAnalyzer.holt_winters_forecast([10, 20]) is None

    def test_zscore_detects_outlier(self):
        # 增加正常样本数量，使异常值 500 的 Z-score 超过 2.0
        data = {"2024-01": 100, "2024-02": 101, "2024-03": 99,
                "2024-04": 100, "2024-05": 102, "2024-06": 500}
        anomalies = TraditionalAnalyzer.zscore_anomaly_detect(data, threshold=2.0)
        assert len(anomalies) == 1
        assert anomalies[0]["month"] == "2024-06"
        assert anomalies[0]["direction"] == "high"

    def test_zscore_no_anomaly(self):
        data = {"2024-01": 100, "2024-02": 101, "2024-03": 99, "2024-04": 100}
        anomalies = TraditionalAnalyzer.zscore_anomaly_detect(data, threshold=2.0)
        assert anomalies == []

    def test_zscore_empty(self):
        assert TraditionalAnalyzer.zscore_anomaly_detect({}, threshold=2.0) == []


class TestPromptBuilder:
    """Prompt 构建器测试"""

    def test_get_prediction_summary_prompt(self):
        builder = PromptBuilder()
        predictions = [
            {"department": "内科", "predicted_next_month": 45, "growth_rate": 5.2, "confidence": "medium"},
            {"department": "外科", "predicted_next_month": 30, "growth_rate": -10.0, "confidence": "medium"},
        ]
        prompt = builder.get_prediction_summary_prompt(predictions)
        prompt_str = str(prompt)
        assert "内科" in prompt_str
        assert "45" in prompt_str
        assert "增长率" in prompt_str

    def test_get_anomaly_prompt(self):
        builder = PromptBuilder()
        anomaly = {
            "department": "心内科",
            "month": "2024-03",
            "value": 200,
            "mean": 100,
            "stddev": 20,
            "z_score": 5.0,
            "direction": "high"
        }
        prompt = builder.get_anomaly_prompt(anomaly)
        prompt_str = str(prompt)
        assert "心内科" in prompt_str
        assert "200" in prompt_str
        assert "Z-score" in prompt_str

    def test_get_bed_prompt(self):
        builder = PromptBuilder()
        bed = {
            "department": "心内科",
            "total_beds": 50,
            "occupied": 45,
            "available": 3,
            "cleaning": 2,
            "utilization_rate": 90.0,
            "avg_stay_days": 7.5
        }
        prompt = builder.get_bed_prompt(bed)
        prompt_str = str(prompt)
        assert "心内科" in prompt_str
        assert "50" in prompt_str
        assert "JSON" in prompt_str

import os

class TestLLMProvider:
    """LangChain LLM Provider 测试（需要有效 API Key）"""

    @pytest.mark.skipif(
        not os.environ.get("HIS_LLM_API_KEY") and not os.environ.get("OPENAI_API_KEY"),
        reason="未配置 LLM API Key，跳过 LLM 初始化测试"
    )
    def test_get_llm_returns_chat_openai(self):
        from core.llm_provider import get_llm
        from langchain_openai import ChatOpenAI
        llm = get_llm()
        assert isinstance(llm, ChatOpenAI)

    @pytest.mark.skipif(
        not os.environ.get("HIS_LLM_API_KEY") and not os.environ.get("OPENAI_API_KEY"),
        reason="未配置 LLM API Key，跳过 LLM 初始化测试"
    )
    def test_get_llm_with_temperature(self):
        from core.llm_provider import get_llm
        llm = get_llm(temperature=0.5)
        assert llm.temperature == 0.5


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
