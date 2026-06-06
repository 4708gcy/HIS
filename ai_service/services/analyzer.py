"""传统统计算法"""
from typing import List, Dict, Any
from collections import defaultdict


class TraditionalAnalyzer:
    """传统统计分析器"""

    @staticmethod
    def holt_winters_forecast(values: List[float], alpha: float = 0.3, beta: float = 0.1):
        """Holt-Winters 双指数平滑预测"""
        n = len(values)
        if n < 3:
            return None

        level = values[0]
        trend = values[1] - values[0]

        for i in range(1, n):
            prev_level = level
            level = alpha * values[i] + (1 - alpha) * (level + trend)
            trend = beta * (level - prev_level) + (1 - beta) * trend

        forecast = level + trend
        return forecast

    @staticmethod
    def zscore_anomaly_detect(month_values: Dict[str, int], threshold: float = 2.0):
        """Z-score 异常检测"""
        values = list(month_values.values())
        if len(values) < 3:
            return []

        mean = sum(values) / len(values)
        std = (sum((x - mean) ** 2 for x in values) / len(values)) ** 0.5
        if std == 0:
            return []

        anomalies = []
        for month, value in month_values.items():
            z = (value - mean) / std
            if abs(z) >= threshold:
                anomalies.append({
                    "month": month,
                    "value": value,
                    "z_score": round(z, 2),
                    "mean": round(mean, 1),
                    "stddev": round(std, 1),
                    "direction": "high" if z > 0 else "low"
                })
        return anomalies

    @staticmethod
    def analyze_bed_allocation(beds: List[Dict], hospitalizations: List[Dict]):
        """床位分配分析"""
        from datetime import datetime

        dept_beds = defaultdict(lambda: {"total": 0, "occupied": 0, "cleaning": 0, "available": 0})
        for b in beds:
            d = dept_beds[b["department"]]
            d["total"] += 1
            status = b.get("status", 0)
            if status == 1:
                d["occupied"] += 1
            elif status == 2:
                d["cleaning"] += 1
            else:
                d["available"] += 1

        dept_stays = defaultdict(list)
        for h in hospitalizations:
            dept = h["department"]
            admit = h.get("admit_time", "")
            discharge = h.get("discharge_time", "")
            if admit and discharge and str(admit) not in ("None", "#") and str(discharge) not in ("None", "#"):
                try:
                    a = datetime.strptime(str(admit)[:10], "%Y-%m-%d")
                    d = datetime.strptime(str(discharge)[:10], "%Y-%m-%d")
                    dept_stays[dept].append(max(1, (d - a).days))
                except (ValueError, TypeError):
                    pass

        result = []
        for dept, info in dept_beds.items():
            avg_stay = sum(dept_stays.get(dept, [0])) / max(len(dept_stays.get(dept, [])), 1)
            utilization = info["occupied"] / max(info["total"], 1) * 100
            result.append({
                "department": dept,
                "total_beds": info["total"],
                "occupied": info["occupied"],
                "available": info["available"],
                "cleaning": info["cleaning"],
                "utilization_rate": round(utilization, 1),
                "avg_stay_days": round(avg_stay, 1)
            })
        return result

    @staticmethod
    def analyze_medicine_inventory(medicines: List[Dict]):
        """药品库存分析"""
        low_stock = [m for m in medicines if m.get("stock", 0) <= m.get("safetyStock", 0)]
        expired = [m for m in medicines if m.get("isExpired", False)]
        return {
            "total_types": len(medicines),
            "low_stock_items": low_stock,
            "expired_items": expired,
            "total_normal": max(0, len(medicines) - len(low_stock) - len(expired))
        }
