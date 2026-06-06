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
    """LangChain Prompt 构建器测试"""

    def test_get_prediction_prompt_structure(self):
        builder = PromptBuilder()
        current = {
            "department": "心内科",
            "months_data": [
                {"month": "2024-01", "new_admissions": 100},
                {"month": "2024-02", "new_admissions": 110}
            ]
        }
        historical = [
            {"department": "心内科", "month": "2023-12", "new_admissions": 95, "trend": "上升"}
        ]
        prompt = builder.get_prediction_prompt(current, historical)
        # 返回的是 ChatPromptValue，转为字符串检查
        prompt_str = str(prompt)
        assert "心内科" in prompt_str
        assert "2024-01" in prompt_str
        assert "JSON" in prompt_str

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

    def test_find_similar_cases_basic(self):
        builder = PromptBuilder()
        historical = [
            {"department": "心内科", "month": "2023-01", "new_admissions": 100},
            {"department": "心内科", "month": "2023-02", "new_admissions": 110},
            {"department": "外科", "month": "2023-01", "new_admissions": 80},
        ]
        similar = builder._find_similar_cases("心内科 2024-01 入院100人", historical)
        assert len(similar) > 0


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
