"""
HIS AI 微服务 — 分析引擎
Holt-Winters 预测、Z-score 异常检测、床位优化、药品库存分析
"""
import math
import pandas as pd
from collections import defaultdict


def holt_winters_forecast(values, alpha=0.3, beta=0.1, horizon=1):
    """Holt-Winters 双指数平滑（level + trend），返回 (forecast, level, trend)"""
    if len(values) < 2:
        return None
    level = values[0]
    trend = values[1] - values[0]
    for i in range(1, len(values)):
        new_level = alpha * values[i] + (1 - alpha) * (level + trend)
        new_trend = beta * (new_level - level) + (1 - beta) * trend
        level, trend = new_level, new_trend
    forecast = level + horizon * trend
    return max(0, forecast), level, trend


def zscore_anomaly_detect(values_dict, threshold=2.0):
    """Z-score 异常检测，返回异常列表"""
    if len(values_dict) < 3:
        return []
    values = list(values_dict.values())
    mean = sum(values) / len(values)
    variance = sum((v - mean) ** 2 for v in values) / len(values)
    stddev = math.sqrt(variance)
    if stddev < 1e-6:
        return []
    anomalies = []
    for label, value in values_dict.items():
        z = (value - mean) / stddev
        if abs(z) > threshold:
            anomalies.append({
                'label': label, 'value': value, 'z_score': round(z, 2),
                'mean': round(mean, 1), 'stddev': round(stddev, 1),
                'direction': 'high' if z > 0 else 'low'
            })
    return anomalies


def predict_demand(monthly_stats):
    """按科室预测下月需求（3月移动平均 + 线性趋势 + Holt-Winters）"""
    dept_data = defaultdict(list)
    for s in monthly_stats:
        dept_data[s['department']].append((s['month'], s['new_admissions']))

    predictions = []
    for dept, data in dept_data.items():
        data.sort()
        values = [d[1] for d in data]
        n = len(values)
        pred = {'department': dept, 'data_points': n,
                'moving_avg_3m': 0.0, 'holt_winters': 0.0,
                'linear_trend': 0.0, 'predicted_next_month': 0, 'growth_rate': 0.0}
        if n == 0:
            predictions.append(pred)
            continue
        ma_count = min(n, 3)
        pred['moving_avg_3m'] = round(sum(values[-ma_count:]) / ma_count, 1)
        if n >= 2:
            mean_x = (n - 1) / 2.0
            mean_y = sum(values) / n
            num = sum((i - mean_x) * (values[i] - mean_y) for i in range(n))
            den = sum((i - mean_x) ** 2 for i in range(n))
            pred['linear_trend'] = round(num / den if den else 0, 2)
            if values[-2] > 0:
                pred['growth_rate'] = round((values[-1] - values[-2]) * 100.0 / values[-2], 1)
        hw = holt_winters_forecast(values)
        if hw:
            pred['holt_winters'] = round(hw[0], 1)
            pred['predicted_next_month'] = max(0, round(hw[0] if n >= 6 else
                                            (pred['moving_avg_3m'] + pred['linear_trend'])))
        else:
            pred['predicted_next_month'] = max(0, round(pred['moving_avg_3m']))
        predictions.append(pred)
    return predictions


def detect_anomalies(monthly_stats, threshold=2.0):
    """按科室检测入院人数异常"""
    dept_data = defaultdict(dict)
    for s in monthly_stats:
        dept_data[s['department']][s['month']] = s['new_admissions']
    all_anomalies = []
    for dept, month_values in dept_data.items():
        for a in zscore_anomaly_detect(month_values, threshold):
            all_anomalies.append({
                'department': dept, **a,
                'description': f"[{dept}] {a['label']} 入院{a['value']}人次异常 "
                               f"(Z={a['z_score']}, 均值{a['mean']}±{a['stddev']}) — "
                               f"{'显著偏高' if a['direction'] == 'high' else '显著偏低'}"
            })
    return all_anomalies


