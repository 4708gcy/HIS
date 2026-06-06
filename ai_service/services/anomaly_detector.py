"""
异常检测引擎（LangChain 版）
"""
import time
from typing import List, Dict, Any
from collections import defaultdict
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder


class AnomalyDetector:
    """异常检测引擎：Z-score + LangChain LLM 解读"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()

    def detect(self, months: int = 12, department: str = None,
               threshold: float = 2.0, strategy: str = "auto") -> Dict[str, Any]:
        start_time = time.time()

        stats = DataLoader.load_monthly_stats(months, department)
        if not stats:
            return {"error": "无可用数据"}

        dept_data = defaultdict(dict)
        for s in stats:
            dept_data[s["department"]][s["month"]] = s["new_admissions"]

        all_anomalies = []
        for dept, month_values in dept_data.items():
            anomalies = TraditionalAnalyzer.zscore_anomaly_detect(month_values, threshold)
            for a in anomalies:
                a["department"] = dept
                if strategy in ("llm", "auto"):
                    a["interpretation"] = self._llm_interpret(a)
                all_anomalies.append(a)

        return {
            "threshold": threshold,
            "anomaly_count": len(all_anomalies),
            "anomalies": all_anomalies,
            "processing_time": round(time.time() - start_time, 3)
        }

    def _llm_interpret(self, anomaly: Dict[str, Any]) -> str:
        """LangChain LLM 解读异常原因"""
        try:
            prompt = self.prompt_builder.get_anomaly_prompt(anomaly)
            return llm_client.invoke_with_prompt(prompt, temperature=0.5)
        except Exception as e:
            return f"无法生成解读: {str(e)}"
