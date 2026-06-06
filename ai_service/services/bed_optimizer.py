"""
床位优化引擎（LangChain 版）
"""
import json
import time
from typing import Dict, Any
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder


class BedOptimizer:
    """床位优化引擎：传统计算 + LangChain LLM 报告"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()

    def optimize(self, department: str = None, strategy: str = "auto") -> Dict[str, Any]:
        start_time = time.time()

        beds = DataLoader.load_bed_info(department)
        hospitalizations = DataLoader.load_hospitalizations(department)

        if not beds:
            return {"error": "无床位数据"}

        traditional_result = TraditionalAnalyzer.analyze_bed_allocation(beds, hospitalizations)

        if strategy in ("llm", "auto"):
            for item in traditional_result:
                item["report"] = self._llm_report(item)

        return {
            "strategy": strategy,
            "departments": traditional_result,
            "processing_time": round(time.time() - start_time, 3)
        }

    def _llm_report(self, bed_data: Dict[str, Any]) -> str:
        """LangChain LLM 生成床位优化报告"""
        try:
            prompt = self.prompt_builder.get_bed_prompt(bed_data)
            raw = llm_client.invoke_with_prompt(prompt, temperature=0.3)

            if "```json" in raw:
                raw = raw.split("```json")[1].split("```")[0].strip()
            elif "```" in raw:
                raw = raw.split("```")[1].split("```")[0].strip()

            result = json.loads(raw)
            return result.get("assessment", "") + "\n建议：" + "；".join(result.get("suggestions", []))
        except Exception as e:
            return f"报告生成失败: {str(e)}"
