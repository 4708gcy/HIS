"""
HIS AI 微服务 — Flask REST API v2.0
提供数据分析、需求预测、异常检测、床位优化、图表生成的 HTTP 接口

启动方式:
    cd ai_service
    pip install -r requirements.txt
    python app.py

接口列表:
    GET  /api/health                  — 健康检查
    GET  /api/stats/monthly           — 月度统计
    GET  /api/predictions             — 需求预测（Holt-Winters + 线性趋势）
    GET  /api/anomalies               — Z-score 异常检测
    GET  /api/medicines               — 药品库存分析（pandas 透视）
    GET  /api/bed-optimization        — 床位分配分析与优化建议
    POST /api/forecast                — 自定义预测参数
    GET  /api/dashboard               — 综合仪表盘
    GET  /api/charts/predictions      — 预测对比柱状图 PNG
    GET  /api/charts/bed-utilization  — 床位利用率图表 PNG
    GET  /api/charts/anomalies        — 异常检测时序图 PNG
    GET  /api/charts/medicines        — 药品库存状态饼图 PNG
"""
from flask import Flask, jsonify, request, send_file
from flask_cors import CORS
import os
from functools import wraps

API_KEY = os.environ.get('HIS_API_KEY', 'his-default-key-change-me')

def require_api_key(f):
    """API Key 认证装饰器。请求头中必须包含 X-API-Key。"""
    @wraps(f)
    def decorated(*args, **kwargs):
        key = request.headers.get('X-API-Key', '')
        if key != API_KEY:
            return jsonify({'error': 'Unauthorized — 缺少有效的 API Key'}), 401
        return f(*args, **kwargs)
    return decorated

from data_loader import (
    load_hospitalizations, load_registrations, load_medicines, load_beds,
    extract_monthly_stats, load_doctors, load_consultations
)
from analyzer import (
    predict_demand, detect_anomalies, analyze_medicine_inventory,
    analyze_bed_allocation, suggest_bed_reallocation
)

app = Flask(__name__)
CORS(app)

# 启动时预加载数据（缓存 60 秒，避免每次请求重复连接）
from functools import lru_cache
import time


def _cache_key():
    return int(time.time() / 60)  # 每分钟刷新


_cached_stats = (0, None)
_cached_predictions = (0, None)
_cached_beds = (0, None)
_cached_hos = (0, None)


def get_monthly_stats():
    global _cached_stats
    key = _cache_key()
    if _cached_stats[0] != key:
        _cached_stats = (key, extract_monthly_stats())
    return _cached_stats[1]


def get_predictions():
    global _cached_predictions
    key = _cache_key()
    if _cached_predictions[0] != key:
        _cached_predictions = (key, predict_demand(get_monthly_stats()))
    return _cached_predictions[1]


def get_bed_data():
    global _cached_beds, _cached_hos
    key = _cache_key()
    if _cached_beds[0] != key:
        _cached_beds = (key, load_beds())
        _cached_hos = (key, load_hospitalizations())
    return _cached_beds[1], _cached_hos[1]


# ==================== 数据 API ====================

@app.route('/api/health')
def health():
    return jsonify({'status': 'ok', 'service': 'HIS AI Service v2.0',
                    'version': '2.0', 'features': ['mysql', 'pandas', 'matplotlib']})


@app.route('/api/stats/monthly')
@require_api_key
def monthly_stats():
    stats = get_monthly_stats()
    return jsonify({'count': len(stats), 'data': stats})


@app.route('/api/predictions')
@require_api_key
def predictions():
    preds = get_predictions()
    return jsonify({'count': len(preds), 'data': preds})


@app.route('/api/anomalies')
@require_api_key
def anomalies():
    threshold = request.args.get('threshold', 2.0, type=float)
    stats = get_monthly_stats()
    result = detect_anomalies(stats, threshold)
    return jsonify({'count': len(result), 'threshold': threshold, 'data': result})


@app.route('/api/medicines')
@require_api_key
def medicines_analysis():
    meds = load_medicines()
    result = analyze_medicine_inventory(meds)
    return jsonify(result)


@app.route('/api/bed-optimization')
@require_api_key
def bed_optimization():
    """床位分配分析与优化建议 — 课程要求 (4) 核心功能"""
    beds, hospitalizations = get_bed_data()
    preds = get_predictions()

    bed_analysis = analyze_bed_allocation(beds, hospitalizations, preds)
    suggestions = suggest_bed_reallocation(bed_analysis)

    # 利用 pandas 构建优化前后对比表
    import pandas as pd
    compare = pd.DataFrame(bed_analysis)
    if not compare.empty:
        summary = {
            'overall_utilization_pct': round(
                compare['occupied'].sum() / compare['total_beds'].sum() * 100, 1
            ) if compare['total_beds'].sum() > 0 else 0,
            'total_beds': int(compare['total_beds'].sum()),
            'total_occupied': int(compare['occupied'].sum()),
            'high_load_depts': compare[compare['status'] == 'high']['department'].tolist(),
            'low_load_depts': compare[compare['status'] == 'low']['department'].tolist(),
        }
    else:
        summary = {}

    return jsonify({
        'bed_analysis': bed_analysis,
        'reallocation_suggestions': suggestions,
        'summary': summary,
    })


