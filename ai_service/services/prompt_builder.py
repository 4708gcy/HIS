"""
动态 Few-shot Prompt 构建器（LangChain 版）
参考来源：02_LangChain_2_提示词模板.py（ChatPromptTemplate.from_messages）
"""
import json
from typing import List, Dict, Any
import jieba
from sklearn.feature_extraction.text import TfidfVectorizer
from langchain_core.prompts import ChatPromptTemplate
from core.config import settings


class PromptBuilder:
    """Prompt 构建器：LangChain ChatPromptTemplate + 动态 Few-shot"""

    def __init__(self):
        self.top_k = settings.analysis.get("few_shot_topk", 5)
        self.vectorizer = TfidfVectorizer(tokenizer=jieba.lcut, lowercase=False)

    def _find_similar_cases(self, query_text: str, historical_cases: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """用 TF-IDF 从历史案例中找到与当前场景最相似的 top_k 条记录"""
        if not historical_cases or len(historical_cases) < 3:
            return historical_cases[:self.top_k]

        corpus = []
        for case in historical_cases:
            text = f"{case.get('department', '')} {case.get('month', '')} 入院{case.get('new_admissions', 0)}人"
            corpus.append(text)

        corpus.append(query_text)

        try:
            tfidf_matrix = self.vectorizer.fit_transform(corpus)
            query_vec = tfidf_matrix[-1]
            import numpy as np
            similarities = np.dot(tfidf_matrix[:-1], query_vec.T).toarray().flatten()
            top_indices = similarities.argsort()[::-1][:self.top_k]
            return [historical_cases[i] for i in top_indices]
        except Exception:
            return historical_cases[-self.top_k:]

    def _format_few_shot(self, cases: List[Dict[str, Any]]) -> str:
        """将案例格式化为 few-shot 文本"""
        lines = []
        for i, case in enumerate(cases, 1):
            lines.append(f"案例{i}：{case.get('department', '')} 在 {case.get('month', '')} 入院 {case.get('new_admissions', 0)} 人，后续趋势为 {case.get('trend', '平稳')}。")
        return "\n".join(lines)

    def get_prediction_prompt(self, current_data: Dict[str, Any], historical_cases: List[Dict[str, Any]]):
        """构建需求预测 prompt → 返回 ChatPromptValue"""
        query_text = f"{current_data.get('department', '')} " + " ".join(
            [f"{m['month']}入院{m['new_admissions']}人" for m in current_data.get('months_data', [])]
        )
        similar_cases = self._find_similar_cases(query_text, historical_cases)
        few_shots = self._format_few_shot(similar_cases)

        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院运营管理专家，擅长基于历史数据预测科室需求。只输出 JSON，不要其他内容。"),
            ("human", """## 历史相似场景（参考）
{few_shots}

## 当前数据
科室：{dept}
近{months}个月入院数据：
{data}

## 任务
1. 预测下一个月的入院人数（给出具体数字和置信度：高/中/低）
2. 分析增长趋势（增长率）
3. 用 1-2 句话给出运营建议

请用 JSON 格式输出：
{{
    "predicted_next_month": 数字,
    "growth_rate": 浮点数,
    "confidence": "高/中/低",
    "interpretation": "自然语言解读",
    "suggestion": "运营建议"
}}""")
        ])

        return template.invoke({
            "few_shots": few_shots,
            "dept": current_data.get("department", "未知科室"),
            "months": len(current_data.get("months_data", [])),
            "data": "\n".join([f"- {m['month']}: {m['new_admissions']} 人" for m in current_data.get("months_data", [])])
        })

    def get_anomaly_prompt(self, anomaly_data: Dict[str, Any]):
        """构建异常解读 prompt"""
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

    def get_bed_prompt(self, bed_data: Dict[str, Any]):
        """构建床位优化 prompt"""
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

    def get_dashboard_prompt(self, summary_data: Dict[str, Any]):
        """构建综合仪表盘摘要 prompt"""
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
            "data": json.dumps(summary_data, ensure_ascii=False, indent=2)
        })

    def get_rag_prompt(self, query: str, retrieved_docs: List[str]):
        """构建 RAG 问答 prompt"""
        context = "\n\n".join([f"资料{i+1}：{doc}" for i, doc in enumerate(retrieved_docs)])
        template = ChatPromptTemplate.from_messages([
            ("system", "你是医院运营管理专家，请结合给定资料回答用户问题。如果资料中没有相关信息，请回答'根据现有资料无法回答'。"),
            ("human", """## 资料
{context}

## 问题
{query}

## 要求
1. 回答要基于资料，不要编造
2. 如果有多条资料支持，请综合回答
3. 控制在 300 字以内""")
        ])
        return template.invoke({"context": context, "query": query})
