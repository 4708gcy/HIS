"""Prompt 构建器"""
import json
from decimal import Decimal
from langchain_core.prompts import ChatPromptTemplate


class _DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super().default(obj)


class PromptBuilder:

    def get_prediction_summary_prompt(self, predictions):
        """根据传统算法的预测结果，让 LLM 写自然语言总结"""
        pred_text = "\n".join([
            f"- {p['department']}：预测下月入院 {p['predicted_next_month']} 人，"
            f"增长率 {p['growth_rate']}%，置信度 {p['confidence']}"
            for p in predictions
        ])

        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院运营管理分析师。根据统计算法的预测结果，用自然语言写一段简洁的趋势总结。"),
            ("human", """## 统计算法预测结果
{predictions}

## 要求
1. 用中文输出，控制在 150 字以内
2. 总结各科室的需求趋势（上升/下降/平稳）
3. 指出需要关注的科室
4. 给出 1-2 条资源调配建议""")
        ])
        return template.invoke({"predictions": pred_text})

    def get_anomaly_prompt(self, anomaly_data):
        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院数据分析师，用简洁专业的语言解读数据异常。"),
            ("human", """## 异常信息
科室：{dept}
月份：{month}
实际入院人数：{value} 人
历史均值：{mean} 人
标准差：{stddev}
Z-score：{z_score}（{direction}）

## 任务
分析该异常可能的原因，并给出 2-3 条具体建议。用自然语言输出，控制在 100 字以内。""")
        ])
        return template.invoke({
            "dept": anomaly_data.get("department", ""),
            "month": anomaly_data.get("month", ""),
            "value": anomaly_data.get("value", 0),
            "mean": anomaly_data.get("mean", 0),
            "stddev": anomaly_data.get("stddev", 0),
            "z_score": anomaly_data.get("z_score", 0),
            "direction": "显著偏高" if anomaly_data.get("direction") == "high" else "显著偏低"
        })

    def get_bed_prompt(self, bed_data):
        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院床位管理专家，只输出 JSON，不要其他内容。"),
            ("human", """## 当前床位数据
科室：{dept}
总床位：{total}
已占用：{occupied}
空闲：{available}
清洁中：{cleaning}
利用率：{utilization}%
平均住院天数：{avg_stay} 天

## 任务
1. 评估当前床位使用状况
2. 给出 2-3 条具体优化建议
3. 预测未来一周床位紧张程度

请用 JSON 格式输出：
{{
    "assessment": "总体评估（50字内）",
    "suggestions": ["建议1", "建议2", "建议3"],
    "forecast": "未来一周预测"
}}""")
        ])
        return template.invoke({
            "dept": bed_data.get("department", ""),
            "total": bed_data.get("total_beds", 0),
            "occupied": bed_data.get("occupied", 0),
            "available": bed_data.get("available", 0),
            "cleaning": bed_data.get("cleaning", 0),
            "utilization": bed_data.get("utilization_rate", 0),
            "avg_stay": bed_data.get("avg_stay_days", 0)
        })

    def get_dashboard_prompt(self, summary_data):
        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院运营总监的 AI 助手，请基于数据生成运营摘要。"),
            ("human", """## 数据概览
{data}

## 要求
1. 用中文输出
2. 控制在 200 字以内
3. 突出重点问题和改进方向
4. 语气专业、简洁""")
        ])
        return template.invoke({
            "data": json.dumps(summary_data, ensure_ascii=False, indent=2, cls=_DecimalEncoder)
        })