@app.route('/api/forecast', methods=['POST'])
@require_api_key
def forecast():
    from analyzer import holt_winters_forecast
    params = request.get_json(silent=True) or {}
    alpha = params.get('alpha', 0.3)
    beta = params.get('beta', 0.1)
    horizon = params.get('horizon', 1)
    department = params.get('department')

    stats = get_monthly_stats()
    dept_data = defaultdict(lambda: [])
    for s in stats:
        dept = s['department']
        if department and dept != department:
            continue
        dept_data[dept].append((s['month'], s['new_admissions']))

    results = []
    for dept, data in dept_data.items():
        data.sort()
        values = [d[1] for d in data]
        hw = holt_winters_forecast(values, alpha, beta, horizon)
        results.append({
            'department': dept, 'data_points': len(values),
            'forecast': round(hw[0], 1) if hw else None,
            'level': round(hw[1], 1) if hw else None,
            'trend': round(hw[2], 1) if hw else None,
            'alpha': alpha, 'beta': beta, 'horizon': horizon,
        })

    return jsonify({
        'count': len(results),
        'params': {'alpha': alpha, 'beta': beta, 'horizon': horizon},
        'data': results,
    })


@app.route('/api/dashboard')
@require_api_key
def dashboard():
    """综合仪表盘"""
    hos = load_hospitalizations()
    regs = load_registrations()
    meds = load_medicines()
    stats = get_monthly_stats()

    preds = get_predictions()
    anoms = detect_anomalies(stats)
    med_analysis = analyze_medicine_inventory(meds)
    beds, _ = get_bed_data()
    bed_analysis = analyze_bed_allocation(beds, hos, preds)

    return jsonify({
        'summary': {
            'total_patients': len(load_registrations()),  # approximate
            'total_admitted': sum(1 for h in hos if h.get('status') == 2),
            'total_beds': sum(b['total_beds'] for b in bed_analysis),
            'overall_utilization_pct': round(
                sum(b['occupied'] for b in bed_analysis) /
                max(1, sum(b['total_beds'] for b in bed_analysis)) * 100, 1
            ),
            'total_medicine_types': med_analysis.get('total_types', 0),
            'low_stock_count': len(med_analysis.get('low_stock_items', [])),
        },
        'predictions': preds,
        'anomalies': anoms,
        'medicine_analysis': med_analysis,
        'bed_analysis': bed_analysis,
        'monthly_stats': stats,
    })


# ==================== 图表 API（课程要求 (4)"多种形式展示"） ====================

@app.route('/api/charts/predictions')
@require_api_key
def chart_predictions():
    """生成需求预测对比柱状图 PNG"""
    from charts import generate_prediction_chart
    preds = get_predictions()
    path = generate_prediction_chart(preds)
    return send_file(path, mimetype='image/png')


@app.route('/api/charts/bed-utilization')
@require_api_key
def chart_bed_utilization():
    """生成床位利用率图表 PNG（柱状图 + 堆叠图）"""
    from charts import generate_bed_utilization_chart
    beds, hospitalizations = get_bed_data()
    preds = get_predictions()
    bed_analysis = analyze_bed_allocation(beds, hospitalizations, preds)
    path = generate_bed_utilization_chart(bed_analysis)
    return send_file(path, mimetype='image/png')


@app.route('/api/charts/anomalies')
@require_api_key
def chart_anomalies():
    """生成异常检测时序折线图 PNG"""
    from charts import generate_anomaly_scatter_chart
    stats = get_monthly_stats()
    anoms = detect_anomalies(stats)
    path = generate_anomaly_scatter_chart(stats, anoms)
    if path is None:
        return jsonify({'error': 'No data to chart'}), 404
    return send_file(path, mimetype='image/png')


@app.route('/api/charts/medicines')
@require_api_key
def chart_medicines():
    """生成药品库存状态饼图 PNG"""
    from charts import generate_medicine_pie_chart
    meds = load_medicines()
    analysis = analyze_medicine_inventory(meds)  # 返回包含 low_stock_items/expired_items/total_types 的字典
    path = generate_medicine_pie_chart(analysis)
    if path is None:
        return jsonify({'error': 'No data to chart'}), 404
    return send_file(path, mimetype='image/png')


if __name__ == '__main__':
    print("=" * 50)
    print("  HIS AI 微服务 v2.0 启动中...")
    print("  数据源: MySQL (his_db)")
    print("  可视化: matplotlib + pandas")
    print(f"  API 文档: http://localhost:5001/api/health")
    print(f"  图表接口: http://localhost:5001/api/charts/predictions")
    print("=" * 50)
    app.run(host='127.0.0.1', port=5001, debug=False)
