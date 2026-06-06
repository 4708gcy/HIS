# HIS AI Service v2.0 Refactor Design

## 背景

重构 HIS 项目 `ai_service/` 模块，从传统 Flask + 统计模型升级为 FastAPI + LLM 增强的智能分析服务。设计思路来源于 `Model-Learning-Source` 中学习的大模型工程化代码。

## 提炼的工程模式

| 来源 | 学到的模式 | 应用到 HIS |
|---|---|---|
| `11_文本分类_fastapi.py` | 多策略路由（ML + LLM） | 预测/异常/优化支持 traditional / llm / auto 三策略 |
| `01-intent-classify/prompt.py` | 动态 Few-shot Prompt（TF-IDF 找相似样本） | 预测时实时从历史数据找相似场景拼 prompt |
| `04-government-rag` | RAG 架构（PDF解析→分块→Embedding→检索→生成） | 医院运营知识库问答 |
| `es_kb_api.py` | Dify 集成格式 + Depends 认证 | 统一 API 标准 + API Key 注入 |
| `convert_pdf.py` | 异步文档处理 + 流式响应 | 知识库文档上传异步处理 |
| `fastapi-student-crud-demo` | FastAPI + Pydantic 数据校验 | 全服务迁移到 FastAPI |
| `08_Qwen大模型调用.py` | OpenAI 兼容 API | LLM 客户端封装（支持阿里云百炼） |

## 架构

```
ai_service/
├── config.yaml                  # 配置中心
├── main.py                      # FastAPI 入口
├── schemas.py                   # Pydantic 模型
├── core/
│   ├── config.py                # YAML 加载
│   ├── response.py              # 统一响应格式
│   ├── database.py              # MySQL 连接池
│   ├── security.py              # API Key 认证（Depends）
│   └── exceptions.py            # 自定义异常
├── services/
│   ├── data_loader.py           # 数据加载
│   ├── analyzer.py              # 传统算法（Holt-Winters、Z-score）
│   ├── llm_client.py            # LLM 客户端
│   ├── prompt_builder.py        # 动态 Few-shot Prompt
│   ├── predictor.py             # 预测引擎（traditional / llm / auto）
│   ├── anomaly_detector.py      # 异常检测引擎
│   ├── bed_optimizer.py         # 床位优化引擎
│   ├── report_generator.py      # 综合报告生成
│   └── rag_engine.py            # RAG 引擎（faiss 轻量版）
├── routers/
│   ├── predictions.py
│   ├── anomalies.py
│   ├── beds.py
│   ├── dashboard.py
│   ├── charts.py
│   └── rag.py
├── knowledge_base/              # 知识库文档存储
└── tests/
```

## 核心设计

### 1. 多策略路由

每个分析接口支持 `strategy` 参数：
- `traditional` → Holt-Winters / Z-score / 利用率计算
- `llm` → LLM 动态 Few-shot 分析
- `auto` → 根据数据量自动选择

### 2. 动态 Few-shot Prompt

1. 从历史运营数据中用 TF-IDF/Embedding 找与当前场景最相似的 3-5 条记录
2. 格式化为 few-shot examples
3. 拼进 system prompt + user prompt
4. LLM 基于"历史相似情况 + 当前数据"推理

### 3. RAG 知识库

- 文档上传：异步 PDF/Word 解析（BackgroundTasks）
- 分块：滑动窗口 + 重叠
- 向量库：faiss（轻量）
- 检索：Embedding 相似度 + 元数据筛选
- 生成：检索片段 → RAG prompt → LLM 生成

### 4. 统一响应格式

```python
class HISResponse(BaseModel):
    request_id: str
    data: Optional[Dict]
    response_code: int
    response_msg: str
    process_status: str
    processing_time: float
```

## API 端点

| 端点 | 功能 |
|---|---|
| `POST /api/v2/predictions?strategy=llm` | LLM 增强需求预测 |
| `POST /api/v2/anomalies?strategy=llm` | 异常检测 + LLM 解读 |
| `POST /api/v2/beds?strategy=llm` | 床位优化 + LLM 报告 |
| `POST /api/v2/dashboard` | 综合仪表盘（含 LLM summary） |
| `POST /api/v2/rag/chat` | 医院运营知识库问答 |
| `POST /api/v2/rag/upload` | 上传文档到知识库（异步） |
| `GET /api/v2/charts/{type}` | PNG 图表 |
| `GET /health` | 健康检查 |

## 技术栈

- FastAPI + Pydantic
- faiss-cpu（向量检索）
- sentence-transformers（Embedding）
- openai（LLM 客户端）
- PyPDF2 / python-docx（文档解析）
- jieba + sklearn（TF-IDF 相似度）
