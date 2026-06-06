"""
图表路由
"""
import os
import time
from fastapi import APIRouter, Depends
from fastapi.responses import FileResponse
from core.security import verify_api_key
from core.config import settings
from core.response import ResponseBuilder
from services.data_loader import DataLoader
from services.analyzer import TraditionalAnalyzer
from charts import (
    generate_prediction_chart,
    generate_bed_utilization_chart,
    generate_anomaly_scatter_chart,
    generate_medicine_pie_chart,
)

router = APIRouter(prefix="/api/v2/charts", tags=["图表"])


@router.get("/{chart_type}", dependencies=[Depends(verify_api_key)])
def get_chart(chart_type: str):
    start = time.time()
    chart_dir = settings.rag.get("charts_output_dir", "./charts")
    chart_file = os.path.join(chart_dir, f"{chart_type}.png")

    if os.path.exists(chart_file):
        return FileResponse(chart_file, media_type="image/png")

    os.makedirs(chart_dir, exist_ok=True)

    try:
        if chart_type == "prediction":
            stats = DataLoader.load_monthly_stats(6)
            generate_prediction_chart(stats, chart_file)
        elif chart_type == "bed":
            beds = DataLoader.load_bed_info()
            hos = DataLoader.load_hospitalizations()
            generate_bed_utilization_chart(beds, hos, chart_file)
        elif chart_type == "anomaly":
            stats = DataLoader.load_monthly_stats(12)
            generate_anomaly_scatter_chart(stats, chart_file)
        elif chart_type == "medicine":
            meds = DataLoader.load_medicine_inventory()
            analysis = TraditionalAnalyzer.analyze_medicine_inventory(meds)
            generate_medicine_pie_chart(analysis, chart_file)
        else:
            return ResponseBuilder.error("不支持的图表类型", code=400, start_time=start)

        if os.path.exists(chart_file):
            return FileResponse(chart_file, media_type="image/png")
    except Exception as e:
        return ResponseBuilder.error(f"图表生成失败: {str(e)}", code=500, start_time=start)

    return ResponseBuilder.error("图表生成失败", code=500, start_time=start)
