"""床位优化引擎"""
import json
import re
import time
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder


def _extract_json(raw):
    """从 LLM 输出提取 JSON"""
    match = re.search(r'```(?:json)?\s*\n?(.*?)```', raw, re.DOTALL)
    text = match.group(1).strip() if match else raw.strip()
    return json.loads(text)


class BedOptimizer:
    """传统计算 + LLM 报告"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()

    def optimize(self, department=None, strategy="auto"):
        start = time.time()

        beds = DataLoader.load_bed_info(department)
        hospitalizations = DataLoader.load_hospitalizations(department)

        if not beds:
            return {"error": "无床位数据"}

        results = TraditionalAnalyzer.analyze_bed_allocation(beds, hospitalizations)

        if strategy in ("llm", "auto"):
            for item in results:
                item["report"] = self._llm_report(item)

        return {
            "strategy": strategy,
            "departments": results,
            "processing_time": round(time.time() - start, 3)
        }

    def _llm_report(self, bed_data):
        try:
            prompt = self.prompt_builder.get_bed_prompt(bed_data)
            raw = llm_client.invoke_with_prompt(prompt, temperature=0.3)

            result = _extract_json(raw)
            return result.get("assessment", "") + "\n建议：" + "；".join(result.get("suggestions", []))
        except Exception as e:
            return f"报告生成失败: {str(e)}"
