"""
LangChain LLM Provider
参考来源：02_LangChain_1_调用模型.py（ChatOpenAI / init_chat_model）
"""
from langchain_openai import ChatOpenAI
from core.config import settings


def get_llm(temperature: float = None) -> ChatOpenAI:
    """获取配置好的 LangChain ChatOpenAI 模型"""
    cfg = settings.llm
    return ChatOpenAI(
        api_key=cfg.get("api_key", ""),
        base_url=cfg.get("base_url", "https://dashscope.aliyuncs.com/compatible-mode/v1"),
        model=cfg.get("model", "qwen-turbo"),
        temperature=temperature if temperature is not None else cfg.get("temperature", 0.3),
        max_tokens=cfg.get("max_tokens", 1024),
    )
