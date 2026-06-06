"""LangChain ChatOpenAI 工厂"""
from langchain_openai import ChatOpenAI
from core.config import settings

_llm_cache = {}


def get_llm(temperature=None):
    cfg = settings.llm
    temp = temperature if temperature is not None else cfg.get("temperature", 0.3)
    if temp not in _llm_cache:
        _llm_cache[temp] = ChatOpenAI(
            api_key=cfg.get("api_key", ""),
            base_url=cfg.get("base_url", "https://dashscope.aliyuncs.com/compatible-mode/v1"),
            model=cfg.get("model", "qwen-turbo"),
            temperature=temp,
            max_tokens=cfg.get("max_tokens", 1024),
        )
    return _llm_cache[temp]
