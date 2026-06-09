"""
图表路由
"""
import os
import time
from fastapi import APIRouter, Depends
from fastapi.responses import FileResponse, HTMLResponse
from core.security import verify_api_key
from core.response import ResponseBuilder
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from charts import (
    CHARTS_DIR,
    generate_prediction_chart,
    generate_bed_utilization_chart,
    generate_anomaly_scatter_chart,
    generate_medicine_pie_chart,
)

router = APIRouter(prefix="/api/v2/charts", tags=["图表"])

CHART_TYPES = ["prediction", "bed", "anomaly", "medicine"]
CHART_LABELS = {"prediction": "需求预测", "bed": "床位使用率", "anomaly": "异常检测", "medicine": "药品库存"}


def _ensure_chart(chart_type: str):
    """确保图表已生成，返回文件路径"""
    chart_file = f"{chart_type}.png"
    save_path = os.path.join(CHARTS_DIR, chart_file)
    if os.path.exists(save_path):
        return save_path

    os.makedirs(CHARTS_DIR, exist_ok=True)
    if chart_type == "prediction":
        stats = DataLoader.load_monthly_stats(6)
        generate_prediction_chart(stats, chart_file)
    elif chart_type == "bed":
        beds = DataLoader.load_bed_info()
        hos = DataLoader.load_hospitalizations()
        bed_analysis = TraditionalAnalyzer.analyze_bed_allocation(beds, hos)
        generate_bed_utilization_chart(bed_analysis, chart_file)
    elif chart_type == "anomaly":
        stats = DataLoader.load_monthly_stats(12)
        generate_anomaly_scatter_chart(stats, None, chart_file)
    elif chart_type == "medicine":
        meds = DataLoader.load_medicine_inventory()
        analysis = TraditionalAnalyzer.analyze_medicine_inventory(meds)
        generate_medicine_pie_chart(analysis, chart_file)
    else:
        return None
    return save_path if os.path.exists(save_path) else None


@router.get("/view")
def chart_dashboard():
    """图表仪表盘 HTML 页面（浏览器直接访问，无需认证）"""
    # 预生成所有图表
    for t in CHART_TYPES:
        _ensure_chart(t)

    cards = ""
    for t in CHART_TYPES:
        cards += f"""
        <div class="card">
            <h2>{CHART_LABELS[t]}</h2>
            <img src="/api/v2/charts/img/{t}" alt="{CHART_LABELS[t]}" />
        </div>"""

    html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <title>HIS 数据分析图表</title>
    <style>
        * {{ margin: 0; padding: 0; box-sizing: border-box; }}
        body {{ background: #1a1a2e; color: #eee; font-family: 'Microsoft YaHei', sans-serif; padding: 20px; }}
        h1 {{ text-align: center; padding: 20px; color: #00d4ff; }}
        .grid {{ display: grid; grid-template-columns: repeat(2, 1fr); gap: 20px; max-width: 1400px; margin: 0 auto; }}
        .card {{ background: #16213e; border-radius: 12px; padding: 16px; box-shadow: 0 4px 20px rgba(0,0,0,0.3); }}
        .card h2 {{ text-align: center; margin-bottom: 12px; color: #00d4ff; font-size: 18px; }}
        .card img {{ width: 100%; border-radius: 8px; }}
        .refresh {{ text-align: center; margin: 20px; }}
        .refresh a {{ color: #00d4ff; text-decoration: none; font-size: 14px; }}
    </style>
</head>
<body>
    <h1>HIS 医院信息系统 — 数据分析图表</h1>
    <div class="refresh"><a href="/api/v2/charts/view">🔄 刷新图表</a></div>
    <div class="grid">{cards}</div>
</body>
</html>"""
    return HTMLResponse(content=html)


@router.get("/img/{chart_type}")
def get_chart_image(chart_type: str):
    """图表图片（无须认证，供 HTML 页面引用）"""
    save_path = _ensure_chart(chart_type)
    if save_path:
        return FileResponse(save_path, media_type="image/png")
    return ResponseBuilder.error("不支持的图表类型", code=400)


@router.get("/{chart_type}", dependencies=[Depends(verify_api_key)])
def get_chart(chart_type: str):
    """C++ 客户端下载图表（需认证）"""
    start = time.time()
    save_path = _ensure_chart(chart_type)
    if save_path:
        return FileResponse(save_path, media_type="image/png")
    if chart_type not in CHART_TYPES:
        return ResponseBuilder.error("不支持的图表类型", code=400, start_time=start)
    return ResponseBuilder.error("图表生成失败", code=500, start_time=start)