def analyze_bed_allocation(beds, hospitalizations, predictions=None):
    """
    床位分配分析 — 对应课程要求 (4)
    按科室统计床位总数、占用数、使用率、平均住院天数，给出优化建议
    """
    from datetime import datetime

    # 按科室统计床位
    dept_beds = defaultdict(lambda: {'total': 0, 'occupied': 0, 'cleaning': 0, 'available': 0})
    for b in beds:
        d = dept_beds[b['department']]
        d['total'] += 1
        status = b.get('status', 0)
        if status == 1:    # OCCUPIED
            d['occupied'] += 1
        elif status == 2:  # CLEANING
            d['cleaning'] += 1
        else:
            d['available'] += 1

    # 按科室统计住院天数
    dept_stays = defaultdict(list)
    for h in hospitalizations:
        dept = h['department']
        admit = str(h.get('admit_time', '')) if h.get('admit_time') else ''
        discharge = str(h.get('discharge_time', '')) if h.get('discharge_time') else ''
        if admit and admit != 'None' and admit != '#' and discharge and discharge != 'None' and discharge != '#':
            try:
                a = datetime.strptime(admit[:10], '%Y-%m-%d')
                d = datetime.strptime(discharge[:10], '%Y-%m-%d')
                days = max(1, (d - a).days)
                dept_stays[dept].append(days)
            except ValueError:
                continue

    # 构建预测查找表
    pred_map = {}
    if predictions:
        for p in predictions:
            pred_map[p['department']] = p.get('predicted_next_month', 0)

    result = []
    for dept, bd in dept_beds.items():
        total = bd['total']
        occupied = bd['occupied']
        util_pct = round(occupied / total * 100, 1) if total > 0 else 0.0
        stays = dept_stays.get(dept, [])
        avg_stay = round(sum(stays) / len(stays), 1) if stays else 0.0
        predicted = pred_map.get(dept, 0)
        recommended = max(total, int(math.ceil(predicted * max(avg_stay, 1) / 30 * 1.2))) if predicted > 0 else total

        result.append({
            'department': dept,
            'total_beds': total,
            'occupied': occupied,
            'available': bd['available'],
            'cleaning': bd['cleaning'],
            'utilization_pct': util_pct,
            'avg_stay_days': avg_stay,
            'predicted_admissions': predicted,
            'recommended_beds': recommended,
            'status': 'high' if util_pct > 85 else ('low' if util_pct < 50 else 'normal')
        })

    result.sort(key=lambda x: x['utilization_pct'], reverse=True)
    return result


def suggest_bed_reallocation(bed_analyses):
    """生成床位调配建议：高负载科室调入，低负载科室调出"""
    suggestions = []
    high = [b for b in bed_analyses if b['status'] == 'high']
    low = [b for b in bed_analyses if b['status'] == 'low']

    for h in high:
        for l in low:
            deficit = max(0, h['recommended_beds'] - h['total_beds'])
            surplus = max(0, l['total_beds'] - l['recommended_beds'])
            if deficit > 0 and surplus > 0:
                transfer = min(deficit, surplus)
                suggestions.append({
                    'from_dept': l['department'],
                    'to_dept': h['department'],
                    'transfer_beds': transfer,
                    'reason': f"{l['department']}利用率仅{l['utilization_pct']}%（低负载），"
                              f"{h['department']}利用率达{h['utilization_pct']}%（高负载）"
                })
    return suggestions


def analyze_medicine_inventory(medicines):
    """分析药品库存状态，使用 pandas 进行数据透视"""
    records = []
    for m in medicines:
        records.append({
            'department': m.get('department', ''),
            'name': m.get('name', ''),
            'stock': int(m.get('stock', 0)),
            'safety_stock': int(m.get('safety_stock', 0)),
            'sale_price_cents': int(m.get('sale_price_cents', 0)),
            'purchase_price_cents': int(m.get('purchase_price_cents', 0)),
            'expiry_date': str(m.get('expiry_date', '')),
            'status': int(m.get('status', 1)),
            'is_special': bool(m.get('is_special', 0)),
        })

    df = pd.DataFrame(records) if records else pd.DataFrame()

    # pandas 透视：按科室汇总
    if not df.empty:
        pivot = df.groupby('department').agg(
            total_types=('name', 'count'),
            total_stock=('stock', 'sum'),
            total_value_yuan=('sale_price_cents', lambda x: x.sum() / 100.0),
            low_stock_count=('stock', lambda x: (x <= df.loc[x.index, 'safety_stock']).sum()),
            special_count=('is_special', 'sum'),
        ).reset_index()
        dept_summary = pivot.to_dict(orient='records')
    else:
        dept_summary = []

    # 低库存和过期明细
    low_stock = []
    expired = []
    from datetime import date
    today = date.today().isoformat()

    for m in medicines:
        stock = int(m.get('stock', 0))
        safety = int(m.get('safety_stock', 0))
        if stock <= safety:
            low_stock.append({'id': m.get('medicine_id', ''), 'name': m.get('name', ''),
                              'stock': stock, 'safety_stock': safety})
        expiry = str(m.get('expiry_date', ''))
        if expiry and expiry != '#' and expiry != 'None' and expiry < today:
            expired.append({'id': m.get('medicine_id', ''), 'name': m.get('name', ''),
                            'expiry_date': expiry})

    return {
        'dept_summary': dept_summary,
        'low_stock_items': low_stock,
        'expired_items': expired,
        'total_types': len(medicines),
        'total_stock': sum(int(m.get('stock', 0)) for m in medicines),
    }
