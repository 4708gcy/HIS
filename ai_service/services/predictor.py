"""需求预测引擎"""
import time
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder


class Predictor:
    """预测引擎：始终用传统统计算法，可选 LLM 写总结"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()

    def predict(self, months=6, department=None, strategy="auto"):
        start = time.time()

        stats = DataLoader.load_monthly_stats(months, department)
        if not stats:
            return {"error": "无可用数据"}

        dept_data = {}
        for s in stats:
            dept = s["department"]
            if dept not in dept_data:
                dept_data[dept] = []
            dept_data[dept].append(s)

        # 始终用传统算法计算预测值
        results = []
        for dept, data in dept_data.items():
            result = self._predict_traditional(dept, data)
            results.append(result)

        # llm/auto 策略：额外调用 LLM 写趋势总结
        if strategy in ("llm", "auto"):
            summary = self._summarize_predictions(results)
            for r in results:
                r["interpretation"] += f"\n\n【AI 分析】{summary}"

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

    def _summarize_predictions(self, predictions):
        """让 LLM 对传统预测结果写自然语言总结"""
        try:
            prompt = self.prompt_builder.get_prediction_summary_prompt(predictions)
            return llm_client.invoke_with_prompt(prompt, temperature=0.3)
        except Exception as e:
            return f"总结生成失败: {str(e)}"
