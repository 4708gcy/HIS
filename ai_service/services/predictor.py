"""需求预测引擎"""
import json
import re
import time
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder
from core.config import settings


def extract_json(raw):
    """从 LLM 输出提取 JSON"""
    match = re.search(r'```(?:json)?\s*\n?(.*?)```', raw, re.DOTALL)
    text = match.group(1).strip() if match else raw.strip()
    return json.loads(text)


class Predictor:
    """traditional / llm / auto 三策略"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()
        self.auto_threshold = settings.analysis.get("auto_threshold", 12)

    def predict(self, months=6, department=None, strategy="auto"):
        start = time.time()

        stats = DataLoader.load_monthly_stats(months, department)
        if not stats:
            return {"error": "无可用数据"}

        if strategy == "auto":
            strategy = "traditional" if len(stats) >= self.auto_threshold else "llm"

        dept_data = {}
        for s in stats:
            dept = s["department"]
            if dept not in dept_data:
                dept_data[dept] = []
            dept_data[dept].append(s)

        results = []
        for dept, data in dept_data.items():
            if strategy == "traditional":
                result = self._predict_traditional(dept, data)
            else:
                result = self._predict_llm(dept, data, stats)
            results.append(result)

        return {
            "strategy": strategy,
            "predictions": results,
            "processing_time": round(time.time() - start, 3)
        }

    def _predict_traditional(self, dept, data):
        values = [d["new_admissions"] for d in data]
        n = len(values)
        ma_count = min(n, 3)
        moving_avg = sum(values[-ma_count:]) / ma_count

        hw = TraditionalAnalyzer.holt_winters_forecast(values)
        predicted = max(0, round(hw if hw else moving_avg))

        growth_rate = 0.0
        if n >= 2 and values[-2] > 0:
            growth_rate = round((values[-1] - values[-2]) * 100.0 / values[-2], 1)

        return {
            "department": dept,
            "predicted_next_month": predicted,
            "growth_rate": growth_rate,
            "confidence": "medium",
            "interpretation": f"基于 Holt-Winters 预测，下月预计入院 {predicted} 人",
            "historical_context": []
        }

    def _predict_llm(self, dept, data, all_stats):
        current_data = {
            "department": dept,
            "months_data": data
        }

        try:
            prompt = self.prompt_builder.get_prediction_prompt(current_data, all_stats)
            raw = llm_client.invoke_with_prompt(prompt, temperature=0.3)

            result = extract_json(raw)
            result["department"] = dept
            result.setdefault("historical_context", [])
            return result
        except Exception as e:
            fallback = self._predict_traditional(dept, data)
            fallback["interpretation"] += f" (LLM 失败，fallback: {str(e)})"
            return fallback
