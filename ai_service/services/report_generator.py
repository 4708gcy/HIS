"""综合报告生成器"""
import time
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from services.llm_client import llm_client
from services.prompt_builder import PromptBuilder


class ReportGenerator:
    """聚合数据 + LLM 生成运营摘要"""

    def __init__(self):
        self.prompt_builder = PromptBuilder()

    def generate(self, strategy="llm"):
        start = time.time()

        monthly_stats = DataLoader.load_monthly_stats(6)
        beds = DataLoader.load_bed_info()
        hospitalizations = DataLoader.load_hospitalizations()
        medicines = DataLoader.load_medicine_inventory()
        doctors = DataLoader.load_doctor_workload()
        patients = DataLoader.load_patient_visits()

        bed_analysis = TraditionalAnalyzer.analyze_bed_allocation(beds, hospitalizations)
        medicine_analysis = TraditionalAnalyzer.analyze_medicine_inventory(medicines)

        summary = {
            "stats_count": len(monthly_stats),
            "departments": list(set([s["department"] for s in monthly_stats])),
            "bed_summary": bed_analysis,
            "medicine_summary": {
                "total_types": medicine_analysis["total_types"],
                "low_stock_count": len(medicine_analysis["low_stock_items"]),
                "expired_count": len(medicine_analysis["expired_items"])
            },
            "doctor_summary": doctors,
            "patient_summary": patients
        }

        if strategy == "llm":
            summary["executive_summary"] = self._llm_summary(summary)
        else:
            summary["executive_summary"] = "（传统模式不生成 LLM 摘要）"

        summary["processing_time"] = round(time.time() - start, 3)
        return summary

    def _llm_summary(self, summary_data):
        try:
            prompt = self.prompt_builder.get_dashboard_prompt(summary_data)
            return llm_client.invoke_with_prompt(prompt, temperature=0.5)
        except Exception as e:
            return f"摘要生成失败: {str(e)}"
