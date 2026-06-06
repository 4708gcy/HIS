"""
HIS AI 微服务 — 数据可视化模块
"""
import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np
from collections import defaultdict


def _setup_cn_font():
    """配置中文字体（Windows: SimHei/微软雅黑, fallback: sans-serif）"""
    cn_fonts = ['SimHei', 'Microsoft YaHei', 'WenQuanYi Micro Hei',
                'Noto Sans CJK SC', 'PingFang SC', 'sans-serif']
    available = {f.name for f in fm.fontManager.ttflist}
    for name in cn_fonts:
        if name in available:
            plt.rcParams['font.sans-serif'] = [name, 'sans-serif']
            plt.rcParams['axes.unicode_minus'] = False
            return name
    plt.rcParams['font.sans-serif'] = ['sans-serif']
    plt.rcParams['axes.unicode_minus'] = False
    return 'sans-serif'


CHARTS_DIR = os.path.join(os.path.dirname(__file__), 'charts')
os.makedirs(CHARTS_DIR, exist_ok=True)
_font_name = _setup_cn_font()


# predictions: list from predict_demand()
def generate_prediction_chart(predictions, filename='predictions.png'):
    depts = [p['department'] for p in predictions]
    hw_vals = [p.get('holt_winters', 0) for p in predictions]
    ma_vals = [p.get('moving_avg_3m', 0) for p in predictions]
    predicted = [p.get('predicted_next_month', 0) for p in predictions]

    x = np.arange(len(depts))
    width = 0.25

    fig, ax = plt.subplots(figsize=(10, 6))
    bars1 = ax.bar(x - width, ma_vals, width, label='3月移动平均', color='#90caf9')
    bars2 = ax.bar(x, hw_vals, width, label='Holt-Winters 预测', color='#42a5f5')
    bars3 = ax.bar(x + width, predicted, width, label='最终预测值', color='#1976d2')

    ax.set_xlabel('科室', fontsize=12)
    ax.set_ylabel('预测入院人数', fontsize=12)
    ax.set_title('各科室下月需求预测', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(depts)
    ax.legend(loc='upper right')
    ax.grid(axis='y', alpha=0.3)

    for bar in bars3:
        height = bar.get_height()
        if height > 0:
            ax.annotate(f'{int(height)}', xy=(bar.get_x() + bar.get_width() / 2, height),
                        xytext=(0, 3), textcoords="offset points", ha='center', fontsize=9)

    plt.tight_layout()
    path = os.path.join(CHARTS_DIR, filename)
    plt.savefig(path, dpi=150)
    plt.close()
    return path


# bed_analyses: [{department, total_beds, occupied, utilization_pct}]
def generate_bed_utilization_chart(bed_analyses, filename='bed_utilization.png'):
    depts = [b['department'] for b in bed_analyses if b.get('department')]
    utilization = [b.get('utilization_pct', 0) for b in bed_analyses if b.get('department')]
    occupied = [b.get('occupied', 0) for b in bed_analyses if b.get('department')]
    total = [b.get('total_beds', 1) for b in bed_analyses if b.get('department')]
    available = [t - o for t, o in zip(total, occupied)]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    colors = ['#4caf50' if u < 70 else '#ff9800' if u < 85 else '#f44336'
              for u in utilization]
    bars = ax1.bar(depts, utilization, color=colors)
    ax1.axhline(y=85, color='#f44336', linestyle='--', label='高负载线 (85%)')
    ax1.axhline(y=50, color='#ff9800', linestyle='--', label='低负载线 (50%)')
    ax1.set_ylabel('床位利用率 (%)', fontsize=12)
    ax1.set_title('各科室床位利用率', fontsize=14, fontweight='bold')
    ax1.legend(fontsize=9)
    ax1.set_ylim(0, 110)
    for bar, u in zip(bars, utilization):
        ax1.text(bar.get_x() + bar.get_width() / 2, bar.get_height() + 1,
                 f'{u:.1f}%', ha='center', fontsize=9)

    x = np.arange(len(depts))
    ax2.bar(x, occupied, label='已占用', color='#f44336')
    ax2.bar(x, available, bottom=occupied, label='空闲', color='#4caf50')
    ax2.set_xticks(x)
    ax2.set_xticklabels(depts)
    ax2.set_ylabel('床位数', fontsize=12)
    ax2.set_title('各科室床位占用/空闲分布', fontsize=14, fontweight='bold')
    ax2.legend(fontsize=9)
    for i, (o, t) in enumerate(zip(occupied, total)):
        ax2.text(i, t + 0.5, str(t), ha='center', fontsize=9)

    plt.tight_layout()
    path = os.path.join(CHARTS_DIR, filename)
    plt.savefig(path, dpi=150)
    plt.close()
    return path


def generate_anomaly_scatter_chart(monthly_stats, anomalies=None, filename='anomalies.png'):
    if anomalies is None:
        anomalies = []

    dept_data = defaultdict(list)
    for s in monthly_stats:
        dept_data[s['department']].append((s['month'], s['new_admissions']))

    anomaly_dept_months = set()
    for a in anomalies:
        if 'department' in a and 'label' in a:
            anomaly_dept_months.add((a['department'], a['label']))

    n_depts = len(dept_data)
    if n_depts == 0:
        return None

    fig, axes = plt.subplots(n_depts, 1, figsize=(12, 3 * n_depts), sharex=False)
    if n_depts == 1:
        axes = [axes]

    for ax, (dept, data) in zip(axes, sorted(dept_data.items())):
        data.sort()
        months = [d[0] for d in data]
        values = [d[1] for d in data]
        ax.plot(months, values, 'o-', color='#1976d2', linewidth=2, markersize=6, label='入院人数')

        for i, (m, v) in enumerate(zip(months, values)):
            if (dept, m) in anomaly_dept_months:
                ax.scatter(m, v, color='red', s=120, zorder=5, edgecolors='darkred', linewidths=1.5)
                ax.annotate(f'异常\nZ-score', xy=(i, v), xytext=(i, v + max(values) * 0.15),
                            ha='center', fontsize=8, color='red',
                            arrowprops=dict(arrowstyle='->', color='red', lw=1))

        ax.set_title(f'{dept} — 入院人数趋势', fontsize=13, fontweight='bold')
        ax.set_ylabel('入院人数', fontsize=11)
        ax.grid(axis='y', alpha=0.3)
        ax.legend(fontsize=9)
        plt.setp(ax.xaxis.get_majorticklabels(), rotation=45, ha='right', fontsize=8)

    plt.tight_layout()
    path = os.path.join(CHARTS_DIR, filename)
    plt.savefig(path, dpi=150)
    plt.close()
    return path


# stats_by_dept: [{department, registration_count, ...}]
def generate_department_report_chart(stats_by_dept, filename='dept_report.png'):
    depts = [s['department'] for s in stats_by_dept]
    n = len(depts)
    if n == 0:
        return None

    fig, axes = plt.subplots(2, 2, figsize=(14, 10))

    regs = [s.get('registration_count', 0) for s in stats_by_dept]
    axes[0, 0].bar(depts, regs, color='#42a5f5')
    axes[0, 0].set_title('各科室挂号量', fontsize=13, fontweight='bold')
    axes[0, 0].set_ylabel('挂号数')
    plt.setp(axes[0, 0].xaxis.get_majorticklabels(), rotation=30, ha='right')

    revenues = [s.get('revenue_yuan', 0) for s in stats_by_dept]
    axes[0, 1].bar(depts, revenues, color='#66bb6a')
    axes[0, 1].set_title('各科室挂号收入 (元)', fontsize=13, fontweight='bold')
    plt.setp(axes[0, 1].xaxis.get_majorticklabels(), rotation=30, ha='right')

    docs = [s.get('doctor_count', 0) for s in stats_by_dept]
    axes[1, 0].bar(depts, docs, color='#ffa726')
    axes[1, 0].set_title('各科室医生数', fontsize=13, fontweight='bold')
    plt.setp(axes[1, 0].xaxis.get_majorticklabels(), rotation=30, ha='right')

    cons = [s.get('consultation_count', 0) for s in stats_by_dept]
    axes[1, 1].bar(depts, cons, color='#ab47bc')
    axes[1, 1].set_title('各科室看诊量', fontsize=13, fontweight='bold')
    plt.setp(axes[1, 1].xaxis.get_majorticklabels(), rotation=30, ha='right')

    plt.tight_layout()
    path = os.path.join(CHARTS_DIR, filename)
    plt.savefig(path, dpi=150)
    plt.close()
    return path


# medicine_analysis: from analyze_medicine_inventory()
def generate_medicine_pie_chart(medicine_analysis, filename='medicines.png'):
    total_low = len(medicine_analysis.get('low_stock_items', []))
    total_expired = len(medicine_analysis.get('expired_items', []))
    total_types = medicine_analysis.get('total_types', 0)
    total_normal = max(0, total_types - total_low - total_expired)

    if total_normal + total_low + total_expired == 0:
        return None

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

    labels = ['正常', '低库存', '已过期']
    sizes = [total_normal, total_low, total_expired]
    colors = ['#4caf50', '#ff9800', '#f44336']
    explode = (0, 0.05, 0.1)
    ax1.pie(sizes, explode=explode, labels=labels, colors=colors, autopct='%1.1f%%',
            shadow=True, startangle=90)
    ax1.set_title('药品库存状态分布', fontsize=14, fontweight='bold')

    ax2.bar(labels, sizes, color=colors)
    ax2.set_ylabel('药品种类数', fontsize=12)
    ax2.set_title('库存状态统计', fontsize=14, fontweight='bold')
    for i, v in enumerate(sizes):
        ax2.text(i, v + 0.3, str(v), ha='center', fontsize=11)

    plt.tight_layout()
    path = os.path.join(CHARTS_DIR, filename)
    plt.savefig(path, dpi=150)
    plt.close()
    return path
